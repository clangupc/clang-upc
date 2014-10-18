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
 * GUPC shared memory interface.
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

/** GMEM shared memory size */
extern size_t gupcr_gmem_size;

/** GMEM get/put information tracking.
 *
 *  Track the information required to access global
 *  memory in a given direction (get/put) using non-blocking
 *  'get' and 'put' functions.
 */
typedef struct gupcr_gmem_xfer_info_struct
{
  /** Number of pending operations */
  size_t num_pending;
  /** Number of completed operations */
  size_t num_completed;
  /** Memory descriptor options */
  unsigned int md_options;
  /** Memory counting events handle */
  fab_cntr_t ct_handle;
  /** Memory region handle */
  fab_mr_t md;
  /** Volatile memory region handle */
  struct fid_mr md_volatile;
} gupcr_gmem_xfer_info_t;
/** GET/PUT information tracking pointer type */
typedef gupcr_gmem_xfer_info_t *gupcr_gmem_xfer_info_p;

/** GET transfer tracking */
extern gupcr_gmem_xfer_info_t gupcr_gmem_gets;
/** PUT transfer tracking */
extern gupcr_gmem_xfer_info_t gupcr_gmem_puts;

/** PUT "bounce buffer" bytes in use */
extern size_t gupcr_gmem_put_bb_used;

extern size_t gupcr_gmem_heap_base_offset;
extern size_t gupcr_gmem_heap_size;

extern void gupcr_gmem_init (void);
extern void gupcr_gmem_fini (void);

/** @} */
#endif /* gupcr_gmem.h */
