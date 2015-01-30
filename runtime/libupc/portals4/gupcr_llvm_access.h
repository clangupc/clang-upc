/*===-- upc_llvm_access.h - UPC Runtime Support Library ------------------===
|*
|*                  The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_LLVM_ACCESS_H_
#define _GUPCR_LLVM_ACCESS_H_

extern u_intQI_t upcr_llvm_get_i8 (long thread, size_t offset);
extern u_intHI_t upcr_llvm_get_i16 (long thread, size_t offset);
extern u_intSI_t upcr_llvm_get_i32 (long thread, size_t offset);
extern u_intDI_t upcr_llvm_get_i64 (long thread, size_t offset);
#if GUPCR_TARGET64
extern u_intTI_t upcr_llvm_get_i128 (long thread, size_t offset);
#endif /* GUPCR_TARGET64 */
extern float upcr_llvm_get_float (long thread, size_t offset);
extern double upcr_llvm_get_double (long thread, size_t offset);
extern void upcr_llvm_getn (long thread, size_t offset, void *dest, size_t n);
extern void upcr_llvm_put_i8 (long thread, size_t offset, u_intQI_t v);
extern void upcr_llvm_put_i16 (long thread, size_t offset, u_intHI_t v);
extern void upcr_llvm_put_i32 (long thread, size_t offset, u_intSI_t v);
extern void upcr_llvm_put_i64 (long thread, size_t offset, u_intDI_t v);
#if GUPCR_TARGET64
extern void upcr_llvm_put_i128 (long thread, size_t offset, u_intTI_t v);
#endif /* GUPCR_TARGET64 */
extern void upcr_llvm_put_float (long thread, size_t offset, float v);
extern void upcr_llvm_put_double (long thread, size_t offset, double v);
extern void upcr_llvm_putn (void *src, long thread, size_t offset, size_t n);
extern void upcr_llvm_copyn (long dthread, size_t doffset,
			     long sthread, size_t soffset, size_t n);
extern u_intQI_t upcr_llvm_get_i8s (long thread, size_t offset);
extern u_intHI_t upcr_llvm_get_i16s (long thread, size_t offset);
extern u_intSI_t upcr_llvm_get_i32s (long thread, size_t offset);
extern u_intDI_t upcr_llvm_get_i64s (long thread, size_t offset);
#if GUPCR_TARGET64
extern u_intTI_t upcr_llvm_get_i128s (long thread, size_t offset);
#endif /* GUPCR_TARGET64 */
extern float upcr_llvm_get_floats (long thread, size_t offset);
extern double upcr_llvm_get_doubles (long thread, size_t offset);
extern void upcr_llvm_getns (long thread, size_t offset, void *dest,
			     size_t n);
extern void upcr_llvm_put_i8s (long thread, size_t offset, u_intQI_t v);
extern void upcr_llvm_put_i16s (long thread, size_t offset, u_intHI_t v);
extern void upcr_llvm_put_i32s (long thread, size_t offset, u_intSI_t v);
extern void upcr_llvm_put_i64s (long thread, size_t offset, u_intDI_t v);
#if GUPCR_TARGET64
extern void upcr_llvm_put_i128s (long thread, size_t offset, u_intTI_t v);
#endif /* GUPCR_TARGET64 */
extern void upcr_llvm_put_floats (long thread, size_t offset, float v);
extern void upcr_llvm_put_doubles (long thread, size_t offset, double v);
extern void upcr_llvm_putns (void *src, long thread, size_t offset, size_t n);
extern void upcr_llvm_copyns (long dthread, size_t doffset,
			      long sthread, size_t soffset, size_t n);

#endif /* gupcr_llvm_access.h */
