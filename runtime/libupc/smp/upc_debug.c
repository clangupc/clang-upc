/*===-- upc_debug.c - UPC Runtime Support Library ------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include "upc_debug.h"

/* MPIR Interface support for debugging.
   http://www.mpi-forum.org/docs/mpir-specification-10-11-2010.pdf

   MPIR_being_debugged is being set by the debugger to 1.
   As we support MPIR_partial_attach_ok (section 9.13), all
   threads are in the hold mode and continue to run only after the
   debugger continues the monitor thread from the MPRI_breakpoint().
   If debugger wants to attach to any of the threads, thread's gate
   must be lowered by the debugger.  */
   
MPIR_PROCDESC *MPIR_proctable = 0;
int MPIR_proctable_size = 0;
const char *MPIR_debug_abort_string = 0;
volatile int MPIR_debug_state;
volatile int MPIR_debug_gate = 1; /* Threads continue to run by default.  */
int MPIR_being_debugged;	  /* Set by the debugger.  */
int MPIR_partial_attach_ok;	  /* OK to attach to subset of threads.  */

/* Debugging breakpoint.
   Subroutine called by the starter process to notify the debugger
   that an MPIR event has occurred.  */
void
MPIR_Breakpoint (void)
{
}

/* Tell the debugger that this initial process is not to be
   included in the set of processes which form the UPC program.  */
void
MPIR_i_am_starter (void)
{
}

/* Tell the debugger that we're not really MPI after all.  */
void
MPIR_ignore_queues (void)
{
}

/* Tell the debugger to display "main" if we stop immediately
   after acquiring the processes at startup time.  */
void
MPIR_force_to_main (void)
{
}

