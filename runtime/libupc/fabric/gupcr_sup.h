/*===-- gupcr_sup.h - UPC Runtime Support Library ------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_sup.h
 * GUPC Runtime prototypes to UPC specific functions.
 */

#ifndef _GUPCR_SUP_H_
#define _GUPCR_SUP_H_

/* Internal runtime routines and external symbols.  */

//begin lib_runtime_api

extern void *__cvtaddr (upc_shared_ptr_t);
extern void *__getaddr (upc_shared_ptr_t);
extern void __upc_barrier (int barrier_id);
extern void __upc_notify (int barrier_id);
extern void __upc_wait (int barrier_id);
extern void __upc_exit (int status) __attribute__ ((__noreturn__));

//end lib_runtime_api

#endif /* gupcr_sup.h */
