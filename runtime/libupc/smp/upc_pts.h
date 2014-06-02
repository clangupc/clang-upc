/*===-- upc_pts.h - UPC Runtime Support Library --------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _UPC_PTS_H_
#define _UPC_PTS_H_ 1

//begin lib_pts_defs

/* UPC pointer representation */

#define GUPCR_PTS_VADDR_SHIFT   0
#define GUPCR_PTS_THREAD_SHIFT  GUPCR_PTS_VADDR_SIZE
#define GUPCR_PTS_PHASE_SHIFT   (GUPCR_PTS_THREAD_SHIFT + GUPCR_PTS_THREAD_SIZE)

#define GUPCR_PTS_TO_REP(V) *((upc_shared_ptr_t *)&(V)) 
#if GUPCR_TARGET64
#if (GUPCR_PTS_PHASE_SIZE+GUPCR_PTS_THREAD_SIZE+GUPCR_PTS_VADDR_SIZE) == 64
#define GUPCR_PTS_REP_T unsigned long
#else
#define GUPCR_PTS_REP_T u_intTI_t
#endif
#else
#define GUPCR_PTS_REP_T unsigned long long
#endif
#define GUPCR_ONE ((GUPCR_PTS_REP_T)1)
#define GUPCR_PTS_VADDR_MASK	((GUPCR_ONE << GUPCR_PTS_VADDR_SIZE) - GUPCR_ONE)
#define GUPCR_PTS_THREAD_MASK	((GUPCR_ONE << GUPCR_PTS_THREAD_SIZE) - GUPCR_ONE)
#define GUPCR_PTS_PHASE_MASK	((GUPCR_ONE << GUPCR_PTS_PHASE_SIZE) - GUPCR_ONE)

/* upc_dbg_shared_ptr_t is used by debugger to figure out
   shared pointer layout */
typedef struct shared_ptr_struct
  {
    unsigned int phase:GUPCR_PTS_PHASE_SIZE;
    unsigned int thread:GUPCR_PTS_THREAD_SIZE;
    unsigned long long vaddr:GUPCR_PTS_VADDR_SIZE;
  } upc_dbg_shared_ptr_t;

typedef GUPCR_PTS_REP_T upc_shared_ptr_t;
typedef upc_shared_ptr_t *upc_shared_ptr_p;

#define GUPCR_PTS_IS_NULL(P) !(P)
#define GUPCR_PTS_SET_NULL_SHARED(P) { (P) = 0; }

#define GUPCR_PTS_OFFSET(P) ((size_t)((P) & GUPCR_PTS_VADDR_MASK))
#define GUPCR_PTS_VADDR(P)  (void *)GUPCR_PTS_OFFSET(P)
#define GUPCR_PTS_THREAD(P) ((size_t)((P)>>GUPCR_PTS_THREAD_SHIFT & GUPCR_PTS_THREAD_MASK))
#define GUPCR_PTS_PHASE(P)  ((size_t)((P)>>GUPCR_PTS_PHASE_SHIFT & GUPCR_PTS_PHASE_MASK))

#define GUPCR_PTS_SET_VADDR(P,V) \
  (P) = (((P) & ~GUPCR_PTS_VADDR_MASK) | (GUPCR_PTS_REP_T)(V))
#define GUPCR_PTS_SET_THREAD(P,V) \
  (P) = (((P) & ~(GUPCR_PTS_THREAD_MASK << GUPCR_PTS_THREAD_SHIFT)) \
             | ((GUPCR_PTS_REP_T)(V) << GUPCR_PTS_THREAD_SHIFT))
#define GUPCR_PTS_SET_PHASE(P,V) \
  (P) = (((P) & ~(GUPCR_PTS_PHASE_MASK << GUPCR_PTS_PHASE_SHIFT)) \
             | ((GUPCR_PTS_REP_T)(V) << GUPCR_PTS_PHASE_SHIFT))
#define GUPCR_PTS_INCR_VADDR(P,V) (P) += (V)

//end lib_pts_defs

#endif /* !_UPC_PTS_H_ */
