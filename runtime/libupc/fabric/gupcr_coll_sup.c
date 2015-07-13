/*===-- gupcr_coll_sup.c - UPC Runtime Support Library -------------------===
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
#include "gupcr_sup.h"
#include "gupcr_fabric.h"
#include "gupcr_iface.h"
#include "gupcr_utils.h"
#include "gupcr_coll_sup.h"

/**
 * @file gupcr_coll_sup.c
 * GUPC Libfabric collectives implementation support routines.
 *
 * @addtogroup COLLECTIVES GUPCR Collectives Functions
 * @{
 */

/** Index of the local memory location */
#define GUPCR_LOCAL_INDEX(addr) \
	(void *) ((char *) addr - (char *) USER_PROG_MEM_START)

/** Fabric communications endpoint resources */
/** RX Endpoint comm resource  */
fab_ep_t gupcr_coll_rx_ep;
/** TX Endpoint comm resource  */
fab_ep_t gupcr_coll_tx_ep;
/** Completion remote counter (target side) */
fab_cntr_t gupcr_coll_ct;
/** Completion counter */
fab_cntr_t gupcr_coll_lct;
/** Completion remote queue (target side) */
fab_cq_t gupcr_coll_cq;
/** Completion queue for errors  */
fab_cq_t gupcr_coll_lcq;
/** Remote access memory region  */
fab_mr_t gupcr_coll_mr;
#if MR_LOCAL_NEEDED
/** Local memory access memory region  */
fab_mr_t gupcr_coll_lmr;
#endif

/** Collectives number of received puts on NC */
static size_t gupcr_coll_signal_cnt;

/** Collectives number of received ACKs on local MR */
static size_t gupcr_coll_ack_cnt;

/* Collectives thread tree.  */
/** Collectives tree parent thread */
int gupcr_coll_parent_thread;
/** Collectives tree number of children */
int gupcr_coll_child_cnt;
/** Collectives tree child's index */
int gupcr_coll_child_index;
/** Collectives tree children threads */
int gupcr_coll_child[GUPCR_TREE_FANOUT];

/**
 * Initialize collectives thread tree.
 *
 * A collectives tree starts from the "start" thread number and
 * includes only "nthreads" (e.g. threads involved in
 * the reduce process).  The simplest case is when all the
 * threads are involved in which case start=0 and
 * nthreads=THREADS (e.g. used for broadcast).
 *
 * The collectives thread tree can be organized in a
 * form where the "newroot" value identitifies
 * the root thread (only if the "newroot" thread
 * is participating in the operation).
 * @param [in] newroot A hint for the tree root thread.
 * @param [in] start Start thread for reduce
 * @param [in] nthreads Number of threads participating
 *
 */
void
gupcr_coll_tree_setup (size_t newroot, size_t start, int nthreads)
{
/* Convert from/to 0-(THREADS-1) to start-(nthreads-1) range.  */
#define NEWID(id,first) ((id - first + THREADS) % THREADS)
#define OLDID(nid,first) ((nid + first) % THREADS)

/* Remap into the new root (from root 0 to "root").  */
#define NEWIDROOT(id,top,cnt) ((cnt + id - top) % cnt)
#define OLDIDROOT(nid,top,cnt) ((nid + top) % cnt)
  int i;
  int ok_to_root = 0;
  int myid;
  int root = NEWID (newroot, start);

  gupcr_debug (FC_COLL, "newroot: %lu, start: %lu nthreads: %d",
	       (long unsigned) newroot, (long unsigned) start, nthreads);

  /* Check if root node is participating.  If yes, use that for the
     root, otherwise 0.  */
  if (root < nthreads)
    ok_to_root = 1;

  /* Get myid - first convert into the new range (0-nthreads),
     then, if needed and possible, into the range where newroot becomes 0.  */
  myid = NEWID (MYTHREAD, start);
  if (ok_to_root)
    myid = NEWIDROOT (myid, root, nthreads);

  /* Calculate the thread id's of the children and parent.  */
  gupcr_coll_child_cnt = 0;
  for (i = 0; i < GUPCR_TREE_FANOUT; i++)
    {
      int child = (GUPCR_TREE_FANOUT * myid + i + 1);
      if (child < nthreads)
	{
	  ++gupcr_coll_child_cnt;
	  if (ok_to_root)
	    child = OLDIDROOT (child, root, nthreads);
	  gupcr_coll_child[i] = OLDID (child, start);
	}
    }
  if (myid)
    {
      gupcr_coll_parent_thread = (myid - 1) / GUPCR_TREE_FANOUT;
      gupcr_coll_child_index =
	myid - gupcr_coll_parent_thread * GUPCR_TREE_FANOUT - 1;
      if (ok_to_root)
	gupcr_coll_parent_thread =
	  OLDIDROOT (gupcr_coll_parent_thread, root, nthreads);
      gupcr_coll_parent_thread = OLDID (gupcr_coll_parent_thread, start);
    }
  else
    gupcr_coll_parent_thread = ROOT_PARENT;
}

/**
 * Collective PUT operation
 *
 * @param [in] dthread Destination thread
 * @param [in] doffset Destination offset in the shared space
 * @param [in] soffset Source offset in the shared space
 * @param [in] nbytes Number of bytes to copy
 */

void
gupcr_coll_put (size_t dthread, size_t doffset, size_t soffset, size_t nbytes)
{
  ssize_t csize;
  gupcr_debug (FC_COLL, "%d:0x%lx %lu:0x%lx %lu",
	       MYTHREAD, (long unsigned) soffset,
	       (long unsigned) dthread, (long unsigned) doffset,
	       (long unsigned) nbytes);
  if (nbytes <= GUPCR_MAX_OPTIM_SIZE)
    {
      gupcr_fabric_size_call (fi_inject_write, csize,
			 (gupcr_coll_tx_ep,
			  GUPCR_LOCAL_INDEX (gupcr_gmem_base + soffset),
			  nbytes, fi_rx_addr ((fi_addr_t) dthread,
					      GUPCR_SERVICE_COLL,
					      GUPCR_SERVICE_BITS),
			  doffset, GUPCR_MR_COLL));
    }
  else
    {
      gupcr_fabric_size_call (fi_write, csize,
			 (gupcr_coll_tx_ep,
			  GUPCR_LOCAL_INDEX (gupcr_gmem_base + soffset),
			  nbytes, NULL, fi_rx_addr ((fi_addr_t) dthread,
						    GUPCR_SERVICE_COLL,
						    GUPCR_SERVICE_BITS),
			  doffset, GUPCR_MR_COLL, NULL));
    }
}

/**
 * Collective triggered PUT operation
 *
 * Schedule put operation once number of signals reaches
 * the specified value.
 *
 * @param [in] dthread Destination thread
 * @param [in] doffset Destination offset in the shared space
 * @param [in] soffset Source offset in the shared space
 * @param [in] nbytes Number of bytes to copy
 * @param [in] cnt Trigger count
 */
void
gupcr_coll_trigput (size_t dthread, size_t doffset, size_t soffset,
		    size_t nbytes, size_t cnt)
{
  gupcr_debug (FC_COLL, "%d:0x%lx -> %lu:0x%lx %lu trig %lu",
	       MYTHREAD, (long unsigned) soffset,
	       (long unsigned) dthread, (long unsigned) doffset,
	       (long unsigned) nbytes, (long unsigned) cnt);
  gupcr_fatal_error ("not implemented");
}

/**
 * Collective atomic PUT operation.
 *
 * @param [in] dthread Destination thread
 * @param [in] doffset Destination offset in the shared space
 * @param [in] soffset Source offset in the shared space
 * @param [in] nbytes Number of bytes to copy
 * @param [in] op atomic operation
 * @param [in] datatype atomic data type
 */

void
gupcr_coll_put_atomic (size_t dthread, size_t doffset, size_t soffset,
		       size_t nbytes, enum fi_op op, enum fi_datatype datatype)
{
  gupcr_debug (FC_COLL, "%d:0x%lx %lu:0x%lx %lu %s %s",
	       MYTHREAD, (long unsigned) soffset,
	       (long unsigned) dthread, (long unsigned) doffset,
	       (long unsigned) nbytes,
	       gupcr_strop (op), gupcr_strdatatype (datatype));

  gupcr_fabric_call (fi_atomic,
		     (gupcr_coll_tx_ep,
		      GUPCR_LOCAL_INDEX (gupcr_gmem_base + soffset),
		      1, NULL, fi_rx_addr ((fi_addr_t) dthread,
					   GUPCR_SERVICE_ATOMIC,
					   GUPCR_SERVICE_BITS),
		      doffset, GUPCR_MR_COLL, datatype, op, NULL));
}

/**
 * Collective triggered atomic PUT operation.
 *
 * Schedule atomic put operation once number of signals reaches
 * the specified value.
 *
 * @param [in] dthread Destination thread
 * @param [in] doffset Destination offset in the shared space
 * @param [in] soffset Source offset in the shared space
 * @param [in] nbytes Number of bytes to copy
 * @param [in] op atomic operation
 * @param [in] datatype atomic data type
 * @param [in] cnt Number of signals that triggers
 */
void
gupcr_coll_trigput_atomic (size_t dthread, size_t doffset, size_t soffset,
			   size_t nbytes, enum fi_op op,
			   enum fi_datatype datatype, size_t cnt)
{
  gupcr_debug (FC_COLL, "%d:0x%lx %lu:0x%lx %lu %s %s trig %lu",
	       MYTHREAD, (long unsigned) soffset,
	       (long unsigned) dthread, (long unsigned) doffset,
	       (long unsigned) nbytes,
	       gupcr_strop (op), gupcr_strdatatype (datatype),
	       (long unsigned) cnt);
  gupcr_fatal_error ("not supported");
}

/**
 * Collectives wait for operation completion
 * This function is used in cases where threads needs to wait
 * for the completion of remote operations.
 *
 * @param [in] cnt Wait count
 */
void
gupcr_coll_ack_wait (size_t cnt)
{
  int status;
  gupcr_debug (FC_COLL, "wait for %lu (%lu)",
               (long unsigned) cnt,
	       (long unsigned) (gupcr_coll_ack_cnt + cnt));
  gupcr_coll_ack_cnt += cnt;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_coll_lct, gupcr_coll_ack_cnt,
			 GUPCR_TRANSFER_TIMEOUT));
  GUPCR_CNT_ERROR_CHECK (status, "coll_put", gupcr_coll_lcq);
}

/**
 * Collectives wait for signaling events
 * This function is used to wait for other threads to complete
 * operations in the thread's shared space (e.g. children performing
 * atomic ops in the parent's shared space).
 *
 * @param [in] cnt Wait count
 */
void
gupcr_coll_signal_wait (size_t cnt)
{
  int status;
  gupcr_debug (FC_COLL, "wait for %lu (%lu)",
	       (long unsigned) cnt,
	       (long unsigned) (gupcr_coll_signal_cnt + cnt));
  gupcr_coll_signal_cnt += cnt;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_coll_ct, gupcr_coll_signal_cnt,
			 GUPCR_TRANSFER_TIMEOUT));
  GUPCR_CNT_ERROR_CHECK (status, "coll_put", gupcr_coll_cq);
}

/**
 * Initialize collectives resources.
 * @ingroup INIT
 *
 * A thread's shared space is mapped via a NC for other
 * threads to write to, and an MR as a source for remote
 * operations.  In this way, the address filed of the shared pointer
 * can be used as an offset into NC/MR.
 */
void
gupcr_coll_init (void)
{
  cntr_attr_t cntr_attr = { 0 };
  cq_attr_t cq_attr = { 0 };
  tx_attr_t tx_attr = { 0 };
  rx_attr_t rx_attr = { 0 };

  gupcr_log (FC_COLL, "coll init called");

  /* Reset the number of signals/acks.  */
  gupcr_coll_signal_cnt = 0;
  gupcr_coll_ack_cnt = 0;

  /* Create context endpoints for COLL transfers.  */
  tx_attr.op_flags = FI_DELIVERY_COMPLETE;
  gupcr_fabric_call (fi_tx_context,
		     (gupcr_ep, GUPCR_SERVICE_COLL, &tx_attr, &gupcr_coll_tx_ep,
		      NULL));
  gupcr_fabric_call (fi_rx_context,
		     (gupcr_ep, GUPCR_SERVICE_COLL, &rx_attr, &gupcr_coll_rx_ep,
		      NULL));

  /* ... and completion counter/eq for remote read/write.  */
  cntr_attr.events = FI_CNTR_EVENTS_COMP;
  cntr_attr.flags = 0;
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr,
				    &gupcr_coll_lct, NULL));
  gupcr_fabric_call (fi_ep_bind, (gupcr_coll_tx_ep, &gupcr_coll_lct->fid,
				  FI_READ | FI_WRITE));
  gupcr_coll_ack_cnt = 0;
  gupcr_coll_signal_cnt = 0;

  /* ... and completion queue for remote target transfer errors.  */
  cq_attr.size = 1;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr, &gupcr_coll_lcq, NULL));
  /* Use FI_SELECTIVE_COMPLETION flag to report errors only.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_coll_tx_ep, &gupcr_coll_lcq->fid,
				  FI_WRITE | FI_READ |
				  FI_SELECTIVE_COMPLETION));

#if LOCAL_MR_NEEDED
  /* NOTE: Create a local memory region before enabling endpoint.  */
  /* ... and memory region for local memory accesses.  */
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, USER_PROG_MEM_START,
				 USER_PROG_MEM_SIZE, FI_READ | FI_WRITE,
				 0, 0, 0, &gupcr_coll_lmr, NULL));
  /* NOTE: There is no need to bind local memory region to endpoint.  */
  /*       Hmm ... ? We can probably use only one throughout the runtime,  */
  /*       as counters and events are bound to endpoint.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_coll_tx_ep, &gupcr_coll_lmr->fid,
				  FI_READ | FI_WRITE));
#endif

  /* Enable TX endpoint.  */
  gupcr_fabric_call (fi_enable, (gupcr_coll_tx_ep));

  /* ... and memory region for remote inbound accesses.  */
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, gupcr_gmem_base, gupcr_gmem_size,
				 FI_REMOTE_READ | FI_REMOTE_WRITE, 0,
				 GUPCR_MR_COLL, 0, &gupcr_coll_mr, NULL));
  /* ... and counter for remote inbound writes.  */
  cntr_attr.events = FI_CNTR_EVENTS_COMP;
  cntr_attr.flags = 0;
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr,
				    &gupcr_coll_ct, NULL));
  gupcr_fabric_call (fi_mr_bind, (gupcr_coll_mr, &gupcr_coll_ct->fid,
				  FI_REMOTE_WRITE));
  /* ... and completion queue for remote inbound errors.  */
  cq_attr.size = 1;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr, &gupcr_coll_cq, NULL));
  gupcr_fabric_call (fi_mr_bind, (gupcr_coll_mr, &gupcr_coll_cq->fid,
			          FI_REMOTE_WRITE | FI_SELECTIVE_COMPLETION));
  /* Enable RX endpoint and bind MR.  */
  gupcr_fabric_call (fi_enable, (gupcr_coll_rx_ep));
  gupcr_fabric_call (fi_ep_bind, (gupcr_coll_rx_ep, &gupcr_coll_mr->fid,
			          FI_REMOTE_READ | FI_REMOTE_WRITE));
  gupcr_log (FC_COLL, "coll init completed");
}

/**
 * Release collectives resources.
 * @ingroup INIT
 */
void
gupcr_coll_fini (void)
{
  int status;
  gupcr_log (FC_COLL, "coll fini called");
  gupcr_fabric_call (fi_close, (&gupcr_coll_ct->fid));
  gupcr_fabric_call (fi_close, (&gupcr_coll_lct->fid));
  gupcr_fabric_call (fi_close, (&gupcr_coll_cq->fid));
  gupcr_fabric_call (fi_close, (&gupcr_coll_mr->fid));
#if LOCAL_MR_NEEDED
  gupcr_fabric_call (fi_close, (&gupcr_coll_lmr->fid));
#endif
  /* NOTE: Do not check for errors.  Fails occasionally.  */
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_coll_rx_ep->fid));
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_coll_tx_ep->fid));
}

/** @} */
