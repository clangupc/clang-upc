/*===-- gupcr_barrier_sup.h - UPC Runtime Support Library ----------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_BARRIER_H_
#define _GUPCR_BARRIER_H_

/**
 * @file gupcr_barrier_sup.h
 * UPC barrier implementation support.
 *
 * @addtogroup BARRIER GUPCR Barrier Functions
 * @{
 */

/* Barrier direction (up or down).  */
enum barrier_dir
{ BARRIER_UP, BARRIER_DOWN };

/* Barrier support functions.  */

extern void gupcr_barrier_sup_init (void);
extern void gupcr_barrier_sup_fini (void);

extern void gupcr_barrier_put (enum barrier_dir, int *, int, int *);
extern void gupcr_barrier_tr_put (enum barrier_dir, int *, int, int *,
				   size_t);
extern void gupcr_barrier_atomic (int *, int, int *);
extern void gupcr_barrier_tr_atomic (enum barrier_dir, int *, int, int *,
				     size_t);
extern void gupcr_barrier_wait_up (size_t);
extern void gupcr_barrier_wait_down (size_t);
extern void gupcr_barrier_wait_delivery (size_t);
/** @} */
#endif /* gupcr_barrier_sup.h */
