/*===-- gupcr_lock_sup.c - UPC Runtime Support Library -------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_lib.h"
#include "gupcr_lock_sup.h"
#include "gupcr_sup.h"
#include "gupcr_fabric.h"
#include "gupcr_gmem.h"
#include "gupcr_utils.h"
#include "gupcr_lock_sup.h"

/**
 * @file gupcr_lock_sup.c
 * GUPC Libfabric locks implementation support routines.
 *
 * @addtogroup LOCK GUPCR Lock Functions
 * @{
 */

/** Lock memory region counter */
static size_t gupcr_lock_mr_count;

/** Lock buffer for CSWAP operation */
static char gupcr_lock_buf[16];

/**
 * Execute lock-related atomic fetch and store remote operation.
 *
 * Value "val" is written into the specified remote location and the
 * old value is returned.
 *
 * A fabric 'atomic write' operation is used when the acquiring thread must
 * atomically determine if the lock is available.  A pointer to the thread's
 * local lock waiting list link is atomically written into the lock's 'last'
 * field, and the current value of the 'last' field is returned.  If NULL,
 * the acquiring thread is the new owner, otherwise it must insert itself
 * onto the waiting list.
 */
void
gupcr_lock_swap (size_t dest_thread,
		 size_t dest_offset, void *val, void *old, size_t size)
{
  gupcr_debug (FC_LOCK, "%lu:0x%lx",
                        (long unsigned) dest_thread,
                        (long unsigned) dest_offset);
  gupcr_lock_mr_count += 1;
  // TODO - wait for completion
  // TODO - check for errors
}

/**
 * Execute a lock-related atomic compare and swap operation.
 *
 * The value  pointed to by 'val' is written into the remote location
 * given by ('dest_thread', 'dest_addr) only if value in the destination
 * is identical to 'cmp'.
 *
 * A fabric compare and swap atomic operation is used during the lock
 * release phase when the owner of the lock must atomically determine if
 * there are threads waiting on the lock.  This is accomplished by using
 * the fabric FI_CSWAP atomic operation, where a NULL pointer is written
 * into the lock's 'last' field only if this field contains the pointer
 * to the owner's local lock link structure.
 *
 * @retval Return TRUE if the operation was successful.
 */
int
gupcr_lock_cswap (size_t dest_thread,
		  size_t dest_offset, void *cmp, void *val, size_t size)
{
  gupcr_debug (FC_LOCK, "%lu:0x%lx",
                        (long unsigned) dest_thread,
			(long unsigned) dest_offset);
  // TODO - execute swap
  gupcr_lock_mr_count += 1;
  // TODO - wait for completion
  // TODO - check for errors
  return !memcmp (cmp, gupcr_lock_buf, size);
}

/*
 * Execute a network put operation on the lock-related NC.
 *
 * Execute a put operation on the NC that is reserved for
 * lock-related operations.  This separate NC is used
 * to make it possible to count only put operations on the
 * 'signal' or 'next' words of a UPC lock wait list entry.
 *
 * gupcr_lock_put() is used to 'signal' the remote thread that:
 * - ownership of the lock is passed to a remote thread if the remote
 * thread is the next thread on the waiting list
 * - a pointer to the calling thread's local control block has
 * been appended to the lock's waiting list
 */
void
gupcr_lock_put (size_t dest_thread, size_t dest_addr, void *val, size_t size)
{
  gupcr_debug (FC_LOCK, "%lu:0x%lx",
                        (long unsigned) dest_thread,
			(long unsigned) dest_addr);
  // TODO - execute put
  gupcr_lock_mr_count += 1;
  // TODO - wait for put completion
  // TODO - check for erros
}

/*
 * Execute a get operation on the lock-related NC.
 *
 * All operations on lock/link data structures must be performed
 * through the network interface to prevent data tearing.
 */
void
gupcr_lock_get (size_t dest_thread, size_t dest_addr, void *val, size_t size)
{
  gupcr_debug (FC_LOCK, "%lu:0x%lx",
                        (long unsigned) dest_thread,
			(long unsigned) dest_addr);
  // TODO - get data
  // TODO - check for error
  gupcr_lock_mr_count += 1;
}

/**
 * Wait for the next counting event to be posted to lock NC.
 *
 * This function is called when it has been determined that
 * the current thread needs to wait until the lock is is released.
 *
 * Wait until the next counting event is posted
 * to the NC reserved for this purpose and then return.
 * The caller will check whether the lock was in fact released,
 * and if not, will call this function again to wait for the
 * next lock-related event to come in.
 */
void
gupcr_lock_wait (void)
{
  gupcr_debug (FC_LOCK, "");
  // TODO - wait for signal put
  // TODO - check for error
}

/**
 * Initialize lock resources.
 * @ingroup INIT
 */
void
gupcr_lock_init (void)
{
  gupcr_log (FC_LOCK, "lock init called");

  gupcr_lock_mr_count = 0;

  gupcr_lock_link_init ();
  /* Initialize the lock free list.  */
  gupcr_lock_free_init ();
  /* Initialize the heap allocator locks.  */
  gupcr_lock_heap_sup_init ();
}

/**
 * Release lock resources.
 * @ingroup INIT
 */
void
gupcr_lock_fini (void)
{
  gupcr_log (FC_LOCK, "lock fini called");
}

/** @} */
