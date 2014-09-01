/*==========-- gupcr_iface.h - UPC Runtime Interfaces --------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_IFACE_H_
#define _GUPCR_IFACE_H_

/**
 * @file gupcr_iface.h
 * GUPC network runtime interface.
 */

/**
 * @addtogroup IFACE GUPCR Shared Memory Access
 * @{
 */

/* NODE interfaces  */
//begin lib_node_local
/** Memory map for threads that share node local memory.  */
extern char **gupcr_node_map;

/** Check if shared memory of the specified thread can be accessed
    as node local reference.  */
#define GUPCR_GMEM_IS_LOCAL(thr) (gupcr_node_map[thr] != NULL)
/** Convert pointer-to-shared address filed into local address.  */
#define GUPCR_GMEM_OFF_TO_LOCAL(thr,off) (gupcr_node_map[thr] + off)
//end lib_node_local

/* GMEM interfaces  */
//begin lib_inline_gmem
/** Number of pending put operations.  */
extern int gupcr_pending_strict_put;

/** GMEM shared memory base */
extern void *gupcr_gmem_base;

extern void gupcr_gmem_sync_gets (void);
extern void gupcr_gmem_sync_puts (void);
extern void gupcr_gmem_get (void *dest, int rthread, size_t roffset,
			    size_t n);
extern void gupcr_gmem_put (int rthread, size_t roffset, const void *src,
			    size_t n);
extern void gupcr_gmem_copy (int dthread, size_t doffset, int sthread,
			     size_t soffset, size_t n);
extern void gupcr_gmem_set (int dthread, size_t doffset, int c, size_t n);
//end lib_inline_gmem
//begin lib_gmem
extern void gupcr_gmem_sync (void);
//end lib_gmem

/* BROADCAST interfaces.  */
extern void gupcr_broadcast_get (void *value, size_t nbytes);
extern void gupcr_broadcast_put (void *value, size_t nbytes);

/* MAPPING interfaces.  */
extern int gupcr_get_rank_nid (int rank);

#endif /* gupcr_iface.h */
/** @} */
