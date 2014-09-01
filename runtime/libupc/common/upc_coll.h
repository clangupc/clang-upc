/*===-- upc_coll.h - UPC Runtime Support Library -------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|* This file is derived from the UPC Collectives reference implementation
|* developed by Michigan Technological University.
|*
|* Copyright 2004, Michigan Technological University.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-MTU.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _UPC_COLL_H_
#define _UPC_COLL_H_ 1

/**
 * @file upc_coll.h
 * GUPC reduce collectives implementation.
 *
 * @addtogroup COLLECTIVES GUPCR Collectives Functions
 * @{
 */

extern int upc_coll_init_flag;
extern void upc_coll_init (void);

/**
 * Collective reduce storage area. It is used for the atomic
 * functions, reduce function storage, and signaling that upper part
 * of the reduced tree is ready for the reduced operation.
 */
struct gupcr_reduce_str
{
  int signal;				/** Signal that parent is ready */
  long double value[GUPCR_TREE_FANOUT];	/** Reduced values from children */
};
typedef shared struct gupcr_reduce_str *gupcr_reduce_str_t;
/** Allocated reduce storage space */
extern gupcr_reduce_str_t gupcr_reduce_storage;

/** @} */
#endif /* upc_coll.h */
