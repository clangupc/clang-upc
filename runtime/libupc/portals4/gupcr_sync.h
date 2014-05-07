/*===-- gupcr_sync.h - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_sync.h
 * GUPC Runtime memory read/write sync definitions.
 */

#ifndef _GUPCR_SYNC_H_
#define _GUPCR_SYNC_H_

//begin lib_mem_barrier

/*

The following table is excerpted from
"Implementing the UPC memory consistency model for
shared-memory architectures", Dan Bonachea et al.

CPU		Write fence		Read fence
--------------------------------------------------
Power/PowerPC	sync			isync
Alpha		wmb			mb
x86		lock; addl $0,0(%%esp)  none reqd.
Athlon/Opteron	mfence			none reqd.
Itanium		mf			none reqd.
SPARC		stbar			none reqd.
MIPS		sync			none reqd.
PA-RISC		SYNC			none reqd. */

#define GUPCR_MEM_BARRIER() \
	{ GUPCR_READ_MEM_BARRIER (); GUPCR_WRITE_MEM_BARRIER (); }

#if defined (PPC)
#define GUPCR_WRITE_MEM_BARRIER() asm __volatile__ ("sync":::"memory")
#define GUPCR_READ_MEM_BARRIER() asm __volatile__ ("isync":::"memory")
#elif defined (alpha)
#define GUPCR_WRITE_MEM_BARRIER() asm __volatile__ ("wmb":::"memory")
#define GUPCR_READ_MEM_BARRIER() asm __volatile__ ("mb":::"memory")
#elif defined (__x86_64__)
#define GUPCR_WRITE_MEM_BARRIER() asm __volatile__ ("mfence":::"memory")
#define GUPCR_READ_MEM_BARRIER() asm __volatile__ ("":::"memory")
#elif defined (__ia64__)
#define GUPCR_WRITE_MEM_BARRIER() asm __volatile__ ("mf":::"memory")
#define GUPCR_READ_MEM_BARRIER() asm __volatile__ ("":::"memory")
#elif defined (i386)
#define GUPCR_WRITE_MEM_BARRIER() asm __volatile__ ( \
                                "lock; addl $0,0(%%esp)":::"memory")
#define GUPCR_READ_MEM_BARRIER() asm __volatile__ ("":::"memory")
#elif defined (sparc)
#define GUPCR_WRITE_MEM_BARRIER() asm __volatile__ ("stbar":::"memory")
#define GUPCR_READ_MEM_BARRIER() asm __volatile__ ("":::"memory")
#elif defined (mips)
#define GUPCR_WRITE_MEM_BARRIER() asm __volatile__ ("sync":::"memory")
#define GUPCR_READ_MEM_BARRIER() asm __volatile__ ("":::"memory")
#elif defined (hppa)
#define GUPCR_WRITE_MEM_BARRIER() asm __volatile__ ("SYNC":::"memory")
#define GUPCR_READ_MEM_BARRIER() asm __volatile__ ("":::"memory")
#else
#error "No memory fence  operations provided for this cpu."
#endif
//end lib_mem_barrier

#endif /* gupcr_sync.h */
