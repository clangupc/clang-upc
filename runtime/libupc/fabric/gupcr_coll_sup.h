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
 * GUPC collectives implementation support routines.
 *
 * @addtogroup COLLECTIVES GUPCR Collectives Functions
 * @{
 */

/** Convert from UPC collectives char to network atomic type.  */
#define UPC_COLL_TO_COM_CHAR FI_INT8
#define UPC_COLL_TO_COM_UCHAR FI_UINT8
/** Convert from UPC collectives short to network atomic type.  */
#if __SIZEOF_SHORT__ == 2
#define UPC_COLL_TO_COM_SHORT FI_INT16
#define UPC_COLL_TO_COM_USHORT FI_UINT16
#elif __SIZEOF_SHORT__ == 4
#define UPC_COLL_TO_COM_SHORT FI_INT32
#define UPC_COLL_TO_COM_USHORT FI_UINT32
#else
#error "Size of short not supported"
#endif
/** Convert from UPC collectives int to Portals atomic type.  */
#if __SIZEOF_INT__ == 4
#define UPC_COLL_TO_COM_INT FI_INT32
#define UPC_COLL_TO_COM_UINT FI_UINT32
#elif __SIZEOF_INT__ == 8
#define UPC_COLL_TO_COM_INT FI_INT64
#define UPC_COLL_TO_COM_UINT FI_UINT64
#else
#error "Size of int not supported"
#endif
/** Convert from UPC collectives long to Portals atomic type.  */
#if __SIZEOF_LONG__ == 4
#define UPC_COLL_TO_COM_LONG FI_INT32
#define UPC_COLL_TO_COM_ULONG FI_UINT32
#elif __SIZEOF_LONG__ == 8
#define UPC_COLL_TO_COM_LONG FI_INT64
#define UPC_COLL_TO_COM_ULONG FI_UINT64
#else
#error "Size of long not supported"
#endif
/** Convert from UPC collectives float to Portals atomic type.  */
#define UPC_COLL_TO_COM_FLOAT FI_FLOAT
/** Convert from UPC collectives double to Portals atomic type.  */
#define UPC_COLL_TO_COM_DOUBLE FI_DOUBLE
/** Convert from UPC collectives long double to Portals atomic type.  */
#define UPC_COLL_TO_COM_LONG_DOUBLE FI_LONG_DOUBLE

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
			    size_t nbytes, enum fi_op op,
			    enum fi_datatype datatype);
void gupcr_coll_trigput_atomic (size_t dthread, size_t doffset,
				size_t soffset, size_t nbytes, enum fi_op op,
				enum fi_datatype datatype, size_t cnt);
void gupcr_coll_ack_wait (size_t cnt);
void gupcr_coll_signal_wait (size_t cnt);

void gupcr_coll_init (void);
void gupcr_coll_fini (void);

/** @} */

#endif /* gupcr_coll_sup.h */
