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

#define __GUPCR_STR__(S) #S
#define __GUPCR_XSTR__(S) __GUPCR_STR__(S)

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
#define	GUPCR_SERVICE_BARRIER		2
/** Reserved */
#define	GUPCR_SERVICE_RESERVED_3	3
/** Lock signaling network connection */
#define	GUPCR_SERVICE_LOCK		4
/** Shutdown service signaling network connection */
#define	GUPCR_SERVICE_SHUTDOWN		5
/** Collectives service signaling network connection */
#define	GUPCR_SERVICE_COLL		6
/** Non-blocking transfers network connection */
#define	GUPCR_SERVICE_NB		7
/** Implicit non-blocking transfers network connection */
#define	GUPCR_SERVICE_NBI		8
/** Memory atomic functions network connection */
#define	GUPCR_SERVICE_ATOMIC		9
/** Number of services */
#define	GUPCR_SERVICE_COUNT		10
/** Number of service bits */
#define	GUPCR_SERVICE_BITS		8
/** @} */

/* GUPCR Memory Regions keys */
#define	GUPCR_MR_GMEM		1
#define	GUPCR_MR_BARRIER_NOTIFY	2
#define	GUPCR_MR_BARRIER_WAIT	3
#define	GUPCR_MR_BARRIER_SIGNAL	4
#define	GUPCR_MR_LOCK		5
#define	GUPCR_MR_SHUTDOWN	6
#define	GUPCR_MR_COLL		7
#define	GUPCR_MR_COLL_SIGNAL	8
#define	GUPCR_MR_NB		9
#define	GUPCR_MR_ATOMIC		10
#define	GUPCR_MR_COUNT		11

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
/** Service endpoint definition */
struct gupcr_epinfo
{
  const char *name;	/** Name of the service */
  int service;		/** Service scalable context number */
  fab_ep_t ep;		/** Service endpoint */
  fab_ep_t tx_ep;	/** TX context endpoint (scalable) */
  fab_ep_t rx_ep;	/** RX context endpoint (scalable) */
  fab_av_t av;		/** Service AV */
  char *epnames;	/** Service endpoint target names */
};
typedef struct gupcr_epinfo gupcr_epinfo_t;

/** Fabric info */
extern fab_info_t gupcr_fi;
/** Fabric domain */
extern fab_domain_t gupcr_fd;
/** Max ordered size - per network interface */
extern size_t gupcr_max_order_size;
#define GUPCR_MAX_PUT_ORDERED_SIZE gupcr_max_order_size
/** Max size of data (put, get, or reply) */
extern size_t gupcr_max_msg_size;
#define GUPCR_MAX_MSG_SIZE gupcr_max_msg_size
/** Max size of data that can use optimized put operations */
extern size_t gupcr_max_optim_size;
#define GUPCR_MAX_OPTIM_SIZE gupcr_max_optim_size
/** Local buffer alignment for RMA/... */
extern size_t gupcr_fabric_alignment;
#define GUPCR_FABRIC_ALIGNMENT gupcr_fabric_alignment
/** Support for scalable endpoints */
extern int gupcr_enable_scalable_ctx;
#define GUPCR_FABRIC_SCALABLE_CTX() (gupcr_enable_scalable_ctx != 0)
/** Support for target MR notifications (FI_RMA_EVENT) */
extern int gupcr_enable_rma_event;
#define GUPCR_FABRIC_RMA_EVENT() (gupcr_enable_rma_event != 0)
/** Support for scalable MR (FI_MR_SCALABLE) */
extern int gupcr_enable_mr_scalable;
#define GUPCR_FABRIC_MR_SCALABLE() (gupcr_enable_mr_scalable != 0)
/** Max time to wait for operation complete (10s) */
#define GUPCR_TRANSFER_TIMEOUT -1
/** Default size for the error CW */
#define GUPCR_CQ_ERROR_SIZE 10

//end lib_fabric

/** Execute fabric call and abort if error */
#define gupcr_fabric_call(fabric_func, args)				\
    {									\
      int pstatus;							\
      do								\
        {								\
          pstatus = fabric_func args;					\
	  if (pstatus && pstatus != -FI_EAGAIN)				\
	    gupcr_fatal_error ("UPC runtime fabric call "		\
	                     "`%s' on thread %d failed: %s\n", 		\
			     __STRING(fabric_func), gupcr_get_rank (),	\
	                     fi_strerror (-pstatus));			\
        }								\
      while (pstatus == -FI_EAGAIN);					\
    }
/** Execute fabric calls that return size  and abort if error */
#define gupcr_fabric_call_size(fabric_func, size, args)			\
    do									\
      {									\
        size = fabric_func args;					\
	if (size < 0 && size != -FI_EAGAIN)				\
	  gupcr_fatal_error ("UPC runtime fabric call "			\
	                     "`%s' on thread %d failed: %s\n", 		\
			     __STRING(fabric_func), gupcr_get_rank (),	\
	                     fi_strerror (-size));			\
      }									\
    while (size == -FI_EAGAIN)

/** Execute fabric call and return status */
#define gupcr_fabric_call_nc(fabric_func, pstatus, args)		\
    do									\
      {									\
        pstatus = fabric_func args;					\
      }									\
    while (0)

/** Execute fabric counter wait and process errors */
#define gupcr_fabric_call_cntr_wait(args, msg, queue)			\
    do									\
      {									\
	int status;							\
        status = fi_cntr_wait args;					\
        switch (status)							\
	  {								\
	    case FI_SUCCESS:						\
	      break;							\
	    case -FI_ETIMEDOUT:						\
	      gupcr_fatal_error ("[%d] TIMEOUT: %s", gupcr_get_rank(),	\
				  msg);					\
	      break;							\
	    default:							\
	      gupcr_process_fail_events (status, msg, queue);		\
	      gupcr_abort ();						\
	  }								\
      } while (0);

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

/* Support for target memory regions types.  If scalable
 * memory regions are supported then offset into the memory
 * region is being passed as the target address for all RMA
 * calls.  Otherwise, an actual target virtual address is used.
 *
 * "gupcr_mr_keys" holds information on all MR keys in the
 * system.  Information on MR keys and virtual addresses is
 * exchanged when each of the subsystems is initialized.
 * All of this is important if FI_MR_BASIC type of memory
 * regions are used: (1) a memory key cannot be specified by the
 * user, and (2) UPC shared memory is allocated with shmem or
 * mmap calls (there are now guaranties that allocated memory will
 * have the same base address).
 */

/** Remote Memory Region Information */
struct gupcr_memreg
{
  char *addr;
  uint64_t key;
};
typedef struct gupcr_memreg gupcr_memreg_t;

/** Gather remote memory region keys */
#define GUPCR_GATHER_MR_KEYS(name,mr_handle,mr_base) \
  { \
    uint64_t mr_key; \
    gupcr_fabric_call_nc (fi_mr_key, mr_key, (mr_handle)); \
    gupcr_##name##_mr_keys = malloc (sizeof (gupcr_memreg_t) * THREADS); \
    if (!gupcr_##name##_mr_keys) \
      gupcr_fatal_error ("cannot allocate memory for MR key exchange"); \
    gupcr_fabric_mr_exchg (__GUPCR_XSTR__(name)"_mr", gupcr_##name##_mr_keys, mr_key, \
			 (char *) mr_base); \
  }

/**
 * Get the remote memory region key.
 */
#define GUPCR_REMOTE_MR_KEY(mrkeys,thread) \
  gupcr_##mrkeys##_mr_keys[thread].key

/**
 *  Calculate the remote memory region address.
 */
#define GUPCR_REMOTE_MR_ADDR(mrkeys,thread,offset) \
  GUPCR_FABRIC_MR_SCALABLE() ? \
    offset : \
    offset + (uint64_t) gupcr_##mrkeys##_mr_keys[thread].addr

/** @} */

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
extern void gupcr_fabric_ep_create (gupcr_epinfo_t * epinfo);
extern void gupcr_fabric_ep_delete (gupcr_epinfo_t * epinfo);
extern void gupcr_nodetree_setup (void);
extern void gupcr_fabric_mr_exchg (const char *, gupcr_memreg_t *, uint64_t,
				   char *);

#endif /* gupcr_fabric.h */
