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
 * GUPC atomics implementation support routines.
 *
 * @addtogroup ATOMIC GUPCR Atomics Support Functions
 * @{
 */

/** Maximum size of atomic types */
#define GUPC_MAX_ATOMIC_SIZE 16

/** Convert from UPC atomics into network atomic type */
#if __SIZEOF_INT__ == 4
#define UPC_ATOMIC_TO_COM_INT FI_INT32
#define UPC_ATOMIC_TO_COM_UINT FI_UINT32
#elif __SIZEOF_INT__ == 8
#define UPC_ATOMIC_TO_COM_INT FI_INT64
#define UPC_ATOMIC_TO_COM_UINT FI_UINT64
#else
#error "Size of int not supported"
#endif
/** Convert from UPC atomics long to network atomic type */
#if __SIZEOF_LONG__ == 4
#define UPC_ATOMIC_TO_COM_LONG FI_INT32
#define UPC_ATOMIC_TO_COM_ULONG FI_UINT32
#elif __SIZEOF_LONG__ == 8
#define UPC_ATOMIC_TO_COM_LONG FI_INT64
#define UPC_ATOMIC_TO_COM_ULONG FI_UINT64
#else
#error "Size of long not supported"
#endif
/** Convert from UPC atomic int32 to network atomic type */
#define UPC_ATOMIC_TO_COM_INT32 FI_INT32
#define UPC_ATOMIC_TO_COM_UINT32 FI_UINT32
/** Convert from UPC atomic int64 to network atomic type */
#define UPC_ATOMIC_TO_COM_INT64 FI_INT64
#define UPC_ATOMIC_TO_COM_UINT64 FI_UINT64
/** Convert from UPC atomic float to network atomic type */
#define UPC_ATOMIC_TO_COM_FLOAT FI_FLOAT
/** Convert from UPC atomic double to network atomic type */
#define UPC_ATOMIC_TO_COM_DOUBLE FI_DOUBLE

/** @} */

void gupcr_atomic_put (size_t, size_t, size_t, enum fi_op, enum fi_datatype);
void gupcr_atomic_get (size_t, size_t, void *, enum fi_datatype);
void gupcr_atomic_set (size_t, size_t, void *, const void *, enum fi_datatype);
void gupcr_atomic_cswap (size_t, size_t, void *, const void *,
			 const void *, enum fi_datatype);
void gupcr_atomic_op (size_t, size_t, void *, const void *,
		      enum fi_op, enum fi_datatype);
void gupcr_atomic_init (void);
void gupcr_atomic_fini (void);

#endif /* gupcr_atomic_sup.h */
