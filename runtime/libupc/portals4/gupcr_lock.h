/*===-- gupcr_lock.h - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_LOCK_H_
#define _GUPCR_LOCK_H_

/**
 * @file gupcr_lock.h
 * GUPC Portals4 lock external definitions.
 */

/* Heap allocation locks.  */
extern upc_lock_t *gupcr_heap_region_lock;
extern upc_lock_t *gupcr_global_heap_lock;
extern upc_lock_t *gupcr_local_heap_lock;

#endif /* gupcr_lock.h */
