/*===-- gupcr_runtime.h - UPC Runtime Support Library --------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_RUNTIME_H_
#define _GUPCR_RUNTIME_H_

/**
 * @file gupcr_runtime.h
 * GUPC Libfabric Runtime.
 */

extern int gupcr_runtime_init (void);
extern int gupcr_runtime_fini (void);
extern int gupcr_runtime_get_rank (void);
extern int gupcr_runtime_get_size (void);
extern void gupcr_runtime_barrier (void);
extern int gupcr_runtime_put (const char *, void *, size_t);
extern int gupcr_runtime_get (int, const char *, void *, size_t);
extern int gupcr_runtime_exchange (const char *, void *, size_t, void *);
#endif /* gupcr_runtime.h */
