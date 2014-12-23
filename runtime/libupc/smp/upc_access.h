/*===-- upc_access.h - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _UPC_ACCESS_H_
#define _UPC_ACCESS_H_


//begin lib_access_prototypes
/* relaxed accesses */

extern u_intQI_t __getqi2 (upc_shared_ptr_t);
extern u_intHI_t __gethi2 (upc_shared_ptr_t);
extern u_intSI_t __getsi2 (upc_shared_ptr_t);
extern u_intDI_t __getdi2 (upc_shared_ptr_t);
#if GUPCR_TARGET64
extern u_intTI_t __getti2 (upc_shared_ptr_t);
#endif
extern float __getsf2 (upc_shared_ptr_t);
extern double __getdf2 (upc_shared_ptr_t);
extern long double __gettf2 (upc_shared_ptr_t);
extern long double __getxf2 (upc_shared_ptr_t);
extern void __getblk3 (void *, upc_shared_ptr_t, size_t);

extern void __putqi2 (upc_shared_ptr_t, u_intQI_t);
extern void __puthi2 (upc_shared_ptr_t, u_intHI_t);
extern void __putsi2 (upc_shared_ptr_t, u_intSI_t);
extern void __putdi2 (upc_shared_ptr_t, u_intDI_t);
#if GUPCR_TARGET64
extern void __putti2 (upc_shared_ptr_t, u_intTI_t);
#endif
extern void __putsf2 (upc_shared_ptr_t, float);
extern void __putdf2 (upc_shared_ptr_t, double);
extern void __puttf2 (upc_shared_ptr_t, long double);
extern void __putxf2 (upc_shared_ptr_t, long double);
extern void __putblk3 (upc_shared_ptr_t, void *, size_t);
extern void __copyblk3 (upc_shared_ptr_t, upc_shared_ptr_t, size_t);

/* strict accesses */

extern u_intQI_t __getsqi2 (upc_shared_ptr_t);
extern u_intHI_t __getshi2 (upc_shared_ptr_t);
extern u_intSI_t __getssi2 (upc_shared_ptr_t);
extern u_intDI_t __getsdi2 (upc_shared_ptr_t);
#if GUPCR_TARGET64
extern u_intTI_t __getsti2 (upc_shared_ptr_t);
#endif
extern float __getssf2 (upc_shared_ptr_t);
extern double __getsdf2 (upc_shared_ptr_t);
extern long double __getstf2 (upc_shared_ptr_t);
extern long double __getsxf2 (upc_shared_ptr_t);
extern void __getsblk3 (void *, upc_shared_ptr_t, size_t);

extern void __putsqi2 (upc_shared_ptr_t, u_intQI_t);
extern void __putshi2 (upc_shared_ptr_t, u_intHI_t);
extern void __putssi2 (upc_shared_ptr_t, u_intSI_t);
extern void __putsdi2 (upc_shared_ptr_t, u_intDI_t);
#if GUPCR_TARGET64
extern void __putsti2 (upc_shared_ptr_t, u_intTI_t);
#endif
extern void __putssf2 (upc_shared_ptr_t, float);
extern void __putsdf2 (upc_shared_ptr_t, double);
extern void __putstf2 (upc_shared_ptr_t, long double);
extern void __putsxf2 (upc_shared_ptr_t, long double);
extern void __putsblk3 (upc_shared_ptr_t, void *, size_t);
extern void __copysblk3 (upc_shared_ptr_t, upc_shared_ptr_t, size_t);

/* relaxed accesses (profiled) */

extern u_intQI_t __getgqi3 (upc_shared_ptr_t, const char *file, int line);
extern u_intHI_t __getghi3 (upc_shared_ptr_t, const char *file, int line);
extern u_intSI_t __getgsi3 (upc_shared_ptr_t, const char *file, int line);
extern u_intDI_t __getgdi3 (upc_shared_ptr_t, const char *file, int line);
#if GUPCR_TARGET64
extern u_intTI_t __getgti3 (upc_shared_ptr_t, const char *file, int line);
#endif
extern float __getgsf3 (upc_shared_ptr_t, const char *file, int line);
extern double __getgdf3 (upc_shared_ptr_t, const char *file, int line);
extern long double __getgtf3 (upc_shared_ptr_t, const char *file, int line);
extern long double __getgxf3 (upc_shared_ptr_t, const char *file, int line);
extern void __getgblk5 (void *, upc_shared_ptr_t, size_t, const char *file,
			int line);

extern void __putgqi4 (upc_shared_ptr_t, u_intQI_t, const char *file,
		       int line);
extern void __putghi4 (upc_shared_ptr_t, u_intHI_t, const char *file,
		       int line);
extern void __putgsi4 (upc_shared_ptr_t, u_intSI_t, const char *file,
		       int line);
extern void __putgdi4 (upc_shared_ptr_t, u_intDI_t, const char *file,
		       int line);
#if GUPCR_TARGET64
extern void __putgti4 (upc_shared_ptr_t, u_intTI_t, const char *file,
		       int line);
#endif
extern void __putgsf4 (upc_shared_ptr_t, float, const char *file, int line);
extern void __putgdf4 (upc_shared_ptr_t, double, const char *file, int line);
extern void __putgtf4 (upc_shared_ptr_t, long double, const char *file, int line);
extern void __putgxf4 (upc_shared_ptr_t, long double, const char *file, int line);
extern void __putgblk5 (upc_shared_ptr_t, void *, size_t, const char *file,
			int line);
extern void __copygblk5 (upc_shared_ptr_t, upc_shared_ptr_t, size_t,
			 const char *file, int line);

/* strict accesses (profiled) */

extern u_intQI_t __getsgqi3 (upc_shared_ptr_t, const char *file, int line);
extern u_intHI_t __getsghi3 (upc_shared_ptr_t, const char *file, int line);
extern u_intSI_t __getsgsi3 (upc_shared_ptr_t, const char *file, int line);
extern u_intDI_t __getsgdi3 (upc_shared_ptr_t, const char *file, int line);
#if GUPCR_TARGET64
extern u_intTI_t __getsgti3 (upc_shared_ptr_t, const char *file, int line);
#endif
extern float __getsgsf3 (upc_shared_ptr_t, const char *file, int line);
extern double __getsgdf3 (upc_shared_ptr_t, const char *file, int line);
extern long double __getsgtf3 (upc_shared_ptr_t, const char *file, int line);
extern long double __getsgxf3 (upc_shared_ptr_t, const char *file, int line);
extern void __getsgblk5 (void *, upc_shared_ptr_t, size_t, const char *file,
			 int line);

extern void __putsgqi4 (upc_shared_ptr_t, u_intQI_t, const char *file,
			int line);
extern void __putsghi4 (upc_shared_ptr_t, u_intHI_t, const char *file,
			int line);
extern void __putsgsi4 (upc_shared_ptr_t, u_intSI_t, const char *file,
			int line);
extern void __putsgdi4 (upc_shared_ptr_t, u_intDI_t, const char *file,
			int line);
#if GUPCR_TARGET64
extern void __putsgti4 (upc_shared_ptr_t, u_intTI_t, const char *file,
			int line);
#endif
extern void __putsgsf4 (upc_shared_ptr_t, float, const char *file, int line);
extern void __putsgdf4 (upc_shared_ptr_t, double, const char *file, int line);
extern void __putsgtf4 (upc_shared_ptr_t, long double, const char *file, int line);
extern void __putsgxf4 (upc_shared_ptr_t, long double, const char *file, int line);
extern void __putsgblk5 (upc_shared_ptr_t, void *, size_t, const char *file,
			 int line);
extern void __copysgblk5 (upc_shared_ptr_t, upc_shared_ptr_t, size_t,
			  const char *file, int line);

/* Miscellaneous access related prototypes.  */
extern void __upc_fence (void);

extern void upcr_llvm_getn (long, long, void *, size_t);
extern void upcr_llvm_getns (long, long, void *, size_t);
extern u_intQI_t upcr_llvm_get_i8 (long, long);
extern u_intQI_t upcr_llvm_get_i8s (long, long);
extern u_intHI_t upcr_llvm_get_i16 (long, long);
extern u_intHI_t upcr_llvm_get_i16s (long, long);
extern u_intSI_t upcr_llvm_get_i32 (long, long);
extern u_intSI_t upcr_llvm_get_i32s (long, long);
extern u_intDI_t upcr_llvm_get_i64 (long, long);
extern u_intDI_t upcr_llvm_get_i64s (long, long);
#if GUPCR_TARGET64
extern u_intTI_t upcr_llvm_get_i128 (long, long);
extern u_intTI_t upcr_llvm_get_i128s (long, long);
#endif
extern float upcr_llvm_get_float (long, long);
extern float upcr_llvm_get_floats (long, long);
extern double upcr_llvm_get_double (long, long);
extern double upcr_llvm_get_doubles (long, long);
extern void upcr_llvm_putn (const void *, long, long, size_t);
extern void upcr_llvm_putns (const void *, long, long, size_t);
extern void upcr_llvm_put_i8 (long, long, u_intQI_t);
extern void upcr_llvm_put_i8s (long, long, u_intQI_t);
extern void upcr_llvm_put_i16 (long, long, u_intHI_t);
extern void upcr_llvm_put_i16s (long, long, u_intHI_t);
extern void upcr_llvm_put_i32 (long, long, u_intSI_t);
extern void upcr_llvm_put_i32s (long, long, u_intSI_t);
extern void upcr_llvm_put_i64 (long, long, u_intDI_t);
extern void upcr_llvm_put_i64s (long, long, u_intDI_t);
#if GUPCR_TARGET64
extern void upcr_llvm_put_i128 (long, long, u_intTI_t);
extern void upcr_llvm_put_i128s (long, long, u_intTI_t);
#endif
extern void upcr_llvm_put_float (long, long, float);
extern void upcr_llvm_put_floats (long, long, float);
extern void upcr_llvm_put_double (long, long, double);
extern void upcr_llvm_put_doubles (long, long, double);

//end lib_access_prototypes

/* memory-to-memory operations (profiled) */
extern void upc_memcpyg (upc_shared_ptr_t dest, upc_shared_ptr_t src,
			 size_t n, const char *filename, int linenum);
extern void upc_memgetg (void *dest, upc_shared_ptr_t src, size_t n,
			 const char *filename, int linenum);
extern void upc_memputg (upc_shared_ptr_t dest, const void *src, size_t n,
			 const char *filename, int linenum);
extern void upc_memsetg (upc_shared_ptr_t dest, int c, size_t n,
			 const char *filename, int linenum);

#endif /* _UPC_ACCESS_H_ */
