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

/* Barrier support functions.  */

extern void gupcr_barrier_sup_init (void *bbase, int bsize);
extern void gupcr_barrier_sup_fini (void);

extern void gupcr_barrier_wait_put (void *, int, void *, size_t);
extern void gupcr_barrier_wait_put_completion (void);
extern void gupcr_barrier_wait_event (void);
extern void gupcr_barrier_notify_put (void *, int, void *);
extern void gupcr_barrier_notify_event (size_t);
extern void gupcr_barrier_tr_put (void *, int, void *, size_t, size_t, int);
extern void gupcr_barrier_notify_tr_put (void *, int, void *, size_t);
/** @} */
#endif /* gupcr_barrier_sup.h */
