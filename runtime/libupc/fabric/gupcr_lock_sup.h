/*===-- gupcr_lock_sup.h - UPC Runtime Support Library -------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_LOCK_SUP_H_
#define _GUPCR_LOCK_SUP_H_

/**
 * @file gupcr_lock_sup.h
 * GUPC Portals4 lock implementation support routines.
 */

extern void gupcr_lock_init (void);
extern void gupcr_lock_fini (void);
extern int gupcr_lock_cswap (size_t, size_t, void *, void *, size_t);
extern void gupcr_lock_swap (size_t, size_t, void *, void *, size_t);
extern void gupcr_lock_put (size_t, size_t, void *, size_t);
extern void gupcr_lock_get (size_t, size_t, void *, size_t);
extern void gupcr_lock_wait (void);

/* See: gupcr_alloc.upc.  */
extern void gupcr_lock_link_init (void);
extern void gupcr_lock_free_init (void);
extern void gupcr_lock_heap_sup_init (void);

#endif /* gupcr_lock_sup.h */
