/*===-- gupcr_gmem.h - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_GMEM_H_
#define _GUPCR_GMEM_H_

/**
 * @file gupcr_gmem.h
 * GUPC Portals4 shared memory interface.
 */

/**
 * @addtogroup GMEM GUPCR Shared Memory Access
 * @{
 */

/* Configuration-defined limits.  */
/** Maximum size of the message that uses put bounce buffer.  */
#define GUPCR_GMEM_MAX_SAFE_PUT_SIZE 1*KILOBYTE

/** Max size of the user program.
 *
 * To simplify management of memory descriptors the entier user
 * program address space is mapped into one memory descriptor per
 * direction of the transfer.
 * Per linux kernel document: Documentation/x86/x86_64/mm.txt
 * the maximum size is 0x8000_0000_0000
 */
#define USER_PROG_MEM_SIZE  0x00008000000000000
/** Beginning of the user program */
#define USER_PROG_MEM_START NULL

//begin lib_inline_gmem
/** Check if shared memory of the specified thread can be accessed
    as node local reference.  */
#define GUPCR_GMEM_IS_LOCAL(thr) (gupcr_node_map[thr] != NULL)
/** Convert pointer-to-shared address filed into local address.  */
#define GUPCR_GMEM_OFF_TO_LOCAL(thr,off) (gupcr_node_map[thr] + off)

/** GMEM shared memory base */
extern void *gupcr_gmem_base;
//end lib_inline_gmem

/** GMEM shared memory size */
extern ptl_size_t gupcr_gmem_size;

/** GMEM get/put information tracking.
 *
 *  Track the information required to access global
 *  memory in a given direction (get/put) using non-blocking
 *  'get' and 'put' functions.
 */
typedef struct gupcr_gmem_xfer_info_struct
{
  /** Number of pending operations */
  ptl_size_t num_pending;
  /** Number of completed operations */
  ptl_size_t num_completed;
  /** Memory descriptor options */
  unsigned int md_options;
  /** Memory descriptor event handle */
  ptl_handle_eq_t eq_handle;
  /** Memory descriptor counting events handle */
  ptl_handle_ct_t ct_handle;
  /** Memory descriptor handle */
  ptl_handle_md_t md;
  /** Volatile memory descriptor handle */
  ptl_handle_md_t md_volatile;
} gupcr_gmem_xfer_info_t;
/** GET/PUT information tracking pointer type */
typedef gupcr_gmem_xfer_info_t *gupcr_gmem_xfer_info_p;

/** GET transfer tracking */
extern gupcr_gmem_xfer_info_t gupcr_gmem_gets;
/** PUT transfer tracking */
extern gupcr_gmem_xfer_info_t gupcr_gmem_puts;

/** PUT "bounce buffer" bytes in use */
extern size_t gupcr_gmem_put_bb_used;

//begin lib_gmem
extern void gupcr_gmem_sync (void);
//end lib_gmem

//begin lib_inline_gmem

/** If TRUE, a strict PUT operation is pending */
extern int gupcr_pending_strict_put;

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

extern size_t gupcr_gmem_heap_base_offset;
extern size_t gupcr_gmem_heap_size;

extern void gupcr_gmem_init (void);
extern void gupcr_gmem_fini (void);

/** @} */
#endif /* gupcr_gmem.h */
