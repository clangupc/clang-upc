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

#if BARRIER_TEST
#define DEFINE_ENDPOINT(ep_name) \
	static fab_ep_t gupcr_##ep_name##_ep; \
	static fab_mr_t gupcr_##ep_name##_mr; \
	static fab_cntr_t gupcr_##ep_name##_ct; \
	static fab_cq_t gupcr_##ep_name##_cq;

DEFINE_ENDPOINT (bup_rx)
DEFINE_ENDPOINT (bup_tx)
DEFINE_ENDPOINT (bdown_rx)
DEFINE_ENDPOINT (bdown_tx)
#endif

/** Send data to a remote thread
 *
 * @param [in] dir Direction (up/down)
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 */
void
gupcr_barrier_put (enum barrier_dir dir, int *src, int thread, int *dst)
{
#if BARRIER_TEST
  fab_ep_t ep = dir == BARRIER_UP ? gupcr_bup_tx_ep : gupcr_bdown_tx_ep;

  if (sizeof (int) <= GUPCR_MAX_OPTIM_SIZE)
    {
      gupcr_fabric_call (fi_inject_writeto,
			 (ep, (const void *) src, sizeof (int),
			  fi_rx_addr ((fi_addr_t) thread,
				      GUPCR_SERVICE_LOCK, GUPCR_SERVICE_BITS),
			  (uint64_t) GUPCR_LOCAL_INDEX (dst), 0));
    }
  else
    {
      gupcr_fabric_call (fi_writeto,
			 (ep, (const void *) src, sizeof (int), NULL,
			  fi_rx_addr ((fi_addr_t) thread,
				      GUPCR_SERVICE_LOCK, GUPCR_SERVICE_BITS),
			  (uint64_t) GUPCR_LOCAL_INDEX (dst), 0, NULL));
    }
#endif
}

/** Setup a trigger for sending data to remote thread
 *
 * @param [in] dir Barrier direction
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 * @param [in] cnt Trigger counter
 */
void
gupcr_barrier_tr_put (enum barrier_dir dir, int *src,
		      int thread, int *dst, size_t cnt)
{
}

/** Atomically send data to a remote thread
 *
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 */
void
gupcr_barrier_atomic (int *src, int thread, int *dst)
{
}

/** Setup a trigger for atomically send data to a remote thread
 *
 * @param [in] dir Barrier direction
 * @param [in] src Address of the source
 * @param [in] thread Remote thread
 * @param [in] dst Address of the destination
 * @param [in] cnt Trigger count
 */
void
gupcr_barrier_tr_atomic (enum barrier_dir dir, int *src, int thread,
			 int *dst, size_t cnt)
{
}

/** Wait for notifying barrier value (up phase).
 *
 * @param [in] cnt Wait count
 */
void
gupcr_barrier_wait_up (size_t cnt)
{
}

/** Wait for calculated barrier value (down phase).
 *
 * @param [in] cnt Wait count
 */
void
gupcr_barrier_wait_down (size_t cnt)
{
}

/** Wait for delivery completion (down phase).
 *
 * @param [in] cnt Wait count
 */
void
gupcr_barrier_wait_delivery (size_t cnt)
{
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
#if BARRIER_TEST
  cntr_attr_t cntr_attr = { 0 };
  cq_attr_t cq_attr = { 0 };

#define CREATE_ENDPOINT(ep_name,ep_service,ep_flags) \
	gupcr_fabric_call (fi_tx_context, \
	(gupcr_ep, ep_service, NULL, &gupcr_##ep_name##_ep, NULL)); \
	/* Create target endpoint counter/queue.  */ \
	cntr_attr.events = FI_CNTR_EVENTS_COMP; \
	cntr_attr.flags = 0; \
	gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr, \
					  &gupcr_##ep_name##_ct, NULL)); \
	cq_attr.size = 1; \
	cq_attr.format = FI_CQ_FORMAT_MSG; \
	cq_attr.wait_obj = FI_WAIT_NONE; \
	gupcr_fabric_call (fi_cq_open, \
			   (gupcr_fd, &cq_attr, &gupcr_##ep_name##_cq, NULL)); \
	/* Create target memory region.  */ \
	gupcr_fabric_call (fi_mr_reg, (gupcr_fd, USER_PROG_MEM_START, \
				       USER_PROG_MEM_SIZE, \
				       ep_flags, 0, \
				       0, 0, &gupcr_##ep_name##_mr, NULL)); \
	/* Bind counters/queues to memory region.  */ \
	gupcr_fabric_call (fi_bind, (&gupcr_##ep_name##_mr->fid, \
				     &gupcr_##ep_name##_ct->fid, \
				     ep_flags)); \
	gupcr_fabric_call (fi_bind, (&gupcr_##ep_name##_mr->fid, \
				     &gupcr_##ep_name##_cq->fid, ep_flags)); \
	/* Bind target memory region to endpoint.  */ \
	gupcr_fabric_call (fi_bind, (&gupcr_##ep_name##_ep->fid, \
				     &gupcr_##ep_name##_mr->fid, ep_flags));

CREATE_ENDPOINT (bup_rx, GUPCR_SERVICE_BARRIER_UP,
		   FI_REMOTE_READ | FI_REMOTE_WRITE)
    CREATE_ENDPOINT (bup_tx, GUPCR_SERVICE_BARRIER_UP,
		       FI_READ | FI_WRITE) CREATE_ENDPOINT (bdown_rx,
				GUPCR_SERVICE_BARRIER_DOWN,
				FI_REMOTE_READ | FI_REMOTE_WRITE)
    CREATE_ENDPOINT (bdown_tx, GUPCR_SERVICE_BARRIER_DOWN,
		       FI_READ | FI_WRITE)
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
#if BARRIER_TEST
#define DELETE_ENDPOINT(ep_name)\
	gupcr_fabric_call (fi_close, (&gupcr_##ep_name##_ct->fid)); \
	gupcr_fabric_call (fi_close, (&gupcr_##ep_name##_cq->fid)); \
	gupcr_fabric_call (fi_close, (&gupcr_##ep_name##_mr->fid)); \
	gupcr_fabric_call (fi_close, (&gupcr_##ep_name##_ep->fid));
  DELETE_ENDPOINT (bup_rx)
  DELETE_ENDPOINT (bup_tx)
  DELETE_ENDPOINT (bdown_rx)
  DELETE_ENDPOINT (bdown_tx)
#endif
}

/** @} */
