/*===-- upc_lock_sup.c - UPC Runtime Support Library ---------------------===
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

/* On some 32 bit targets 64 bit CSWAP is not available.  */
#if !defined (__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
/* Use OS spin lock to protect the critical section.  */
unsigned char
__sync_bool_compare_and_swap_8 (long long *addr, long long exp, long long val)
{
  unsigned char result = 0;
  upc_info_p u = __upc_info;
  __upc_acquire_lock (&u->lock);
  if (*addr == exp)
    {
      *addr = val;
      result = 1;
    }
  __upc_release_lock (&u->lock);
  return result;
}
#endif
 
