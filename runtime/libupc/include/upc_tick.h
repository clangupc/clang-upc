/*===-- upc_tick.h - UPC Runtime Support Library -------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#ifndef _UPC_TICK_H_
#define _UPC_TICK_H_ 1

/* Required, for uint64_t.  */
#include <stdint.h>

typedef uint64_t upc_tick_t;

#define     UPC_TICK_MIN 0ULL
#define     UPC_TICK_MAX 0xffffffffffffffffULL

extern upc_tick_t upc_ticks_now (void);
extern uint64_t upc_ticks_to_ns (upc_tick_t ticks);

#endif /* _UPC_TICK_H_ */
