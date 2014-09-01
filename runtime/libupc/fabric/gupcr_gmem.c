/*===-- gupcr_gmem.c - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_gmem.c
 * GUPC Libfabric shared memory interface.
 */

/**
 * @addtogroup GMEM GUPCR Shared Memory Access
 * @{
 */

#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_sup.h"
#include "gupcr_fabric.h"
#include "gupcr_node.h"
#include "gupcr_gmem.h"
#include "gupcr_utils.h"
#include "gupcr_sync.h"

/** Thread's default shared heap size */
#define GUPCR_GMEM_DEFAULT_HEAP_SIZE 256*1024*1024

/** Shared memory base and size */
void *gupcr_gmem_base;
size_t gupcr_gmem_size;

/** GET event tracking */
gupcr_gmem_xfer_info_t gupcr_gmem_gets;
/** PUT event tracking */
gupcr_gmem_xfer_info_t gupcr_gmem_puts;

/** PUT "bounce buffer" type */
typedef char gupcr_gmem_put_bb_t[GUPCR_BOUNCE_BUFFER_SIZE];
/** PUT "bounce buffer" space */
static gupcr_gmem_put_bb_t gupcr_gmem_put_bb;
/** PUT "bounce buffer" memory region descriptor handle */
static struct fid_mr gupcr_gmem_put_bb_mr;
/** PUT "bounce buffer" used counter */
size_t gupcr_gmem_put_bb_used;

/** Previous operation was a strict put */
int gupcr_pending_strict_put;

/** Heap base offset relative to start of UPC shared region */
size_t gupcr_gmem_heap_base_offset;

/** Size of UPC shared region reserved for the heap */
size_t gupcr_gmem_heap_size;

/** Remote puts flow control */
static const size_t gupcr_gmem_high_mark_puts = GUPCR_MAX_OUTSTANDING_PUTS;
static const size_t gupcr_gmem_low_mark_puts = GUPCR_MAX_OUTSTANDING_PUTS / 2;

/**
 * Allocate memory for this thread's shared space contribution.
 *
 * Calculate needed memory size and let the node allocate
 * shared memory and map other thread's shared memory into
 * the current thread memory space.
 */
static void
gupcr_gmem_alloc_shared (void)
{
  size_t heap_size = GUPCR_ROUND (gupcr_get_shared_heap_size (), C64K);
  size_t data_size = GUPCR_ROUND (GUPCR_SHARED_SECTION_END -
				  GUPCR_SHARED_SECTION_START, C64K);
  gupcr_gmem_heap_base_offset = data_size;
  gupcr_gmem_heap_size = heap_size;
  gupcr_gmem_size = heap_size + data_size;

  /* Allocate this thread's shared space.  */
  gupcr_gmem_base = gupcr_node_local_alloc (gupcr_gmem_size);
}

/**
 * Complete all outstanding remote GET operations.
 *
 * This procedure waits for all outstanding GET operations
 * to complete.  If the wait on the GET counting event returns
 * a failure, a full event queue is checked for failure specifics
 * and the program aborts.
 */
void
gupcr_gmem_sync_gets (void)
{
  /* Sync all outstanding local accesses.  */
  GUPCR_MEM_BARRIER ();
  /* Sync all outstanding remote get accesses.  */
  if (gupcr_gmem_gets.num_pending > 0)
    {
      size_t num_initiated =
	gupcr_gmem_gets.num_completed + gupcr_gmem_gets.num_pending;
      gupcr_debug (FC_MEM, "outstanding gets: %lu",
		   (long unsigned) gupcr_gmem_gets.num_pending);
      // TODO - wait for outstanding gets
      // TODO - check for errors
      gupcr_gmem_gets.num_pending = 0;
      gupcr_gmem_gets.num_completed = num_initiated;
    }
}

/**
 * Complete outstanding remote PUT operations.
 *
 * This procedure waits for all outstanding PUT operations
 * to complete.  If the wait on the Portals PUT counting event returns
 * a failure, a full event queue is checked for failure specifics
 * and the program aborts.
 */
void
gupcr_gmem_sync_puts (void)
{
  /* Sync all outstanding local accesses.  */
  GUPCR_MEM_BARRIER ();
  /* Sync all outstanding remote put accesses.  */
  if (gupcr_gmem_puts.num_pending > 0)
    {
      size_t num_initiated =
	gupcr_gmem_puts.num_completed + gupcr_gmem_puts.num_pending;
      gupcr_debug (FC_MEM, "outstanding puts: %lu",
		   (long unsigned) gupcr_gmem_puts.num_pending);
      // TODO - wait for outstanding puts
      // TODO - check for errors
      gupcr_gmem_puts.num_pending = 0;
      gupcr_gmem_puts.num_completed = num_initiated;
      gupcr_pending_strict_put = 0;
      gupcr_gmem_put_bb_used = 0;
    }
}

/**
 * Complete all outstanding remote operations.
 *
 * Check and wait for completion of all PUT/GET operations.
 */
void
gupcr_gmem_sync (void)
{
  gupcr_gmem_sync_gets ();
  gupcr_gmem_sync_puts ();
}

/**
 * Read data from remote shared memory.
 *
 * A GET request is broken into multiple GET requests
 * if the number of requested bytes is greater then
 * the configuration limited maximum message size.
 *
 * @param [in] dest Local memory to receive remote data
 * @param [in] thread Remote thread to request data from
 * @param [in] offset Remote address
 * @param [in] n Number of bytes to transfer
 */
void
gupcr_gmem_get (void *dest, int thread, size_t offset, size_t n)
{
  char *dest_addr = (char *)dest - (size_t) USER_PROG_MEM_START;
  size_t n_rem = n;

  gupcr_debug (FC_MEM, "%d:0x%lx 0x%lx",
	       thread, (long unsigned) offset, (long unsigned) dest);
  while (n_rem > 0)
    {
      size_t n_xfer;
      n_xfer = GUPCR_MIN (n_rem, (size_t) GUPCR_MAX_MSG_SIZE);
      ++gupcr_gmem_gets.num_pending;
      // TODO - get data
      n_rem -= n_xfer;
      dest_addr += n_xfer;
    }
}

/**
 * Write data to remote shared memory.
 *
 * For data requests smaller then maximum safe size, the data is first
 * copied into a bounce buffer.  In this way, the put operation
 * can be non-blocking and there are no restrictions placed upon
 * the caller's use of the source data buffer.
 * Otherwise,  a synchronous operation is performed
 * and this function returns to the caller after the operation completes.
 *
 * @param [in] thread Destination thread
 * @param [in] offset Destination offset
 * @param [in] src Local source pointer to data
 * @param [in] n Number of bytes to transfer
 */
void
gupcr_gmem_put (int thread, size_t offset, const void *src, size_t n)
{
  int must_sync = (n > GUPCR_GMEM_MAX_SAFE_PUT_SIZE);
  char *src_addr = (char *) src;
  size_t n_rem = n;
  gupcr_debug (FC_MEM, "0x%lx %d:0x%lx",
                       (long unsigned) src, thread, (long unsigned) offset);
  /* Large puts must be synchronous, to ensure that it is
     safe to re-use the source buffer upon return.  */
  while (n_rem > 0)
    {
      size_t n_xfer;
      struct fid_mr mr_handle;
      size_t local_offset;
      n_xfer = GUPCR_MIN (n_rem, (size_t) GUPCR_MAX_MSG_SIZE);
      if (must_sync)
	{
	  local_offset = src_addr - (char *) USER_PROG_MEM_START;
	  mr_handle = gupcr_gmem_puts.md;
	}
      else if (n_rem <= GUPCR_MAX_VOLATILE_SIZE)
	{
	  local_offset = src_addr - (char *) USER_PROG_MEM_START;
	  mr_handle = gupcr_gmem_puts.md_volatile;
	}
      else
	{
	  char *bounce_buf;
	  /* If this transfer will overflow the bounce buffer,
	     then first wait for all outstanding puts to complete.  */
	  if ((gupcr_gmem_put_bb_used + n_xfer) > GUPCR_BOUNCE_BUFFER_SIZE)
	    gupcr_gmem_sync_puts ();
	  bounce_buf = &gupcr_gmem_put_bb[gupcr_gmem_put_bb_used];
	  memcpy (bounce_buf, src_addr, n_xfer);
	  local_offset = bounce_buf - gupcr_gmem_put_bb;
	  gupcr_gmem_put_bb_used += n_xfer;
	  mr_handle = gupcr_gmem_put_bb_mr;
	}
      ++gupcr_gmem_puts.num_pending;
      // TODO - send data
      n_rem -= n_xfer;
      src_addr += n_xfer;

      if (gupcr_gmem_puts.num_pending == gupcr_gmem_high_mark_puts)
   	{
	  size_t complete_cnt;
	  size_t wait_cnt = gupcr_gmem_puts.num_completed
			    + gupcr_gmem_puts.num_pending
			    - gupcr_gmem_low_mark_puts;
	  // TODO - wait for partial put complete
	  // TODO - check for errors
	  // complete_cnt = ct.success - gupcr_gmem_puts.num_completed;
	  // gupcr_gmem_puts.num_pending -= complete_cnt;
	  // gupcr_gmem_puts.num_completed = ct.success;
	}
    }
  if (must_sync)
    gupcr_gmem_sync_puts ();
}

/**
 * Copy remote shared memory from the source thread
 * to the destination thread.
 *
 * Bulk copy from one thread to another.
 * The put bounce buffer is used as an intermediate buffer.
 * Caller assumes responsibility for checking the validity
 * of the remote thread id's and/or shared memory offsets.
 *
 * @param [in] dthread Destination thread
 * @param [in] doffset Destination offset
 * @param [in] sthread Source thread
 * @param [in] soffset Source offset
 * @param [in] n Number of bytes to transfer
 */
void
gupcr_gmem_copy (int dthread, size_t doffset,
		 int sthread, size_t soffset, size_t n)
{
  size_t n_rem = n;
  size_t dest_addr = doffset;
  size_t src_addr = soffset;
  gupcr_debug (FC_MEM, "%d:0x%lx %d:0x%lx %lu",
	       sthread, (long unsigned) soffset,
	       dthread, (long unsigned) doffset,
	       (long unsigned) n);
  while (n_rem > 0)
    {
      size_t n_xfer;
      char *bounce_buf;
      size_t local_offset;
      /* Use the entire put "bounce buffer" if the transfer
         count is sufficiently large.  */
      n_xfer = GUPCR_MIN (n_rem, GUPCR_BOUNCE_BUFFER_SIZE);
      if ((gupcr_gmem_put_bb_used + n_xfer) > GUPCR_BOUNCE_BUFFER_SIZE)
	gupcr_gmem_sync_puts ();
      bounce_buf = &gupcr_gmem_put_bb[gupcr_gmem_put_bb_used];
      gupcr_gmem_put_bb_used += n_xfer;
      /* Read the source data into the bounce buffer.  */
      gupcr_gmem_get (bounce_buf, sthread, src_addr, n_xfer);
      gupcr_gmem_sync_gets ();
      local_offset = bounce_buf - gupcr_gmem_put_bb;
      ++gupcr_gmem_puts.num_pending;
      // TODO - send data 
      n_rem -= n_xfer;
      src_addr += n_xfer;
      dest_addr += n_xfer;
    }
}

/**
 * Write the same byte value into the bytes of the
 * destination thread's memory at the specified offset.
 *
 * The put bounce buffer is used as an intermediate buffer.
 * The last write of a chunk of data is non-blocking.
 * Caller assumes responsibility for checking the validity
 * of the remote thread id's and/or shared memory offsets.
 *
 * @param [in] thread Destination thread
 * @param [in] offset Destination offset
 * @param [in] c Set value
 * @param [in] n Number of bytes to transfer
 */
void
gupcr_gmem_set (int thread, size_t offset, int c, size_t n)
{
  size_t n_rem = n;
  int already_filled = 0;
  size_t dest_addr = offset;
  gupcr_debug (FC_MEM, "0x%x %d:0x%lx %lu", c, thread,
                       (long unsigned) offset, (long unsigned) n);
  while (n_rem > 0)
    {
      size_t n_xfer;
      char *bounce_buf;
      size_t local_offset;
      /* Use the entire put "bounce buffer" if the transfer
         count is sufficiently large.  */
      n_xfer = GUPCR_MIN (n_rem, (size_t) GUPCR_BOUNCE_BUFFER_SIZE);
      if ((gupcr_gmem_put_bb_used + n_xfer) > GUPCR_BOUNCE_BUFFER_SIZE)
	gupcr_gmem_sync_puts ();
      bounce_buf = &gupcr_gmem_put_bb[gupcr_gmem_put_bb_used];
      gupcr_gmem_put_bb_used += n_xfer;
      /* Fill the bounce buffer, if we haven't already.  */
      if (!already_filled)
	{
	  memset (bounce_buf, c, n_xfer);
	  already_filled = (bounce_buf == gupcr_gmem_put_bb
			    && n_xfer == GUPCR_BOUNCE_BUFFER_SIZE);
	}
      local_offset = bounce_buf - gupcr_gmem_put_bb;
      ++gupcr_gmem_puts.num_pending;
      // TODO - network put
      n_rem -= n_xfer;
      dest_addr += n_xfer;
    }
}

/**
 * Initialize gmem resources.
 * @ingroup INIT
 */
void
gupcr_gmem_init (void)
{
  gupcr_log (FC_MEM, "gmem init called");
  /* Allocate memory for this thread's contribution to shared memory.  */
  gupcr_gmem_alloc_shared ();
}

/**
 * Release gmem resources.
 * @ingroup INIT
 */
void
gupcr_gmem_fini (void)
{
  gupcr_log (FC_MEM, "gmem fini called");
}

/** @} */
