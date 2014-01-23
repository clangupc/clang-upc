/*===-- upc_types.h - UPC Runtime Support Library ------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#ifndef _UPC_TYPES_H_
#define _UPC_TYPES_H_


typedef int upc_type_t;

#define UPC_CHAR        1
#define UPC_UCHAR       2
#define UPC_SHORT       3
#define UPC_USHORT      4
#define UPC_INT         5
#define UPC_UINT        6
#define UPC_LONG        7
#define UPC_ULONG       8
#define UPC_LLONG       9
#define UPC_ULLONG      10
#define UPC_INT8        11
#define UPC_UINT8       12
#define UPC_INT16       13
#define UPC_UINT16      14
#define UPC_INT32       15
#define UPC_UINT32      16
#define UPC_INT64       17
#define UPC_UINT64      18
#define UPC_FLOAT       19
#define UPC_DOUBLE      20
#define UPC_LDOUBLE     21
#define UPC_PTS         22

/* Flag type for synchronization semantics
   (and potentially other uses).  */

typedef int upc_flag_t;

/* Synchronization flags.  */

#define UPC_IN_ALLSYNC       (1<<0)
#define UPC_IN_MYSYNC        (1<<1)
#define UPC_IN_NOSYNC        (1<<2)
#define UPC_OUT_ALLSYNC      (1<<3)
#define UPC_OUT_MYSYNC       (1<<4)
#define UPC_OUT_NOSYNC       (1<<5)

/* Operation type for upc_all_reduceT() and upc_all_prefix_reduceT().  */

typedef unsigned long upc_op_t;

#define UPC_ADD         (1UL<<0)
#define UPC_MULT        (1UL<<1)
#define UPC_AND         (1UL<<2)
#define UPC_OR          (1UL<<3)
#define UPC_XOR         (1UL<<4)
#define UPC_LOGAND      (1UL<<5)
#define UPC_LOGOR       (1UL<<6)
#define UPC_MIN         (1UL<<7)
#define UPC_MAX         (1UL<<8)

#define UPC_ADD_OP         0
#define UPC_MULT_OP        1
#define UPC_AND_OP         2
#define UPC_OR_OP          3
#define UPC_XOR_OP         4
#define UPC_LOGAND_OP      5
#define UPC_LOGOR_OP       6
#define UPC_MIN_OP         7
#define UPC_MAX_OP         8

#define UPC_FIRST_OP       UPC_ADD_OP
#define UPC_LAST_OP        UPC_MAX_OP

#endif /* _UPC_TYPES_H_ */
