/*===-- upc_affinity_stub.c - UPC Runtime Support Library ----------------===
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
#include "upc_affinity.h"

int
__upc_affinity_supported (void)
{
  return 0;
}

int
__upc_affinity_init (const upc_info_p ARG_UNUSED (u),
                     const upc_cpu_avoid_p ARG_UNUSED (avoid),
		     const char **ARG_UNUSED (err_msg))
{
  return 1;
}

upc_cpu_avoid_p
__upc_affinity_cpu_avoid_new (void)
{
  return NULL;
}

void
__upc_affinity_cpu_avoid_free (const upc_cpu_avoid_p ARG_UNUSED (avoid))
{
}

void
__upc_affinity_cpu_avoid_set (const int ARG_UNUSED (cpu),
                              const upc_cpu_avoid_p ARG_UNUSED (avoid))
{
}

void
__upc_affinity_set (const upc_info_p ARG_UNUSED (u),
                    const int ARG_UNUSED (thread_id))
{
}
