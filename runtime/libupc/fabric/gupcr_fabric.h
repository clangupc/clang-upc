/*===-- gupcr_fabric.h - UPC Runtime Support Library --------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_FABRIC_H_
#define _GUPCR_FABRIC_H_

#include <rdma/fabric.h>
#include <rdma/fi_atomic.h>
#include <rdma/fi_cm.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_endpoint.h>
#include <rdma/fi_errno.h>
#include <rdma/fi_prov.h>
#include <rdma/fi_rma.h>
#include <rdma/fi_trigger.h>

/**
 * @file gupcr_fabric.h
 * GUPC Libfabric Global Definitions.
 */

/**
 * @addtogroup CONFIG GUPCR Configuration
 * @{
 */

/* GUPCR Service Table Entries */
/** Reserved endpoint context */
#define	GUPCR_SERVICE_RESERVED		0
/** Memory put/get functions network connection */
#define	GUPCR_SERVICE_GMEM		1
/** Barrier messages to parent node network connection */
#define	GUPCR_SERVICE_BARRIER_UP	2
/** Barrier messages from parent node network connection */
#define	GUPCR_SERVICE_BARRIER_DOWN	3
/** Lock signaling network connection */
#define	GUPCR_SERVICE_LOCK		4
/** Shutdown service signaling network connection */
#define	GUPCR_SERVICE_SHUTDOWN		5
/** Collectives service signaling network connection */
#define	GUPCR_SERVICE_COLL		6
/** Non-blocking transfers network connection */
#define	GUPCR_SERVICE_NB		7
/** Number of services */
#define	GUPCR_SERVICE_COUNT		8
/** Number of service bits (? TODO - no match with docs) */
#define	GUPCR_SERVICE_BITS		8
/** @} */

/* GUPCR Memory Regions keys */
#define	GUPCR_MR_GMEM		1
#define	GUPCR_MR_BARRIER_UP	2
#define	GUPCR_MR_BARRIER_DOWN	3
#define	GUPCR_MR_LOCK		4
#define	GUPCR_MR_SHUTDOWN	5
#define	GUPCR_MR_COLL		6
#define	GUPCR_MR_NB		7

//begin lib_fabric

typedef struct fi_info *fab_info_t;
typedef struct fi_resource fab_res_t;
typedef struct fid_fabric *fab_t;
typedef struct fid_domain *fab_domain_t;
typedef struct fid_ep *fab_ep_t;
typedef struct fid_av *fab_av_t;
typedef struct fid_cq *fab_cq_t;
typedef struct fid_eq *fab_eq_t;
typedef struct fid_cntr *fab_cntr_t;
typedef struct fid_mr *fab_mr_t;
typedef struct fi_tx_attr tx_attr_t;
typedef struct fi_rx_attr rx_attr_t;
typedef struct fi_ep_attr ep_attr_t;
typedef struct fi_cntr_attr cntr_attr_t;
typedef struct fi_cq_attr cq_attr_t;
typedef struct fi_eq_attr eq_attr_t;
typedef struct fi_av_attr av_attr_t;
typedef struct fi_triggered_context fi_trig_t;

/** Endpoint to/form Rank/Service mapping */
extern fab_ep_t gupcr_ep;

/** Get endpoint for Rank/Service */
#define GUPCR_GET_EP(rank, service) \
	gupcr_assert (gupcr_ep); \
	&gupcr_ep[rank][service]
/** Set endpoint for Rank/Service */
#define GUPCR_SET_EP(rank, service, ep) \
	gupcr_assert (gupcr_ep); \
	gupcr_assert (ep); \
	memcpy (&gupcr_ep[rank][service], ep, sizeof (struct fid_ep));

/** Fabric info */
extern fab_info_t gupcr_fi;
/** Fabric domain */
extern fab_domain_t gupcr_fd;
/* TODO: Workaround for multiple endpoints under the same PTE.  */
extern fab_info_t gupcr_fi_lock;
extern fab_domain_t gupcr_fd_lock;
/** Max ordered size - per network interface */
extern size_t gupcr_max_order_size;
#define GUPCR_MAX_PUT_ORDERED_SIZE gupcr_max_order_size
/** Max size of data (put, get, or reply) */
extern size_t gupcr_max_msg_size;
#define GUPCR_MAX_MSG_SIZE gupcr_max_msg_size
/** Max size of data that can use optimized put operations */
extern size_t gupcr_max_optim_size;
#define GUPCR_MAX_OPTIM_SIZE gupcr_max_optim_size
/** Max time to wait for operation complete (10s) */
#define GUPCR_TRANSFER_TIMEOUT 10000

//end lib_fabric

/** Execute fabric call and abort if error */
#define gupcr_fabric_call(fabric_func, args)				\
    do									\
      {									\
        int pstatus;							\
        pstatus = fabric_func args;					\
	if (pstatus)							\
	  gupcr_fatal_error ("UPC runtime fabric call "			\
	                     "`%s' on thread %d failed: %s\n", 		\
			     __STRING(fabric_func), gupcr_get_rank (),	\
	                     gupcr_strfaberror (pstatus));		\
      }									\
    while (0)

/** Execute fabric call and return status */
#define gupcr_fabric_call_nc(fabric_func, pstatus, args)		\
    do									\
      {									\
        pstatus = fabric_func args;					\
      }									\
    while (0)

/** Check for timeout error code */
#define GUPCR_TIMEOUT_CHECK(status, msg, queue) 			\
    {									\
      int errcode = -(int)status;					\
      if (errcode)							\
      {									\
        switch (errcode)						\
	  {								\
	    case FI_ETIMEDOUT:						\
	      gupcr_fatal_error ("[%d] TIMEOUT: %s", gupcr_get_rank(),	\
				  msg);					\
	      break;							\
	    default:							\
	      gupcr_process_fail_events (errcode, msg, queue);		\
	      gupcr_abort ();						\
	  }								\
      }									\
    }

/**
 * @addtogroup GLOBAL GUPCR Global Variables
 * @{
 */

/* For the purposes of implementing GUPC barriers, all UPC threads
   in a given job are organized as a tree.  Thread 0 is the
   root node (at the top of the tree).  Other threads can represent
   either an inner node (has at least one child), or a leaf
   node (has no children).  */

/** Parent of the root thread */
#define ROOT_PARENT -1

/** Thread IDs of all children of the current thread.  */
extern int gupcr_child[GUPCR_TREE_FANOUT];
/** Number of children of the current thread.  */
extern int gupcr_child_cnt;
/** Parent thread ID of the current thread.
    The tree root thread has a parent ID of -1.  */
extern int gupcr_parent_thread;

/** @} */

extern const char *gupcr_strfaberror (int);
extern const char *gupcr_streqtype (uint64_t);
extern const char *gupcr_strop (enum fi_op);
extern const char *gupcr_strdatatype (enum fi_datatype);
extern void gupcr_process_fail_events (int, const char *, fab_cq_t);
extern enum fi_datatype gupcr_get_atomic_datatype (int);
extern size_t gupcr_get_atomic_size (enum fi_datatype);
extern int gupcr_get_rank (void);
extern int gupcr_get_threads_count (void);
extern int gupcr_get_rank_pid (int rank);
extern int gupcr_get_rank_nid (int rank);
extern void gupcr_startup_barrier (void);

extern void gupcr_fabric_init (void);
extern void gupcr_fabric_fini (void);
extern void gupcr_fabric_ni_init (void);
extern void gupcr_fabric_ni_fini (void);
extern void gupcr_nodetree_setup (void);

#endif /* gupcr_fabric.h */
