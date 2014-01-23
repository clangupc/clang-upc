/*===-- upc_numa_stub.c - UPC Runtime Support Library --------------------===
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
#include "upc_numa.h"

int
__upc_numa_supported (void)
{
  return 0;
}

int
__upc_numa_init (const upc_info_p ARG_UNUSED (u),
                 const char **ARG_UNUSED (err_msg))
{
  return 1;
}

int
__upc_numa_allocate (const upc_info_p ARG_UNUSED (u),
                     const int ARG_UNUSED (thread_id),
		     int *ARG_UNUSED (sched_affinity),
		     int *ARG_UNUSED (mem_affinity),
		     const char **err_msg)
{
  *err_msg = "UPC error: unable to schedule over nodes - NUMA not available.";
  return 0;
}

void
__upc_numa_sched_set (const upc_info_p ARG_UNUSED (u),
                      const int ARG_UNUSED (thread_id))
{
}

void
__upc_numa_memory_affinity_set (const upc_info_p ARG_UNUSED (u),
                                const int ARG_UNUSED (thread_id))
{
}

void
__upc_numa_memory_region_affinity_set (const upc_info_p ARG_UNUSED (u),
				       const int ARG_UNUSED (thread_id),
				       const void *ARG_UNUSED (region),
				       const size_t ARG_UNUSED (size))
{
}
