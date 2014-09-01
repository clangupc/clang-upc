/*===-- gupcr_node.h - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_NODE_H_
#define _GUPCR_NODE_H_ 1

/**
 * @file gupcr_node.h
 * GUPC Node Local Memory.
 */

/**
 * @addtogroup NODE GUPCR Node Local Memory
 * @{
 */

/** Node local memory file/object prefix.  */
#define GUPCR_LOCAL_NAME_PREFIX "upc-mem"

char *gupcr_node_local_alloc (size_t);
void gupcr_node_init (void);
void gupcr_node_fini (void);
char *gupcr_mem_local_map (int, size_t);
void gupcr_mem_local_unlink (void);
void gupcr_mem_local_init (void);
/** @} */

#endif /* gupcr_node.h */
