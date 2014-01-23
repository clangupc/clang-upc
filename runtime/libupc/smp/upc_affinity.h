/*===-- upc_affinity.h - UPC Runtime Support Library ---------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _UPC_AFFINITY_H_
#define _UPC_AFFINITY_H_

extern void __upc_affinity_cpu_avoid_free (const upc_cpu_avoid_p);
extern upc_cpu_avoid_p __upc_affinity_cpu_avoid_new (void);
extern void __upc_affinity_cpu_avoid_set (const int, const upc_cpu_avoid_p);
extern int __upc_affinity_init (const upc_info_p, const upc_cpu_avoid_p,
				const char **err_msg);
extern void __upc_affinity_set (const upc_info_p, const int);
extern int __upc_affinity_supported (void);

#endif /* !_UPC_AFFINITY_H_ */
