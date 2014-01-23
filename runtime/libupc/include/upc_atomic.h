/*===-- upc_atomic.h - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#ifndef _UPC_ATOMIC_H_
#define _UPC_ATOMIC_H_

#include "upc_types.h"

/* Atomic operations not defined in <upc_types.h>.  */
#define  UPC_GET    (1UL<<9)
#define  UPC_SET    (1UL<<10)
#define  UPC_CSWAP  (1UL<<11)
#define  UPC_SUB    (1UL<<12)
#define  UPC_INC    (1UL<<13)
#define  UPC_DEC    (1UL<<14)

#define  UPC_GET_OP    9
#define  UPC_SET_OP    10
#define  UPC_CSWAP_OP  11
#define  UPC_SUB_OP    12
#define  UPC_INC_OP    13
#define  UPC_DEC_OP    14

/* Preferred mode of optimization of a domain.  */
typedef int upc_atomichint_t;
/* Preferred mode of optimization values.  */
#define UPC_ATOMIC_HINT_DEFAULT 0
#define UPC_ATOMIC_HINT_LATENCY 1
#define UPC_ATOMIC_HINT_THROUGHPUT 2

/* Atomics domain allocator (collective function).  */
upc_atomicdomain_t *upc_all_atomicdomain_alloc (upc_type_t type,
						upc_op_t ops,
						upc_atomichint_t hints);

/* Atomics domain release (collective function).  */
void upc_all_atomicdomain_free (upc_atomicdomain_t * ptr);

/* Atomics strict operation.  */
void upc_atomic_strict (upc_atomicdomain_t * domain,
			void *restrict fetch_ptr, upc_op_t op,
			shared void *restrict target,
			const void *restrict operand1,
			const void *restrict operand2);

/* Atomics relaxed operation.  */
void upc_atomic_relaxed (upc_atomicdomain_t * domain,
			 void *restrict fetch_ptr, upc_op_t op,
			 shared void *restrict target,
			 const void *restrict operand1,
			 const void *restrict operand2);

/* Atomics query function for expected performance.  */
int upc_atomic_query (upc_type_t type, upc_op_t ops, shared void *addr);
/* Expected performance return value.  */
#define UPC_ATOMIC_PERFORMANCE_NOT_FAST 0
#define UPC_ATOMIC_PERFORMANCE_FAST 1

#endif /* !_UPC_ATOMIC_H_ */
