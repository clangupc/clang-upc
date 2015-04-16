/*===-- gupcr_barrier_sup.c - UPC Runtime Support Library ----------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_barrier_sup.c
 * Libfabric barrier support functions.
 *
 * @addtogroup BARRIER GUPCR Barrier Functions
 * @{
 */

#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_sup.h"
#include "gupcr_sync.h"
#include "gupcr_broadcast.h"
#include "gupcr_fabric.h"
#include "gupcr_iface.h"
#include "gupcr_utils.h"
#include "gupcr_runtime.h"
#include "gupcr_barrier_sup.h"


/** Index of the local memory location */
#define GUPCR_LOCAL_INDEX(addr) \
	(void *) ((char *) addr - (char *) USER_PROG_MEM_START)

/* For barrier purpose we create two endpoints (UP/DOWN) and
   bind target/local memory regions to them.  The whole user
   space is bound which allows us to pass actual addresses through
   the support interface.  */

#define DEFINE_ENDPOINTS(bar) \
	static fab_ep_t gupcr_##bar##_tx_ep; \
	static fab_mr_t gupcr_##bar##_tx_mr; \
	static fab_cntr_t gupcr_##bar##_tx_put_ct; \
	static fab_cq_t gupcr_##bar##_tx_put_cq; \
	static size_t gupcr_##bar##_tx_put_count; \
	static fab_cntr_t gupcr_##bar##_tx_get_ct; \
	static fab_cq_t gupcr_##bar##_tx_get_cq; \
	static size_t gupcr_##bar##_tx_get_count; \
	static fab_ep_t gupcr_##bar##_rx_ep; \
	static fab_mr_t gupcr_##bar##_rx_mr; \
	static fab_cntr_t gupcr_##bar##_rx_ct; \
	static fab_cq_t gupcr_##bar##_rx_cq; \
	size_t gupcr_##bar##_rx_count;

DEFINE_ENDPOINTS (bup)
DEFINE_ENDPOINTS (bdown)

/** Max number of outstanding triggered operations  */
#define GUPCR_BAR_MAX_TRIG_CTX 3
/** Triggered context structures.  Libfabric spec requires context
    data structure to be available until operation completes.  */
fi_trig_t trig_ctx[GUPCR_BAR_MAX_TRIG_CTX];

/** Send data to a remote thread
 *
 * @param [in] dir Direction (up/down)
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 */
void
gupcr_barrier_put (enum barrier_dir dir, void *src, int thread, void *dst,
		   size_t count)
{
  fab_ep_t ep = dir == BARRIER_UP ? gupcr_bup_tx_ep : gupcr_bdown_tx_ep;
  int bar_serv = dir == BARRIER_UP ?
		GUPCR_SERVICE_BARRIER_UP : GUPCR_SERVICE_BARRIER_DOWN;
  gupcr_debug (FC_BARRIER, "%lx -> %d:%lx (%ld)", (unsigned long)src,
	       thread, (unsigned long) dst, (unsigned long) count);

  if (sizeof (int) <= GUPCR_MAX_OPTIM_SIZE)
    {
      gupcr_fabric_call (fi_inject_write,
			 (ep, (const void *) src, count,
			  fi_rx_addr ((fi_addr_t) thread,
			  bar_serv, GUPCR_SERVICE_BITS),
			  (uint64_t) GUPCR_LOCAL_INDEX (dst), 0));
    }
  else
    {
      gupcr_fabric_call (fi_write,
			 (ep, (const void *) src, count, NULL,
			  fi_rx_addr ((fi_addr_t) thread,
			  bar_serv, GUPCR_SERVICE_BITS),
			  (uint64_t) GUPCR_LOCAL_INDEX (dst), 0, NULL));
    }
}

/** Wait for put completion.
 *
 * @param [in] count Wait count
 */
void
gupcr_barrier_put_wait (enum barrier_dir dir, size_t count)
{
  int status;
  fab_cntr_t ct;
  fab_cq_t cq;
  size_t wait_count;

  gupcr_debug (FC_BARRIER, "");
  if (dir == BARRIER_UP)
    {
      ct = gupcr_bup_tx_put_ct;
      cq = gupcr_bup_tx_put_cq;
      gupcr_bup_tx_put_count += count;
      wait_count = gupcr_bup_tx_put_count;
    }
  else
    {
      ct = gupcr_bdown_tx_put_ct;
      cq = gupcr_bdown_tx_put_cq;
      gupcr_bdown_tx_put_count += count;
      wait_count = gupcr_bdown_tx_put_count;
    }
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(ct, wait_count, GUPCR_TRANSFER_TIMEOUT));
  GUPCR_TIMEOUT_CHECK (status, "barrier put wait", cq);
}

/** Setup a trigger for sending data to remote thread
 *
 * @param [in] dir Barrier direction
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 * @param [in] count Number of bytes to put
 * @param [in] from_dir Barrier direction for trigger
 * @param [in] trig Trigger counter
 * @param [in] ctx Context index
 */
void
gupcr_barrier_tr_put (enum barrier_dir dir, void *src,
		      int thread, void *dst, size_t count,
		      enum barrier_dir from_dir, size_t trig, int ctx)
{
  fab_ep_t ep = dir == BARRIER_UP ? gupcr_bup_tx_ep : gupcr_bdown_tx_ep;
  int bar_serv = dir == BARRIER_UP ?
		GUPCR_SERVICE_BARRIER_UP : GUPCR_SERVICE_BARRIER_DOWN;
  gupcr_assert (ctx < GUPCR_BAR_MAX_TRIG_CTX);
  gupcr_debug (FC_BARRIER, "%lx -> %d:%lx (%ld)", (unsigned long)src,
	       thread, (unsigned long) dst, (unsigned long) count);
  trig_ctx[ctx].event_type = FI_TRIGGER_THRESHOLD;
  if (from_dir == BARRIER_UP)
    {
      gupcr_bup_rx_count += trig;
      trig_ctx[ctx].trigger.threshold.cntr = gupcr_bup_rx_ct;
      trig_ctx[ctx].trigger.threshold.threshold = gupcr_bup_rx_count;
    }
  else
    {
      gupcr_bdown_rx_count += trig;
      trig_ctx[ctx].trigger.threshold.cntr = gupcr_bdown_rx_ct;
      trig_ctx[ctx].trigger.threshold.threshold = gupcr_bdown_rx_count;
    }
  {
    struct fi_msg_rma msg_rma = {0};
    struct iovec msg = {0};
    struct fi_rma_iov endpt = {0};

    msg.iov_base = src;
    msg.iov_len = count;
    endpt.addr = (uint64_t) dst;
    msg_rma.msg_iov = &msg;
    msg_rma.addr = fi_rx_addr ((fi_addr_t) thread, bar_serv,
			       GUPCR_SERVICE_BITS);
    msg_rma.rma_iov = &endpt;
    msg_rma.context = &trig_ctx[ctx];
    gupcr_fabric_call (fi_writemsg, (ep, &msg_rma, FI_TRIGGER));
  }
}

/** Atomically send data to the remote thread
 *
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 */
void
gupcr_barrier_atomic (int *src, int thread, int *dst)
{
  gupcr_debug (FC_BARRIER, "%lx -> %d:%lx", (unsigned long)src,
	       thread, (unsigned long) dst);

  if (sizeof (int) <= GUPCR_MAX_OPTIM_SIZE)
    {
      gupcr_fabric_call (fi_inject_atomic,
			 (gupcr_bup_tx_ep, (const void *) src, 1,
			  fi_rx_addr ((fi_addr_t) thread,
			  GUPCR_SERVICE_BARRIER_UP, GUPCR_SERVICE_BITS),
			  (uint64_t) GUPCR_LOCAL_INDEX (dst), 0,
			  FI_UINT32, FI_MIN));
    }
  else
    {
      gupcr_fabric_call (fi_atomic,
			 (gupcr_bup_tx_ep, (const void *) src, 1,
			  gupcr_bup_tx_mr,
			  fi_rx_addr ((fi_addr_t) thread,
			  GUPCR_SERVICE_BARRIER_UP, GUPCR_SERVICE_BITS),
			  (uint64_t) GUPCR_LOCAL_INDEX (dst), 0,
			  FI_UINT32, FI_MIN, NULL));
    }
}

/** Setup a trigger for atomically send data to the parent thread
 *
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 * @param [in] trig Trigger count
 * @param [in] ctx Trigger context index
 */
void
gupcr_barrier_tr_atomic (int *src, int thread, int *dst, size_t trig, int ctx)
{
  struct fi_msg_atomic msg_atomic = {0};
  struct fi_ioc msg = {0};
  struct fi_rma_ioc endpt = {0};
  gupcr_assert (ctx < GUPCR_BAR_MAX_TRIG_CTX);
  gupcr_debug (FC_BARRIER, "%lx -> %d:%lx (%ld)", (unsigned long)src,
	       thread, (unsigned long) dst, sizeof (int));
  trig_ctx[ctx].event_type = FI_TRIGGER_THRESHOLD;
  gupcr_bup_rx_count += trig;
  trig_ctx[ctx].trigger.threshold.cntr = gupcr_bup_rx_ct;
  trig_ctx[ctx].trigger.threshold.threshold = gupcr_bup_rx_count;
  msg.addr = src;
  msg.count = 1;
  endpt.addr = (uint64_t) dst;
  msg_atomic.msg_iov = &msg;
  msg_atomic.addr = fi_rx_addr ((fi_addr_t) thread, GUPCR_SERVICE_BARRIER_UP,
				GUPCR_SERVICE_BITS);
  msg_atomic.rma_iov = &endpt;
  msg_atomic.context = &trig_ctx[ctx];
  msg_atomic.datatype = FI_UINT32;
  msg_atomic.op = FI_MIN;
  gupcr_fabric_call (fi_atomicmsg, (gupcr_bup_tx_ep, &msg_atomic,
				    FI_TRIGGER));
}

/** Wait for notifying barrier value (up phase).
 *
 * @param [in] count Wait count
 */
void
gupcr_barrier_wait_up (size_t count)
{
  int status;
  gupcr_debug (FC_BARRIER, "");
  gupcr_bup_rx_count += count;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_bup_rx_ct, gupcr_bup_rx_count,
			 GUPCR_TRANSFER_TIMEOUT));
  GUPCR_TIMEOUT_CHECK (status, "barrier wait up", gupcr_bup_rx_cq);
}

/** Wait for calculated barrier value (down phase).
 */
void
gupcr_barrier_wait_down (void)
{
  int status;
  gupcr_debug (FC_BARRIER, "");
  gupcr_bdown_rx_count++;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_bdown_rx_ct, gupcr_bdown_rx_count,
			 GUPCR_TRANSFER_TIMEOUT));
  GUPCR_TIMEOUT_CHECK (status, "barrier wait down", gupcr_bdown_rx_cq);
}

/** Wait for delivery completion (down phase).
 *
 * @param [in] count Wait count
 */
void
gupcr_barrier_wait_delivery (size_t count)
{
  int status;
  gupcr_debug (FC_BARRIER, "");
  gupcr_bdown_tx_put_count += count;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_bdown_tx_put_ct, gupcr_bdown_tx_put_count,
			 GUPCR_TRANSFER_TIMEOUT));
  if (status)
    {
      if (status == FI_ETIMEDOUT)
	gupcr_fatal_error ("Timeout on barrier up wait");
      else
	{
	  gupcr_process_fail_events (gupcr_bdown_tx_put_cq);
	  gupcr_abort ();
	}
    }
}

/**
 * @fn gupcr_barrier_sup_init (void)
 * Initialize barrier resources.
 * @ingroup INIT
 *
 */
void
gupcr_barrier_sup_init (void)
{
  cntr_attr_t cntr_attr = { 0 };
  cq_attr_t cq_attr = { 0 };
  rx_attr_t rx_attr = { 0 };
  tx_attr_t tx_attr = { 0 };
  tx_attr.op_flags = FI_TRANSMIT_COMPLETE;

#define CREATE_ENDPOINTS(bar,ep_service) \
	gupcr_##bar##_tx_put_count = 0; \
	gupcr_##bar##_tx_get_count = 0; \
	gupcr_fabric_call (fi_tx_context, \
			   (gupcr_ep, ep_service, &tx_attr, \
			    &gupcr_##bar##_tx_ep, NULL)); \
	/* Create local memory region.  */ \
	gupcr_fabric_call (fi_mr_reg, (gupcr_fd, USER_PROG_MEM_START, \
				       USER_PROG_MEM_SIZE, \
				       FI_READ | FI_WRITE, 0, 0, FI_MR_OFFSET, \
				       &gupcr_##bar##_tx_mr,\
				       NULL)); \
	/* NOTE: No need to bind, implicitly done.  */ \
	/* Create local endpoint counter/queue.  */ \
	cntr_attr.events = FI_CNTR_EVENTS_COMP; \
	cntr_attr.flags = 0; \
	cq_attr.size = 1; \
	cq_attr.format = FI_CQ_FORMAT_MSG; \
	cq_attr.wait_obj = FI_WAIT_NONE; \
	gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr, \
					  &gupcr_##bar##_tx_put_ct, NULL)); \
	gupcr_fabric_call (fi_cq_open, \
		(gupcr_fd, &cq_attr, &gupcr_##bar##_tx_put_cq, NULL)); \
	gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr, \
					  &gupcr_##bar##_tx_get_ct, NULL)); \
	gupcr_fabric_call (fi_cq_open, \
		(gupcr_fd, &cq_attr, &gupcr_##bar##_tx_get_cq, NULL)); \
	/* And bind them to and point.   */ \
	gupcr_fabric_call (fi_ep_bind, (gupcr_##bar##_tx_ep, \
				       &gupcr_##bar##_tx_put_ct->fid, \
				       FI_WRITE)); \
	gupcr_fabric_call (fi_ep_bind, (gupcr_##bar##_tx_ep, \
				       &gupcr_##bar##_tx_put_cq->fid, \
				       FI_WRITE)); \
	gupcr_fabric_call (fi_ep_bind, (gupcr_##bar##_tx_ep, \
				       &gupcr_##bar##_tx_get_ct->fid, \
				       FI_READ)); \
	gupcr_fabric_call (fi_ep_bind, (gupcr_##bar##_tx_ep, \
				       &gupcr_##bar##_tx_get_cq->fid, \
				       FI_READ)); \
	/* Enable endpoint.  */ \
	gupcr_fabric_call (fi_enable, (gupcr_##bar##_tx_ep)); \
	gupcr_fabric_call (fi_ep_bind, (gupcr_##bar##_tx_ep, \
				       &gupcr_##bar##_tx_mr->fid, \
				       FI_WRITE | FI_READ)); \
	/* Create target side of the endpoint.  */ \
	gupcr_##bar##_rx_count = 0; \
	gupcr_fabric_call (fi_rx_context, \
			   (gupcr_ep, ep_service, &rx_attr, \
			    &gupcr_##bar##_rx_ep, NULL)); \
	/* Create target memory region.  Map the whole memory too.  */ \
	gupcr_fabric_call (fi_mr_reg, (gupcr_fd, USER_PROG_MEM_START, \
				       USER_PROG_MEM_SIZE, \
				       FI_REMOTE_READ | FI_REMOTE_WRITE, \
				       0, 0, FI_MR_OFFSET, &gupcr_##bar##_rx_mr,\
				       NULL)); \
	/* Create remote endpoint counter/queue.  */ \
	cntr_attr.events = FI_CNTR_EVENTS_COMP; \
	cntr_attr.flags = 0; \
	cq_attr.size = 1; \
	cq_attr.format = FI_CQ_FORMAT_MSG; \
	cq_attr.wait_obj = FI_WAIT_NONE; \
	gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr, \
					  &gupcr_##bar##_rx_ct, NULL)); \
	gupcr_fabric_call (fi_mr_bind, (gupcr_##bar##_rx_mr, \
				       &gupcr_##bar##_rx_ct->fid, \
				       FI_REMOTE_WRITE)); \
	gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr, \
					&gupcr_##bar##_rx_cq, NULL)); \
	gupcr_fabric_call (fi_mr_bind, (gupcr_##bar##_rx_mr, \
				       &gupcr_##bar##_rx_cq->fid, \
				       FI_REMOTE_WRITE)); \
	/* Enable endpoint bind remote memory to it.  */ \
	gupcr_fabric_call (fi_enable, (gupcr_##bar##_rx_ep)); \
	gupcr_fabric_call (fi_ep_bind, (gupcr_##bar##_rx_ep, \
				       &gupcr_##bar##_rx_mr->fid, \
				       FI_REMOTE_WRITE | FI_REMOTE_READ)); \

  CREATE_ENDPOINTS (bup, GUPCR_SERVICE_BARRIER_UP);
  CREATE_ENDPOINTS (bdown, GUPCR_SERVICE_BARRIER_DOWN);
}

/**
 * @fn gupcr_barrier_sup_fini (void)
 * Release barrier resources.
 * @ingroup INIT
 */
void
gupcr_barrier_sup_fini (void)
{
  int status;
#define DELETE_ENDPOINTS(bar) \
  gupcr_fabric_call (fi_close, (&gupcr_##bar##_tx_put_ct->fid)); \
  gupcr_fabric_call (fi_close, (&gupcr_##bar##_tx_put_cq->fid)); \
  gupcr_fabric_call (fi_close, (&gupcr_##bar##_tx_get_ct->fid)); \
  gupcr_fabric_call (fi_close, (&gupcr_##bar##_tx_get_cq->fid)); \
  gupcr_fabric_call (fi_close, (&gupcr_##bar##_tx_mr->fid)); \
  gupcr_fabric_call (fi_close, (&gupcr_##bar##_rx_ct->fid)); \
  gupcr_fabric_call (fi_close, (&gupcr_##bar##_rx_cq->fid)); \
  gupcr_fabric_call (fi_close, (&gupcr_##bar##_rx_mr->fid)); \
  /* NOTE: Do not check for errors.  Breaks occasionally.  */ \
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_##bar##_rx_ep->fid)); \
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_##bar##_tx_ep->fid));

  DELETE_ENDPOINTS (bup);
  DELETE_ENDPOINTS (bdown);
}

/** @} */
