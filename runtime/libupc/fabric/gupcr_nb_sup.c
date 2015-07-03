/*===-- gupcr_nb_sup.c - UPC Runtime Support Library ---------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include "malloc.h"
#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_lib.h"
#include "gupcr_sup.h"
#include "gupcr_fabric.h"
#include "gupcr_gmem.h"
#include "gupcr_iface.h"
#include "gupcr_utils.h"
#include "gupcr_nb_sup.h"

/**
 * @file gupcr_nb_sup.c
 * GUPC Libfabric non-blocking transfers support routines.
 *
 * @addtogroup NON-BLOCKING GUPCR Non-Blocking Transfer Support Functions
 * @{
 */

/** Index of the local memory location */
#define GUPCR_LOCAL_INDEX(addr) \
	(void *) ((char *) addr - (char *) USER_PROG_MEM_START)

/** Fabric communications endpoint resources */
/** RX Endpoint comm resource  */
static fab_ep_t gupcr_nb_rx_ep;
/** TX Endpoint comm resource for explicit non-blocking  */
static fab_ep_t gupcr_nb_tx_ep;
/** TX Endpoint comm resource for implicit non-blocking  */
static fab_ep_t gupcr_nbi_tx_ep;
/** NB transfers MR handle */
static fab_mr_t gupcr_nb_mr;
#if MR_LOCAL_NEEDED
/** NB local memory access memory region  */
static fab_mr_t gupcr_atomic_lmr;
#endif
/** NB implicit counter */
static fab_cntr_t gupcr_nbi_ct;
/** NB implicit completion queue */
static fab_cq_t gupcr_nbi_cq;
/** NB explicit completion queue */
static fab_cq_t gupcr_nb_cq;

/** Start of the explicit non-blocking MD */
/* static char *gupcr_nb_mr_start; */

/** Implicit non-blocking number of received ACKs on local mr */
static size_t gupcr_nbi_count;
/** Start of the implicit non-blocking MD */
static char *gupcr_nbi_mr_start;

/* All non-blocking transfers with explicit handle
   are managed through the 'gupcr_nbcb' structure
   (control block).  Free control blocks are on
   the free list, while those with active transfers
   are on the 'active' list.  A single linked list
   is used to link CBs on the free or active list.

   Non-blocking transfer handle is an unsigned long
   number that we increment every time a new transfer is
   requested.  */

/** Non-blocking transfers control block */
struct gupcr_nbcb
{
  struct gupcr_nbcb *next; /** forward link on the free or used list */
  unsigned long id; /** UPC handle for non-blocking transfer */
  int status; /** non-blocking transfer status */
};
typedef struct gupcr_nbcb gupcr_nbcb_t;
typedef struct gupcr_nbcb *gupcr_nbcb_p;

/** nb transfer status value */
#define	NB_STATUS_COMPLETED 1
#define	NB_STATUS_NOT_COMPLETED 0

/** NB handle values */
unsigned long gupcr_nb_handle_next;

/** NB cb free list */
gupcr_nbcb_p gupcr_nbcb_cb_free = NULL;
/** List of NB active transfers */
gupcr_nbcb_p gupcr_nbcb_active = NULL;

/** Number of outstanding transfers with explicit handle */
int gupcr_nb_outstanding;
void gupcr_nb_check_outstanding (void);

/**
 * Allocate free NB control block
 */
static gupcr_nbcb_p
gupcr_nbcb_alloc (void)
{
  gupcr_nbcb_p cb;
  if (gupcr_nbcb_cb_free)
    {
      cb = gupcr_nbcb_cb_free;
      gupcr_nbcb_cb_free = cb->next;
    }
  else
    {
      /* Allocate memory for the new block.  */
      cb = calloc (sizeof (struct gupcr_nbcb), 1);
      if (cb == NULL)
	gupcr_fatal_error ("cannot allocate local memory");
    }
  return cb;
}

/**
 * Place NB control block on the free list
 */
static void
gupcr_nbcb_free (gupcr_nbcb_p cb)
{
  cb->next = gupcr_nbcb_cb_free;
  gupcr_nbcb_cb_free = cb;
}

/**
 * Place NB control block on the active list
 */
static void
gupcr_nbcb_active_insert (gupcr_nbcb_p cb)
{
  cb->next = gupcr_nbcb_active;
  gupcr_nbcb_active = cb;
}

/**
 * Remove NB control block from the active list
 */
static void
gupcr_nbcb_active_remove (gupcr_nbcb_p cb)
{
  gupcr_nbcb_p acb = gupcr_nbcb_active;
  gupcr_nbcb_p prev_acb = acb;
  while (acb)
    {
      if (acb == cb)
	{
	  if (acb == gupcr_nbcb_active)
	    gupcr_nbcb_active = acb->next;
	  else
	    prev_acb->next = acb->next;
	  return;
	}
      prev_acb = acb;
      acb = acb->next;
    }
}

/**
 * Find NB control block on the active list
 */
static gupcr_nbcb_p
gupcr_nbcb_find (unsigned long id)
{
  gupcr_nbcb_p cb = gupcr_nbcb_active;
  while (cb)
    {
      if (cb->id == id)
	return cb;
      cb = cb->next;
    }
  return NULL;
}

/**
 * Non-blocking GET operation
 *
 * @param[in] sthread Source thread
 * @param[in] soffset Source offset
 * @param[in] dst_ptr Destination local pointer
 * @param[in] size Number of bytes to transfer
 * @param[in] handle Transfer handle (NULL for implicit)
 */
void
gupcr_nb_get (size_t sthread, size_t soffset, char *dst_ptr,
	      size_t size, unsigned long *handle)
{
  size_t n_rem = size;
  size_t local_offset = dst_ptr - gupcr_nbi_mr_start;

  if (handle)
    {
      gupcr_nbcb_p cb = gupcr_nbcb_alloc ();
      cb->id = gupcr_nb_handle_next++;
      cb->status = NB_STATUS_NOT_COMPLETED;
      gupcr_nbcb_active_insert (cb);
      *handle = cb->id;
      gupcr_nb_check_outstanding ();
    }
  gupcr_debug (FC_NB, "%s %lu:0x%lx(%ld) -> 0x%lx (%lu)",
	       handle ? "NB" : "NBI", sthread, soffset,
	       size, (long unsigned int) dst_ptr, handle ? *handle : 0);

  /* Large transfers must be done in chunks.  Only the last chunk
     behaves as a non-blocking transfer.  */
  while (n_rem > 0)
    {
      ssize_t rsize;
      size_t n_xfer;
      n_xfer = GUPCR_MIN (n_rem, GUPCR_MAX_MSG_SIZE);
      if (handle)
	{
	  gupcr_fabric_size_call (fi_read, rsize,
				  (gupcr_nb_tx_ep,
				   GUPCR_LOCAL_INDEX (dst_ptr), n_xfer, NULL,
				   fi_rx_addr ((fi_addr_t) sthread,
					       GUPCR_SERVICE_NB,
					       GUPCR_SERVICE_BITS), soffset,
				   GUPCR_MR_NB, (void *) *handle));
	  gupcr_nb_outstanding += 1;
	}
      else
	{
	  gupcr_fabric_size_call (fi_read, rsize,
				  (gupcr_nbi_tx_ep,
				   GUPCR_LOCAL_INDEX (dst_ptr), n_xfer, NULL,
				   fi_rx_addr ((fi_addr_t) sthread,
					       GUPCR_SERVICE_NB,
					       GUPCR_SERVICE_BITS), soffset,
				   GUPCR_MR_NB, NULL));
	  gupcr_nbi_count += 1;
	}
      n_rem -= n_xfer;
      local_offset += n_xfer;
      if (n_rem)
	{
	  /* Unfortunately, there are more data to transfer, we have to
	     wait for all non-blocking transfers to complete.  */
	  if (handle)
	    gupcr_sync (*handle);
	  else
	    gupcr_synci ();
	}
    }
}

/**
 * Non-blocking transfer PUT operation
 *
 * @param[in] dthread Destination thread
 * @param[in] doffset Destination offset
 * @param[in] src_ptr Source local pointer
 * @param[in] size Number of bytes to transfer
 * @param[in] handle Transfer handle (NULL for implicit)
 */
void
gupcr_nb_put (size_t dthread, size_t doffset, const void *src_ptr,
	      size_t size, unsigned long *handle)
{
  size_t n_rem = size;
  size_t local_offset = (char *) src_ptr - gupcr_nbi_mr_start;

  if (handle)
    {
      gupcr_nbcb_p cb = gupcr_nbcb_alloc ();
      cb->id = gupcr_nb_handle_next++;
      cb->status = NB_STATUS_NOT_COMPLETED;
      gupcr_nbcb_active_insert (cb);
      *handle = cb->id;
      gupcr_nb_check_outstanding ();
    }

  gupcr_debug (FC_NB, "%s 0x%lx(%ld) -> %lu:0x%lx (%lu)",
	       handle ? "NB" : "NBI", (long unsigned int) src_ptr, size,
	       dthread, doffset, handle ? *handle : 0);

  /* Large transfers must be done in chunks.  Only the last chunk
     behaves as a non-blocking transfer.  */
  while (n_rem > 0)
    {
      ssize_t ssize;
      size_t n_xfer;
      n_xfer = GUPCR_MIN (n_rem, GUPCR_MAX_MSG_SIZE);
      if (handle)
	{
	  gupcr_fabric_size_call (fi_write, ssize,
				  (gupcr_nb_tx_ep,
				   GUPCR_LOCAL_INDEX (src_ptr), n_xfer, NULL,
				   fi_rx_addr ((fi_addr_t) dthread,
					       GUPCR_SERVICE_NB,
					       GUPCR_SERVICE_BITS), doffset,
				   GUPCR_MR_NB, (void *) *handle));
	  gupcr_nb_outstanding += 1;
	}
      else
	{
	  gupcr_fabric_size_call (fi_write, ssize,
				  (gupcr_nbi_tx_ep,
				   GUPCR_LOCAL_INDEX (src_ptr), n_xfer, NULL,
				   fi_rx_addr ((fi_addr_t) dthread,
					       GUPCR_SERVICE_NB,
					       GUPCR_SERVICE_BITS), doffset,
				   GUPCR_MR_NB, NULL));
	  gupcr_nbi_count += 1;
	}
      n_rem -= n_xfer;
      local_offset += n_xfer;
      if (n_rem)
	{
	  /* Unfortunately, there are more data to transfer, we have to
	     wait for all non-blocking transfers to complete.  */
	  if (handle)
	    gupcr_sync (*handle);
	  else
	    gupcr_synci ();
	}
    }
}

/**
 * Check for the max number of outstanding non-blocking
 * transfers with explicit handle
 *
 * We cannot allow for number of outstanding transfers
 * to go over the event queue size.  Otherwise, some ACK/REPLY
 * can be dropped.
 */
void
gupcr_nb_check_outstanding (void)
{
  int pstatus = 0;
  gupcr_nbcb_p cb;

  /* Wait for completion if MAX outstanding.  */
  if (gupcr_nb_outstanding == GUPCR_NB_MAX_OUTSTANDING)
    {
      do
	{
	  struct fi_cq_entry nb_context;
	  /* Wait for one completion.  */
	  gupcr_fabric_call_nc (fi_cq_sread, pstatus, (gupcr_nb_cq,
						       &nb_context, 1, NULL,
						       0));
	  switch (pstatus)
	    {
	    case 1:
	      {
		unsigned long id = (unsigned long) nb_context.op_context;
		gupcr_debug (FC_NB, "received event ID %ld", id);
		cb = gupcr_nbcb_find (id);
		if (!cb || cb->status == NB_STATUS_COMPLETED)
		  gupcr_fatal_error ("received event for invalid or"
				     " already completed NB handle");
		cb->status = NB_STATUS_COMPLETED;
		gupcr_nb_outstanding--;
	      }
	      break;
	    case -FI_EAGAIN:
	      break;
	    default:
	      gupcr_process_fail_events (pstatus, "nb check outstanding",
					 gupcr_nb_cq);
	    }
	}
      while (pstatus != 1);
    }
}

/**
 * Check for non-blocking transfer complete
 *
 * @param[in] handle Transfer handle
 * @retval "1" if transfer completed
 */
int
gupcr_nb_completed (unsigned long handle)
{
  ssize_t cnt;
  gupcr_nbcb_p cb;

  /* Any fabric completion event?  */
  do
    {
      struct fi_cq_entry nb_context;
      gupcr_fabric_call_nc (fi_cq_read, cnt, (gupcr_nb_cq,
						&nb_context, 1));
      switch (cnt)
	{
	case 1:
	  {
	    unsigned long id = (unsigned long) nb_context.op_context;
	    gupcr_debug (FC_NB, "received event ID %ld", id);
	    cb = gupcr_nbcb_find (id);
	    if (!cb || cb->status == NB_STATUS_COMPLETED)
	      gupcr_fatal_error ("received event for invalid or"
				 " already completed NB handle");
	    cb->status = NB_STATUS_COMPLETED;
	    gupcr_nb_outstanding--;
	  }
	  break;
	case -FI_EAGAIN:
	  break;
	default:
	  gupcr_process_fail_events (-cnt, "nb sync", gupcr_nb_cq);
	}
    }
  while (cnt == 1);

  /* Check if transfer is completed.  */
  cb = gupcr_nbcb_find (handle);
  if (cb && cb->status == NB_STATUS_COMPLETED)
    {
      gupcr_nbcb_active_remove (cb);
      gupcr_nbcb_free (cb);
      return 1;
    }

  return 0;
}

/**
 * Complete non-blocking transfers with explicit handle
 *
 * Wait for outstanding request to complete.
 *
 * @param[in] handle Transfer handle
 */
void
gupcr_sync (unsigned long handle)
{
  gupcr_nbcb_p cb;
  ssize_t cnt;

  gupcr_debug (FC_NB, "waiting for handle %lu", handle);
  /* Check if transfer already completed.  */
  cb = gupcr_nbcb_find (handle);
  if (!cb)
    {
      /* Handle does not exist.  Assume it is a duplicate
         sync request.  */
      return;
    }
  if (cb->status == NB_STATUS_COMPLETED)
    {
      /* Already completed.  */
      gupcr_nbcb_active_remove (cb);
      gupcr_nbcb_free (cb);
    }
  else
    {
      /* Must wait for fabric to complete the transfer.  */
      for (;;)
	{
	  struct fi_cq_entry nb_context;
	  gupcr_fabric_call_nc (fi_cq_sread, cnt, (gupcr_nb_cq,
						   &nb_context, 1, NULL,
						   0));
	  gupcr_debug (FC_NB, "received count %ld", cnt);
	  switch (cnt)
	    {
	    case 1:
	      {
		unsigned long id = (unsigned long) nb_context.op_context;
		gupcr_debug (FC_NB, "received event ID %ld", id);
		cb = gupcr_nbcb_find (id);
		if (!cb || cb->status == NB_STATUS_COMPLETED)
		  gupcr_fatal_error ("received event for nonexistent or"
				     " already completed NB handle");
		cb->status = NB_STATUS_COMPLETED;
		gupcr_nb_outstanding--;
		if (id == handle)
		  {
		    gupcr_nbcb_active_remove (cb);
		    gupcr_nbcb_free (cb);
		    return;
		  }
	      }
	      break;
	    case -FI_EAGAIN:
	      break;
	    default:
	      gupcr_process_fail_events (-cnt, "nb sync", gupcr_nb_cq);
	    }
	}
    }
}

/**
 * Check for any outstanding implicit handle non-blocking transfer
 *
 * @retval Number of outstanding transfers
 */
int
gupcr_nbi_outstanding (void)
{
  uint64_t cnt;
  gupcr_fabric_call_nc (fi_cntr_read, cnt, (gupcr_nbi_ct));
  return (int) (gupcr_nbi_count - cnt);
}

/**
 * Complete non-blocking transfers with implicit handle
 *
 * Wait for all outstanding requests to complete.
 */
void
gupcr_synci (void)
{
  int status;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_nbi_ct, gupcr_nbi_count,
			 GUPCR_TRANSFER_TIMEOUT));
  GUPCR_CNT_ERROR_CHECK (status, "nbi sync", gupcr_nbi_cq);

}

/**
 * Initialize non-blocking transfer resources
 * @ingroup INIT
 */
void
gupcr_nb_init (void)
{
  cntr_attr_t cntr_attr = { 0 };
  cq_attr_t cq_attr = { 0 };
  tx_attr_t tx_attr = { 0 };
  rx_attr_t rx_attr = { 0 };

  gupcr_log (FC_NB, "non-blocking transfer init called");

  /* Create context endpoints for NB/NBI transfers.  */
  tx_attr.op_flags = FI_DELIVERY_COMPLETE;
  gupcr_fabric_call (fi_tx_context,
		     (gupcr_ep, GUPCR_SERVICE_NB, &tx_attr, &gupcr_nb_tx_ep,
		      NULL));
  gupcr_fabric_call (fi_tx_context,
		     (gupcr_ep, GUPCR_SERVICE_NB, &tx_attr, &gupcr_nbi_tx_ep,
		      NULL));
  gupcr_fabric_call (fi_rx_context,
		     (gupcr_ep, GUPCR_SERVICE_NB, &rx_attr, &gupcr_nb_rx_ep,
		      NULL));

  /* ... and memory region for remote inbound accesses.  */
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, gupcr_gmem_base, gupcr_gmem_size,
				 FI_REMOTE_READ | FI_REMOTE_WRITE, 0,
				 GUPCR_MR_NB, 0, &gupcr_nb_mr, NULL));

  /* Enable RX endpoint and bind MR.  */
  gupcr_fabric_call (fi_enable, (gupcr_nb_rx_ep));
  gupcr_fabric_call (fi_ep_bind, (gupcr_nb_rx_ep, &gupcr_nb_mr->fid,
				  FI_REMOTE_READ | FI_REMOTE_WRITE));

  /* ... and completion cq for remote NB read/write.  Completion counter
     is not used for NB explicit transfers.  */
  cq_attr.size = GUPCR_NB_MAX_OUTSTANDING;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr, &gupcr_nb_cq, NULL));
  /* Use FI_COMPLETION flag to report all completions by default.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_nb_tx_ep, &gupcr_nb_cq->fid,
				  FI_WRITE | FI_READ | FI_COMPLETION));

  /* ... and completion cntr/cq for remote NBI read/write.  */
  cntr_attr.events = FI_CNTR_EVENTS_COMP;
  cntr_attr.flags = 0;
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr,
				    &gupcr_nbi_ct, NULL));
  gupcr_fabric_call (fi_ep_bind, (gupcr_nbi_tx_ep, &gupcr_nbi_ct->fid,
				  FI_READ | FI_WRITE));
  /* Reset number of acknowledgments.  */
  gupcr_nbi_count = 0;

  /* ... and completion queue for remote target transfer errors.  */
  cq_attr.size = 1;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr, &gupcr_nbi_cq, NULL));
  /* Use FI_SELECTIVE_COMPLETION flag to report errors only.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_nbi_tx_ep, &gupcr_nbi_cq->fid,
				  FI_WRITE | FI_READ |
				  FI_SELECTIVE_COMPLETION));

#if LOCAL_MR_NEEDED
  /* NOTE: Create a local memory region before enabling endpoint.  */
  /* ... and memory region for local memory accesses.  */
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, USER_PROG_MEM_START,
				 USER_PROG_MEM_SIZE, FI_READ | FI_WRITE,
				 0, 0, 0, &gupcr_nb_lmr, NULL));
  /* NOTE: There is no need to bind local memory region to endpoint.  */
  /*       Hmm ... ? We can probably use only one throughout the runtime,  */
  /*       as counters and events are bound to endpoint.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_nb_tx_ep, &gupcr_nb_lmr->fid,
				  FI_READ | FI_WRITE));
  gupcr_fabric_call (fi_ep_bind, (gupcr_nbi_tx_ep, &gupcr_nb_lmr->fid,
				  FI_READ | FI_WRITE));
#endif

  /* Enable TX endpoints.  */
  gupcr_fabric_call (fi_enable, (gupcr_nb_tx_ep));
  gupcr_fabric_call (fi_enable, (gupcr_nbi_tx_ep));

  /* Initialize NB handle values.  */
  gupcr_nb_handle_next = 1;
  /* Initialize number of outstanding transfers.  */
  gupcr_nb_outstanding = 0;

  gupcr_log (FC_NB, "non-blocking transfer init completed");
}

/**
 * Release non-blocking transfer resources
 * @ingroup INIT
 */
void
gupcr_nb_fini (void)
{
  int status;
  gupcr_log (FC_NB, "non-blocking transfer fini called");
  gupcr_fabric_call (fi_close, (&gupcr_nbi_ct->fid));
  gupcr_fabric_call (fi_close, (&gupcr_nbi_cq->fid));
  gupcr_fabric_call (fi_close, (&gupcr_nb_cq->fid));
  gupcr_fabric_call (fi_close, (&gupcr_nb_mr->fid));
#if LOCAL_MR_NEEDED
  gupcr_fabric_call (fi_close, (&gupcr_nb_lmr->fid));
#endif
  /* NOTE: Do not check for errors.  Fails occasionally.  */
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_nb_rx_ep->fid));
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_nb_tx_ep->fid));
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_nbi_tx_ep->fid));
  gupcr_log (FC_NB, "non-blocking transfer fini completed");
}

/** @} */
