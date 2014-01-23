/*===-- upc_mem.c - UPC Runtime Support Library --------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include "upc_config.h"
#include "upc_sysdep.h"
#include "upc_defs.h"
#include "upc_sup.h"
#include "upc_access.h"
#include "upc_mem.h"

void
upc_memcpy (upc_shared_ptr_t dest, upc_shared_ptr_t src, size_t n)
{
  __upc_memcpy (dest, src, n);
}

void
upc_memget (void *dest, upc_shared_ptr_t src, size_t n)
{
  __upc_memget (dest, src, n);
}

void
upc_memput (upc_shared_ptr_t dest, const void *src, size_t n)
{
  __upc_memput (dest, src, n);
}

void
upc_memset (upc_shared_ptr_t dest, int c, size_t n)
{
  __upc_memset (dest, c, n);
}
