/*===--------- gupcr_lock_iface.h - UPC Runtime Interfaces ---------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_LOCK_IFACE_H_
#define _GUPCR_LOCK_IFACE_H_

/**
 * @file gupcr_iface.h
 * GUPC lock runtime interface.
 */

/**
 * @addtogroup IFACE GUPCR Shared Memory Access
 * @{
 */

/* LOCK interfaces.  */
/* Heap allocation locks.  */
extern upc_lock_t *gupcr_heap_region_lock;
extern upc_lock_t *gupcr_global_heap_lock;
extern upc_lock_t *gupcr_local_heap_lock;

#endif /* gupcr_lock_iface.h */
/** @} */
