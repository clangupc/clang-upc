/*===-- gupcr_atomic_sup.h - UPC Runtime Support Library -----------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#ifndef _GUPCR_ATOMIC_SUP_H_
#define _GUPCR_ATOMIC_SUP_H_ 1

/**
 * @file gupcr_atomic_sup.h
 * GUPC Portals4 atomics implementation support routines.
 *
 * @addtogroup ATOMIC GUPCR Atomics Support Functions
 * @{
 */

/** Maximum size of atomic types */
#define GUPC_MAX_ATOMIC_SIZE 16

/** Convert from UPC atomics int to Portals atomic type */
#if __SIZEOF_INT__ == 4
#define UPC_ATOMIC_TO_PTL_INT PTL_INT32_T
#define UPC_ATOMIC_TO_PTL_UINT PTL_UINT32_T
#elif __SIZEOF_INT__ == 8
#define UPC_ATOMIC_TO_PTL_INT PTL_INT64_T
#define UPC_ATOMIC_TO_PTL_UINT PTL_UINT64_T
#else
#error "Size of int not supported"
#endif
/** Convert from UPC atomics long to Portals atomic type */
#if __SIZEOF_LONG__ == 4
#define UPC_ATOMIC_TO_PTL_LONG PTL_INT32_T
#define UPC_ATOMIC_TO_PTL_ULONG PTL_UINT32_T
#elif __SIZEOF_LONG__ == 8
#define UPC_ATOMIC_TO_PTL_LONG PTL_INT64_T
#define UPC_ATOMIC_TO_PTL_ULONG PTL_UINT64_T
#else
#error "Size of long not supported"
#endif
/** Convert from UPC atomic int32 to Portals atomic type */
#define UPC_ATOMIC_TO_PTL_INT32 PTL_INT32_T
#define UPC_ATOMIC_TO_PTL_UINT32 PTL_UINT32_T
/** Convert from UPC atomic int64 to Portals atomic type */
#define UPC_ATOMIC_TO_PTL_INT64 PTL_INT64_T
#define UPC_ATOMIC_TO_PTL_UINT64 PTL_UINT64_T
/** Convert from UPC atomic float to Portals atomic type */
#define UPC_ATOMIC_TO_PTL_FLOAT PTL_FLOAT
/** Convert from UPC atomic double to Portals atomic type */
#define UPC_ATOMIC_TO_PTL_DOUBLE PTL_DOUBLE

/** @} */

void gupcr_atomic_put (size_t, size_t, size_t, ptl_op_t op, ptl_datatype_t);
void gupcr_atomic_get (size_t, size_t, void *, ptl_datatype_t);
void gupcr_atomic_set (size_t, size_t, void *, const void *, ptl_datatype_t);
void gupcr_atomic_cswap (size_t, size_t, void *, const void *,
			 const void *, ptl_datatype_t);
void gupcr_atomic_op (size_t, size_t, void *, const void *,
		      ptl_op_t, ptl_datatype_t);
void gupcr_atomic_init (void);
void gupcr_atomic_fini (void);

#endif /* gupcr_atomic_sup.h */
