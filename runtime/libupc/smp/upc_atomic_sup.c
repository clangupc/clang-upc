/*===-- upc_atomic_sup.c - UPC Atomic Library Support --------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*===---------------------------------------------------------------------===*/

#include "upc_config.h"
#include "upc_sysdep.h"
#include "upc_defs.h"
#include "upc_sup.h"

void
__upc_atomic_lock ()
{
  upc_info_p u = __upc_info;
  if (u) __upc_acquire_lock (&u->lock);
  GUPCR_FENCE ();
}

void
__upc_atomic_release ()
{
  upc_info_p u = __upc_info;
  GUPCR_READ_FENCE ();
  if (u) __upc_release_lock (&u->lock);
}
