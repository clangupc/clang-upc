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
#define GUPCR_LOCAL_OFFSET(addr) \
	(void *) ((char *) addr - (char *) USER_PROG_MEM_START)
/** Index of the target memory  */
#define GUPCR_REMOTE_OFFSET(addr) \
	(uint64_t) ((char *) addr - (char *) gupcr_bbase)

/* For barrier purpose we create two endpoints (UP/DOWN) and
   bind target/local memory regions to them.  Only barrier structure
   is mapped with memory region and accessible to other threads.  */

/** Barrier memory region base address */
static char *gupcr_bbase;
/** Barrier memory region size */
static int gupcr_bsize;

/** Barrier endpoints */
static gupcr_epinfo_t gupcr_barrier_ep;

static fab_cntr_t gupcr_barrier_tx_put_ct;
static fab_cq_t gupcr_barrier_tx_put_cq;
static size_t gupcr_barrier_tx_put_count;
static fab_mr_t gupcr_barrier_notify_mr;
static fab_cntr_t gupcr_barrier_notify_ct;
static size_t gupcr_barrier_notify_count;
static fab_mr_t gupcr_barrier_wait_mr;
static fab_cntr_t gupcr_barrier_wait_ct;
static size_t gupcr_barrier_wait_count;
static fab_cq_t gupcr_barrier_event_cq;

/** Target address */
#define GUPCR_TARGET_ADDR(target) \
	fi_rx_addr ((fi_addr_t)target, \
	GUPCR_FABRIC_SCALABLE_CTX() ? GUPCR_SERVICE_BARRIER : 0, \
	GUPCR_FABRIC_SCALABLE_CTX() ? GUPCR_SERVICE_BITS : 1)
/** Max number of outstanding triggered operations  */
#define GUPCR_BAR_MAX_TRIG_CTX 6
/** Triggered context structures  */
static fi_trig_t trig_ctx[GUPCR_BAR_MAX_TRIG_CTX];
static int ctx_index = 0;

/* The basic barrier code assumes that RMA_EVENT is supported
   but the fabric.  In cases that it is not, a special signaling
   is being performed to inform the other side of the barrier
   progress.  */
/** Data structure used for remote signaling */
static struct bar_signal
{
  int notify;			/* Barrier notify signal.  */
  int wait;			/* Barrier wait signal.  */
} gupcr_bar_signal;
/** Barrier signaling constant */
static int gupcr_bar_one;
/** Barrier signaling memory region */
static fab_mr_t gupcr_bar_signal_mr;
/** Barrier signal memory region index */
#define GUPCR_SIGNAL_INDEX(addr) \
  (uint64_t)((char *)addr - (char *)&gupcr_bar_signal)
/** Target memory regions */
static gupcr_memreg_t *gupcr_notify_mr_keys;
static gupcr_memreg_t *gupcr_wait_mr_keys;
static gupcr_memreg_t *gupcr_bar_signal_mr_keys;

/** Send data to remote thread in down phase.
 *
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 * @param [in] count Number of bytes to send
 */
void
gupcr_barrier_wait_put (void *src, int thread, void *dst, size_t count)
{
  ssize_t ret;
  gupcr_debug (FC_BARRIER, "%lx -> %d:%lx (%ld)", (unsigned long) src,
	       thread, GUPCR_REMOTE_OFFSET (dst), (unsigned long) count);

  if (sizeof (int) <= GUPCR_MAX_OPTIM_SIZE)
    {
      gupcr_fabric_call_size (fi_inject_write, ret,
			      (gupcr_barrier_ep.tx_ep, (const void *) src,
			       count, GUPCR_TARGET_ADDR (thread),
			       GUPCR_REMOTE_MR_ADDR (wait, thread,
						     GUPCR_REMOTE_OFFSET
						     (dst)),
			       GUPCR_REMOTE_MR_KEY (wait, thread)));
    }
  else
    {
      gupcr_fabric_call_size (fi_write, ret,
			      (gupcr_barrier_ep.tx_ep, (const void *) src,
			       count, NULL, GUPCR_TARGET_ADDR (thread),
			       GUPCR_REMOTE_MR_ADDR (wait, thread,
						     GUPCR_REMOTE_OFFSET
						     (dst)),
			       GUPCR_REMOTE_MR_KEY (wait, thread), NULL));
    }
  gupcr_barrier_tx_put_count++;
  if (!GUPCR_FABRIC_RMA_EVENT ())
    {
      /* Must wait for completion before signaling.  */
      gupcr_fabric_call_cntr_wait ((gupcr_barrier_tx_put_ct,
				    gupcr_barrier_tx_put_count,
				    GUPCR_TRANSFER_TIMEOUT),
				   "barrier wait down",
				   gupcr_barrier_tx_put_cq);

      /* Signal the target side.  */
      gupcr_fabric_call_size (fi_write, ret,
			      (gupcr_barrier_ep.tx_ep, &gupcr_bar_one,
			       1, NULL,
			       GUPCR_TARGET_ADDR (thread),
			       GUPCR_REMOTE_MR_ADDR (bar_signal, thread,
						     GUPCR_SIGNAL_INDEX
						     (&gupcr_bar_signal.wait)),
			       GUPCR_REMOTE_MR_KEY (bar_signal, thread),
			       NULL));
      gupcr_barrier_tx_put_count++;
    }
}

/** Wait for value from parent in wait phase.
 */
void
gupcr_barrier_wait_event (void)
{
  gupcr_debug (FC_BARRIER, "");
  if (GUPCR_FABRIC_RMA_EVENT ())
    {
      gupcr_barrier_wait_count++;
      gupcr_fabric_call_cntr_wait ((gupcr_barrier_wait_ct,
				    gupcr_barrier_wait_count,
				    GUPCR_TRANSFER_TIMEOUT),
				   "barrier wait down",
				   gupcr_barrier_event_cq);
    }
  else
    {
      /* Verify that parent signaled the wait.
         Atomic CSWAP might be better.  */
      while (gupcr_bar_signal.wait != 1)
	gupcr_yield_cpu ();
      gupcr_bar_signal.wait = 0;
    }
}

/** Wait for value delivery completion in wait phase.
 */
void
gupcr_barrier_wait_put_completion (void)
{
  gupcr_debug (FC_BARRIER, "wait: %ld", gupcr_barrier_tx_put_count);
  gupcr_fabric_call_cntr_wait (
			(gupcr_barrier_tx_put_ct, gupcr_barrier_tx_put_count,
			 GUPCR_TRANSFER_TIMEOUT), "barrier down",
				 gupcr_barrier_tx_put_cq);
}

/** Notify remote thread of barrier ID
 *
 * @param [in] src Address of my barrier ID
 * @param [in] thread Remote thread
 * @param [in] dst Address of the parents calculated ID
 */
void
gupcr_barrier_notify_put (void *src, int thread, void *dst)
{
  ssize_t ret;
  gupcr_debug (FC_BARRIER, "%lx -> %d:%lx", (unsigned long) src, thread,
	       GUPCR_REMOTE_OFFSET (dst));

  if (sizeof (int) <= GUPCR_MAX_OPTIM_SIZE)
    {
      gupcr_fabric_call_size (fi_inject_atomic, ret,
			      (gupcr_barrier_ep.tx_ep, (const void *) src, 1,
			       GUPCR_TARGET_ADDR (thread),
			       GUPCR_REMOTE_MR_ADDR (notify, thread,
						     GUPCR_REMOTE_OFFSET
						     (dst)),
			       GUPCR_REMOTE_MR_KEY (notify, thread),
			       FI_INT32, FI_MIN));
    }
  else
    {
      gupcr_fabric_call_size (fi_atomic, ret,
			      (gupcr_barrier_ep.tx_ep, (const void *) src, 1,
			       NULL, GUPCR_TARGET_ADDR (thread),
			       GUPCR_REMOTE_MR_ADDR (notify, thread,
						     GUPCR_REMOTE_OFFSET
						     (dst)),
			       GUPCR_REMOTE_MR_KEY (notify, thread),
			       FI_INT32, FI_MIN, NULL));
    }
  gupcr_barrier_tx_put_count++;
  if (!GUPCR_FABRIC_RMA_EVENT ())
    {
      /* Must wait for completion before signaling.  */
      gupcr_fabric_call_cntr_wait ((gupcr_barrier_tx_put_ct,
				    gupcr_barrier_tx_put_count,
				    GUPCR_TRANSFER_TIMEOUT),
				   "barrier wait up",
				   gupcr_barrier_tx_put_cq);

      /* We also need to signal to target side.  */
      gupcr_fabric_call_size (fi_atomic, ret,
			      (gupcr_barrier_ep.tx_ep, &gupcr_bar_one,
			       1, NULL, GUPCR_TARGET_ADDR (thread),
			       GUPCR_REMOTE_MR_ADDR (bar_signal, thread,
						     GUPCR_SIGNAL_INDEX
						     (&gupcr_bar_signal.
						      notify)),
			       GUPCR_REMOTE_MR_KEY (bar_signal, thread),
			       FI_UINT32, FI_SUM, NULL));
      gupcr_barrier_tx_put_count++;
    }
}

/** Wait for notifying barrier value.
 *
 * @param [in] count Wait count
 */
void
gupcr_barrier_notify_event (size_t count)
{
  gupcr_debug (FC_BARRIER, "%ld", count);
  if (GUPCR_FABRIC_RMA_EVENT ())
    {
      gupcr_barrier_notify_count += count;
      gupcr_fabric_call_cntr_wait ((gupcr_barrier_notify_ct,
				    gupcr_barrier_notify_count,
				    GUPCR_TRANSFER_TIMEOUT),
				   "barrier wait up", gupcr_barrier_event_cq);
    }
  else
    {
      /* Verify that children + parent signaled.
         Atomic CSWAp might be better.  */
      while (gupcr_bar_signal.notify != (int) count)
	gupcr_yield_cpu ();
      gupcr_bar_signal.notify = 0;
    }
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
gupcr_barrier_tr_put (void *src, int thread, void *dst, size_t count,
		      size_t trig, int wait_phase)
{
  ssize_t ret;
  struct fi_msg_rma msg_rma = { 0 };
  struct iovec msg = { 0 };
  struct fi_rma_iov endpt = { 0 };
  if (ctx_index == GUPCR_BAR_MAX_TRIG_CTX)
    ctx_index = 0;;
  gupcr_debug (FC_BARRIER, "%lx -> %d:%lx (%ld)", (unsigned long) src,
	       thread, (unsigned long) dst, (unsigned long) count);
  trig_ctx[ctx_index].event_type = FI_TRIGGER_THRESHOLD;
  if (wait_phase)
    {
      gupcr_barrier_wait_count += trig;
      trig_ctx[ctx_index].trigger.threshold.cntr = gupcr_barrier_wait_ct;
      trig_ctx[ctx_index].trigger.threshold.threshold =
	gupcr_barrier_wait_count;
      endpt.key = GUPCR_REMOTE_MR_KEY (wait, thread);
    }
  else
    {
      gupcr_barrier_notify_count += trig;
      trig_ctx[ctx_index].trigger.threshold.cntr = gupcr_barrier_notify_ct;
      trig_ctx[ctx_index].trigger.threshold.threshold =
	gupcr_barrier_notify_count;
      endpt.key = GUPCR_REMOTE_MR_KEY (notify, thread);
    }
  msg.iov_base = src;
  msg.iov_len = count;
  endpt.addr =
    wait_phase ?
    GUPCR_REMOTE_MR_ADDR (wait, thread, GUPCR_REMOTE_OFFSET (dst)) :
    GUPCR_REMOTE_MR_ADDR (notify, thread, GUPCR_REMOTE_OFFSET (dst));
  endpt.len = count;
  msg_rma.msg_iov = &msg;
  msg_rma.addr = GUPCR_TARGET_ADDR (thread);
  msg_rma.rma_iov = &endpt;
  msg_rma.context = &trig_ctx[ctx_index];
  gupcr_fabric_call_size (fi_writemsg, ret,
			  (gupcr_barrier_ep.tx_ep, &msg_rma, FI_TRIGGER));
  ctx_index++;
}

/** Setup a trigger for atomic data transfer to the parent thread
 *
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 * @param [in] trig Trigger count
 */
void
gupcr_barrier_notify_tr_put (void *src, int thread, void *dst, size_t trig)
{
  ssize_t ret;
  struct fi_msg_atomic msg_atomic = { 0 };
  struct fi_ioc msg = { 0 };
  struct fi_rma_ioc endpt = { 0 };
  if (ctx_index == GUPCR_BAR_MAX_TRIG_CTX)
    ctx_index = 0;
  gupcr_debug (FC_BARRIER, "%lx -> %d:%lx (%ld)", (unsigned long) src,
	       thread, (unsigned long) dst, sizeof (int));
  trig_ctx[ctx_index].event_type = FI_TRIGGER_THRESHOLD;
  gupcr_barrier_notify_count += trig;
  trig_ctx[ctx_index].trigger.threshold.cntr = gupcr_barrier_notify_ct;
  trig_ctx[ctx_index].trigger.threshold.threshold =
    gupcr_barrier_notify_count;
  msg.addr = src;
  msg.count = 1;
  endpt.addr =
    GUPCR_REMOTE_MR_ADDR (notify, thread, GUPCR_REMOTE_OFFSET (dst));
  endpt.count = 1;
  endpt.key = GUPCR_REMOTE_MR_KEY (notify, thread);
  msg_atomic.msg_iov = &msg;
  msg_atomic.addr = GUPCR_TARGET_ADDR (thread);
  msg_atomic.rma_iov = &endpt;
  msg_atomic.context = &trig_ctx[ctx_index];
  msg_atomic.datatype = FI_INT32;
  msg_atomic.op = FI_MIN;
  gupcr_fabric_call_size (fi_atomicmsg, ret,
			  (gupcr_barrier_ep.tx_ep, &msg_atomic, FI_TRIGGER));
  ctx_index++;
}

/**
 * @fn gupcr_barrier_sup_init (void)
 * Initialize barrier resources.
 * @ingroup INIT
 *
 * @param [in] bbase Base address of the barrier region
 * @param [in] bsize Size of the barrier region
 */
void
gupcr_barrier_sup_init (void *bbase, int bsize)
{
  cntr_attr_t cntr_attr = { 0 };
  cq_attr_t cq_attr = { 0 };

  gupcr_bbase = bbase;
  gupcr_bsize = bsize;

  /* Create barrier endpoints.  */
  gupcr_barrier_ep.name = "barrier";
  gupcr_barrier_ep.service = GUPCR_SERVICE_BARRIER;
  gupcr_fabric_ep_create (&gupcr_barrier_ep);

  gupcr_barrier_tx_put_count = 0;
  /* Create local endpoint counter/queue.  */
  cntr_attr.events = FI_CNTR_EVENTS_COMP;
  cntr_attr.flags = 0;
  cq_attr.size = GUPCR_CQ_ERROR_SIZE;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr,
				    &gupcr_barrier_tx_put_ct, NULL));
  gupcr_fabric_call (fi_ep_bind, (gupcr_barrier_ep.tx_ep,
				  &gupcr_barrier_tx_put_ct->fid, FI_WRITE));
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr,
				  &gupcr_barrier_tx_put_cq, NULL));
  gupcr_fabric_call (fi_ep_bind, (gupcr_barrier_ep.tx_ep,
				  &gupcr_barrier_tx_put_cq->fid,
				  FI_WRITE | FI_SELECTIVE_COMPLETION));
  /* Initialize target side of the endpoint.  */
  /* Two memory regions with separate counters is being used.  */
#define CREATE_BARRIER_TARGET_MR(phase,mr_phase) \
  gupcr_barrier_##phase##_count = 0; \
  /* Create target memory region.  */ \
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, gupcr_bbase, gupcr_bsize, \
				 FI_REMOTE_READ | FI_REMOTE_WRITE, \
				 0, mr_phase, 0, \
				 &gupcr_barrier_##phase##_mr, NULL)); \
  /* Create remote endpoint counter/queue.  */ \
  cntr_attr.events = FI_CNTR_EVENTS_COMP; \
  cntr_attr.flags = 0; \
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr, \
					  &gupcr_barrier_##phase##_ct, NULL)); \
  if (GUPCR_FABRIC_RMA_EVENT ()) \
    gupcr_fabric_call (fi_mr_bind, (gupcr_barrier_##phase##_mr, \
				    &gupcr_barrier_##phase##_ct->fid, \
				    FI_REMOTE_WRITE));
  CREATE_BARRIER_TARGET_MR (notify, GUPCR_MR_BARRIER_NOTIFY);
  GUPCR_GATHER_MR_KEYS (notify, gupcr_barrier_notify_mr, gupcr_bbase);
  CREATE_BARRIER_TARGET_MR (wait, GUPCR_MR_BARRIER_WAIT);
  GUPCR_GATHER_MR_KEYS (wait, gupcr_barrier_wait_mr, gupcr_bbase);

  /* There is only one completion queue for event errors.  */
  cq_attr.size = GUPCR_CQ_ERROR_SIZE;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr,
				  &gupcr_barrier_event_cq, NULL));
  if (GUPCR_FABRIC_RMA_EVENT ())
    {
      gupcr_fabric_call (fi_mr_bind, (gupcr_barrier_notify_mr,
				      &gupcr_barrier_event_cq->fid,
				      FI_REMOTE_WRITE));
      gupcr_fabric_call (fi_mr_bind, (gupcr_barrier_wait_mr,
				      &gupcr_barrier_event_cq->fid,
				      FI_REMOTE_WRITE));
    }
  else
    {
      gupcr_bar_signal.notify = 0;
      gupcr_bar_signal.wait = 0;
      gupcr_bar_one = 1;
      /* Create target signaling memory region.  */
      gupcr_fabric_call (fi_mr_reg,
			 (gupcr_fd, &gupcr_bar_signal,
			  sizeof (gupcr_bar_signal),
			  FI_REMOTE_READ | FI_REMOTE_WRITE, 0,
			  GUPCR_MR_BARRIER_SIGNAL, 0,
			  &gupcr_bar_signal_mr, NULL));
      GUPCR_GATHER_MR_KEYS (bar_signal, gupcr_bar_signal_mr,
			    &gupcr_bar_signal);
    }

#if TARGET_MR_BIND_NEEDED
  /* ... bind remote memory to it.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_barrier_ep.rx_ep,
				  &gupcr_barrier_notify_mr->fid,
				  FI_REMOTE_WRITE | FI_REMOTE_READ));
  gupcr_fabric_call (fi_ep_bind, (gupcr_barrier_ep.rx_ep,
				  &gupcr_barrier_wait_mr->fid,
				  FI_REMOTE_WRITE | FI_REMOTE_READ));
#endif
}

/**
 * @fn gupcr_barrier_sup_fini (void)
 * Release barrier resources.
 * @ingroup INIT
 */
void
gupcr_barrier_sup_fini (void)
{
  gupcr_fabric_call (fi_close, (&gupcr_barrier_tx_put_ct->fid));
  gupcr_fabric_call (fi_close, (&gupcr_barrier_tx_put_cq->fid));
  gupcr_fabric_call (fi_close, (&gupcr_barrier_event_cq->fid));
  gupcr_fabric_call (fi_close, (&gupcr_barrier_notify_ct->fid));
  gupcr_fabric_call (fi_close, (&gupcr_barrier_notify_mr->fid));
  gupcr_fabric_call (fi_close, (&gupcr_barrier_wait_ct->fid));
  gupcr_fabric_call (fi_close, (&gupcr_barrier_wait_mr->fid));
  gupcr_fabric_ep_delete (&gupcr_barrier_ep);
  free (gupcr_notify_mr_keys);
  free (gupcr_wait_mr_keys);
  free (gupcr_bar_signal_mr_keys);
}

/** @} */
