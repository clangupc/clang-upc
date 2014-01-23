/*===-- upc_numa.h - UPC Runtime Support Library -------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _UPC_NUMA_H_
#define _UPC_NUMA_H_

extern int __upc_numa_supported (void);
extern int __upc_numa_init (const upc_info_p,
                            const char **err_msg);
extern int __upc_numa_allocate (const upc_info_p u, const int thread_id,
				int *sched_affinity, int *mem_affinity,
				const char **err_msg);
extern void __upc_numa_sched_set (const upc_info_p, const int);
extern void __upc_numa_memory_affinity_set (const upc_info_p, const int);
extern void __upc_numa_memory_region_affinity_set (const upc_info_p u,
						   const int thread_id,
						   const void *region,
						   const size_t size);

#endif /* !_UPC_NUMA_H */
