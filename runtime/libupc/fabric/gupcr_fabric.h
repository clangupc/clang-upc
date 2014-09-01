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
#include <rdma/fi_ucma.h>

/**
 * @file gupcr_fabric.h
 * GUPC Libfabric Global Definitions.
 */

/**
 * @addtogroup CONFIG GUPCR Configuration
 * @{
 */

/* GUPCR Service Table Entries */
/** Memory put/get functions network connection */
#define	GUPCR_SERVICE_GMEM		0
/** Barrier messages to parent node network connection */
#define	GUPCR_SERVICE_BARRIER_UP	1
/** Barrier messages from parent node network connection */
#define	GUPCR_SERVICE_BARRIER_DOWN	2
/** Lock signaling network connection */
#define	GUPCR_SERVICE_LOCK		3
/** Shutdown service signaling network connection */
#define	GUPCR_SERVICE_SHUTDOWN		4
/** Collectives service signaling network connection */
#define	GUPCR_SERVICE_COLL		5
/** Non-blocking transfers network connection */
#define	GUPCR_SERVICE_NB		6
/** @} */

//begin lib_fabric

/** Max ordered size - per network interface */
extern size_t gupcr_max_ordered_size;
#define GUPCR_MAX_PUT_ORDERED_SIZE gupcr_max_ordered_size
/** Max size of a message (put, get, or reply) */
extern size_t gupcr_max_msg_size;
#define GUPCR_MAX_MSG_SIZE gupcr_max_msg_size
/** Max size of a message that can use volatile memory descriptor */
extern size_t gupcr_max_volatile_size;
#define GUPCR_MAX_VOLATILE_SIZE gupcr_max_volatile_size

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
	                     gupcr_strptlerror (pstatus));		\
      }									\
    while (0)

/** Execute fabric call and return status if there is no fatal error */
#define gupcr_fabric_call_with_status(fabric_func, pstatus, args)	\
    do									\
      {									\
        pstatus = fabric_func args;					\
	if (pstatus)							\
	  gupcr_fatal_error ("UPC runtime fabric call "			\
	                     "`%s' on thread %d failed: %s\n",		\
			     __STRING(fabric_func), gupcr_get_rank (),	\
	                     gupcr_strptlerror (pstatus));		\
      }									\
    while (0)

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

extern const char *gupcr_strptlerror (int);
extern const char *gupcr_streqtype (uint64_t);
extern const char *gupcr_strptlop (enum fi_op);
extern const char *gupcr_strptldatatype (enum fi_datatype);
extern void gupcr_process_fail_events (struct fid_eq);
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
