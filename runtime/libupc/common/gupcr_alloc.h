/*===-- gupcr_alloc.h - UPC Runtime Support Library ----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_alloc.h
 * GUPC UPC dynamic shared memory allocation.
 */

#ifndef _GUPCR_ALLOC_H_
#define _GUPCR_ALLOC_H_ 1

extern void gupcr_alloc_init (upc_shared_ptr_t, size_t);

#ifndef __UPC__

extern upc_shared_ptr_t upc_global_alloc (size_t, size_t);
extern upc_shared_ptr_t upc_all_alloc (size_t, size_t);
extern upc_shared_ptr_t upc_local_alloc (size_t, size_t);
extern upc_shared_ptr_t upc_alloc (size_t);
extern void upc_free (upc_shared_ptr_t);

#endif /* !__UPC__ */

#endif /* gupcr_alloc.h */
