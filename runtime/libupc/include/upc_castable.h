/*===-- upc_castable.h - UPC Runtime Support Library ---------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#ifndef _UPC_CASTABLE_H_
#define _UPC_CASTABLE_H_

#if __UPC_CASTABLE__ != 1
#error Bad feature macro predefinition
#endif

#include <stddef.h>		/* size_t */

#define UPC_CASTABLE_ALL_ALLOC      (1<<0)
#define UPC_CASTABLE_GLOBAL_ALLOC   (1<<1)
#define UPC_CASTABLE_ALLOC          (1<<2)
#define UPC_CASTABLE_STATIC         (1<<3)

#define UPC_CASTABLE_ALL  (            \
           UPC_CASTABLE_ALL_ALLOC    | \
           UPC_CASTABLE_GLOBAL_ALLOC | \
           UPC_CASTABLE_ALLOC        | \
           UPC_CASTABLE_STATIC         \
         )

typedef struct _S_upc_thread_info
{
  int guaranteedCastable;
  int probablyCastable;
} upc_thread_info_t;


void *upc_cast (const shared void *);

upc_thread_info_t upc_thread_info (size_t);

#endif /* _UPC_CASTABLE_H_ */
