/*===-- gupcr_coll_sup.h - UPC Runtime Support Library -------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_COLL_SUP_H_
#define _GUPCR_COLL_SUP_H_ 1

/**
 * @file gupcr_coll_sup.h
 * GUPC Portals4 collectives implementation support routines.
 *
 * @addtogroup COLLECTIVES GUPCR Collectives Functions
 * @{
 */

/** Convert from UPC collectives char to Portals atomic type.  */
#define UPC_COLL_TO_PTL_CHAR PTL_INT8_T
#define UPC_COLL_TO_PTL_UCHAR PTL_UINT8_T
/** Convert from UPC collectives short to Portals atomic type.  */
#if __SIZEOF_SHORT__ == 2
#define UPC_COLL_TO_PTL_SHORT PTL_INT16_T
#define UPC_COLL_TO_PTL_USHORT PTL_UINT16_T
#elif __SIZEOF_SHORT__ == 4
#define UPC_COLL_TO_PTL_SHORT PTL_INT32_T
#define UPC_COLL_TO_PTL_USHORT PTL_UINT32_T
#else
#error "Size of short not supported"
#endif
/** Convert from UPC collectives int to Portals atomic type.  */
#if __SIZEOF_INT__ == 4
#define UPC_COLL_TO_PTL_INT PTL_INT32_T
#define UPC_COLL_TO_PTL_UINT PTL_UINT32_T
#elif __SIZEOF_INT__ == 8
#define UPC_COLL_TO_PTL_INT PTL_INT64_T
#define UPC_COLL_TO_PTL_UINT PTL_UINT64_T
#else
#error "Size of int not supported"
#endif
/** Convert from UPC collectives long to Portals atomic type.  */
#if __SIZEOF_LONG__ == 4
#define UPC_COLL_TO_PTL_LONG PTL_INT32_T
#define UPC_COLL_TO_PTL_ULONG PTL_UINT32_T
#elif __SIZEOF_LONG__ == 8
#define UPC_COLL_TO_PTL_LONG PTL_INT64_T
#define UPC_COLL_TO_PTL_ULONG PTL_UINT64_T
#else
#error "Size of long not supported"
#endif
/** Convert from UPC collectives float to Portals atomic type.  */
#define UPC_COLL_TO_PTL_FLOAT PTL_FLOAT
/** Convert from UPC collectives double to Portals atomic type.  */
#define UPC_COLL_TO_PTL_DOUBLE PTL_DOUBLE
/** Convert from UPC collectives long double to Portals atomic type.  */
#define UPC_COLL_TO_PTL_LONG_DOUBLE PTL_LONG_DOUBLE

extern int gupcr_coll_parent_thread;
extern int gupcr_coll_child_cnt;
extern int gupcr_coll_child_index;
extern int gupcr_coll_child[GUPCR_TREE_FANOUT];

/** Check if thread is the root thread by checking its parent.  */
#define IS_ROOT_THREAD (gupcr_coll_parent_thread == ROOT_PARENT)

void gupcr_coll_tree_setup (size_t newroot, size_t start, int nthreads);
void gupcr_coll_put (size_t dthread,
		     size_t doffset, size_t soffset, size_t nbytes);
void gupcr_coll_trigput (size_t dthread,
			 size_t doffset, size_t soffset, size_t nbytes,
			 size_t cnt);
void gupcr_coll_put_atomic (size_t dthread, size_t doffset, size_t soffset,
			    size_t nbytes, ptl_op_t op,
			    ptl_datatype_t datatype);
void gupcr_coll_trigput_atomic (size_t dthread, size_t doffset,
				size_t soffset, size_t nbytes, ptl_op_t op,
				ptl_datatype_t datatype, size_t cnt);
void gupcr_coll_ack_wait (size_t cnt);
void gupcr_coll_signal_wait (size_t cnt);

void gupcr_coll_init (void);
void gupcr_coll_fini (void);

/** @} */

#endif /* gupcr_coll_sup.h */
