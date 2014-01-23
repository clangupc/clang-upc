/*===-- upc_collective.h - UPC Runtime Support Library -------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#ifndef _UPC_COLLECTIVE_H_
#define _UPC_COLLECTIVE_H_

#include <upc_types.h>

/* Per the UPC collectives library specification, the following
   operations are defined in addition to those defined in upc_types.h.
   Values below 2^16 are reserved.  */
#define UPC_FUNC		(1UL<<16)
#define UPC_NONCOMM_FUNC	(1UL<<17)
#define UPC_MAX_COLL_OP		UPC_NONCOMM_FUNC

/* Function codes for error checking  */

#define UPC_BRDCST		0
#define UPC_SCAT		1
#define UPC_GATH		2
#define UPC_GATH_ALL		3
#define UPC_EXCH		4
#define UPC_PERM		5
#define UPC_RED			6
#define UPC_PRED		7
#define UPC_SORT		8

extern void upc_all_broadcast (shared void *dst,
			       shared const void *src,
			       size_t nbytes, upc_flag_t sync_mode);

extern void upc_coll_err (shared void *dst, shared const void *src,
			  shared const int *perm,
			  size_t nbytes,
			  upc_flag_t sync_mode,
			  size_t blk_size,
			  size_t nelems, upc_op_t op, upc_flag_t upc_coll_op);

extern void upc_all_exchange (shared void *dst, shared const void *src,
			      size_t nbytes, upc_flag_t sync_mode);

extern void upc_all_gather_all (shared void *dst,
				shared const void *src,
				size_t nbytes, upc_flag_t sync_mode);

extern void upc_all_gather (shared void *dst, shared const void *src,
			    size_t nbytes, upc_flag_t sync_mode);

extern void upc_coll_init (void);

extern void upc_all_permute (shared void *dst, shared const void *src,
			     shared const int *perm, size_t nbytes,
			     upc_flag_t sync_mode);

extern void upc_all_prefix_reduceC (shared void *dst, shared const void *src,
				    upc_op_t op, size_t nelems,
				    size_t blk_size,
				    signed char (*func) (signed char,
							 signed char),
				    upc_flag_t sync_mode);

extern void upc_all_prefix_reduceUC (shared void *dst,
				     shared const void *src, upc_op_t op,
				     size_t nelems, size_t blk_size,
				     unsigned char (*func) (unsigned char,
							    unsigned char),
				     upc_flag_t sync_mode);

extern void upc_all_prefix_reduceS (shared void *dst, shared const void *src,
				    upc_op_t op, size_t nelems,
				    size_t blk_size,
				    signed short (*func) (signed short,
							  signed short),
				    upc_flag_t sync_mode);

extern void upc_all_prefix_reduceUS (shared void *dst,
				     shared const void *src, upc_op_t op,
				     size_t nelems, size_t blk_size,
				     unsigned short (*func) (unsigned short,
							     unsigned short),
				     upc_flag_t sync_mode);

extern void upc_all_prefix_reduceI (shared void *dst, shared const void *src,
				    upc_op_t op, size_t nelems,
				    size_t blk_size,
				    signed int (*func) (signed int,
							signed int),
				    upc_flag_t sync_mode);

extern void upc_all_prefix_reduceUI (shared void *dst,
				     shared const void *src, upc_op_t op,
				     size_t nelems, size_t blk_size,
				     unsigned int (*func) (unsigned int,
							   unsigned int),
				     upc_flag_t sync_mode);

extern void upc_all_prefix_reduceL (shared void *dst, shared const void *src,
				    upc_op_t op, size_t nelems,
				    size_t blk_size,
				    signed long (*func) (signed long,
							 signed long),
				    upc_flag_t sync_mode);

extern void upc_all_prefix_reduceUL (shared void *dst,
				     shared const void *src, upc_op_t op,
				     size_t nelems, size_t blk_size,
				     unsigned long (*func) (unsigned long,
							    unsigned long),
				     upc_flag_t sync_mode);

extern void upc_all_prefix_reduceF (shared void *dst, shared const void *src,
				    upc_op_t op, size_t nelems,
				    size_t blk_size, float (*func) (float,
								    float),
				    upc_flag_t sync_mode);

extern void upc_all_prefix_reduceD (shared void *dst, shared const void *src,
				    upc_op_t op, size_t nelems,
				    size_t blk_size, double (*func) (double,
								     double),
				    upc_flag_t sync_mode);

extern void upc_all_prefix_reduceLD (shared void *dst, shared const void *src,
				     upc_op_t op, size_t nelems,
				     size_t blk_size,
				     long double (*func) (long double,
							  long double),
				     upc_flag_t sync_mode);

extern void upc_all_reduceC (shared void *dst, shared const void *src,
			     upc_op_t op, size_t nelems, size_t blk_size,
			     signed char (*func) (signed char, signed char),
			     upc_flag_t sync_mode);

extern void upc_all_reduceUC (shared void *dst, shared const void *src,
			      upc_op_t op, size_t nelems, size_t blk_size,
			      unsigned char (*func) (unsigned char,
						     unsigned char),
			      upc_flag_t sync_mode);

extern void upc_all_reduceS (shared void *dst, shared const void *src,
			     upc_op_t op, size_t nelems, size_t blk_size,
			     signed short (*func) (signed short,
						   signed short),
			     upc_flag_t sync_mode);

extern void upc_all_reduceUS (shared void *dst, shared const void *src,
			      upc_op_t op, size_t nelems, size_t blk_size,
			      unsigned short (*func) (unsigned short,
						      unsigned short),
			      upc_flag_t sync_mode);

extern void upc_all_reduceI (shared void *dst, shared const void *src,
			     upc_op_t op, size_t nelems, size_t blk_size,
			     signed int (*func) (signed int, signed int),
			     upc_flag_t sync_mode);

extern void upc_all_reduceUI (shared void *dst, shared const void *src,
			      upc_op_t op, size_t nelems, size_t blk_size,
			      unsigned int (*func) (unsigned int,
						    unsigned int),
			      upc_flag_t sync_mode);

extern void upc_all_reduceL (shared void *dst, shared const void *src,
			     upc_op_t op, size_t nelems, size_t blk_size,
			     signed long (*func) (signed long, signed long),
			     upc_flag_t sync_mode);

extern void upc_all_reduceUL (shared void *dst, shared const void *src,
			      upc_op_t op, size_t nelems, size_t blk_size,
			      unsigned long (*func) (unsigned long,
						     unsigned long),
			      upc_flag_t sync_mode);

extern void upc_all_reduceF (shared void *dst, shared const void *src,
			     upc_op_t op, size_t nelems, size_t blk_size,
			     float (*func) (float, float),
			     upc_flag_t sync_mode);

extern void upc_all_reduceD (shared void *dst, shared const void *src,
			     upc_op_t op, size_t nelems, size_t blk_size,
			     double (*func) (double, double),
			     upc_flag_t sync_mode);

extern void upc_all_reduceLD (shared void *dst, shared const void *src,
			      upc_op_t op, size_t nelems, size_t blk_size,
			      long double (*func) (long double, long double),
			      upc_flag_t sync_mode);

extern void upc_all_scatter (shared void *dst, shared const void *src,
			     size_t nbytes, upc_flag_t sync_mode);

extern void upc_all_sort (shared void *A, size_t elem_size,
			  size_t nelems, size_t blk_size,
			  int (*func) (shared void *, shared void *),
			  upc_flag_t sync_mode);

#endif /* !_UPC_COLLECTIVE_H_ */
