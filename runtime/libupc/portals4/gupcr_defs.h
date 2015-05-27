/*===-- gupcr_defs.h - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_defs.h
 * GUPC Runtime definitions
 */

#ifndef _GUPCR_DEFS_H_
#define _GUPCR_DEFS_H_

#include "gupcr_pts.h"

//begin lib_max_threads_def

/* Maximum number of THREADS supported in this implementation */
#define GUPCR_THREAD_SIZE 12
#define GUPCR_THREADS_MAX (1 << GUPCR_THREAD_SIZE)
//end lib_max_threads_def

#if GUPCR_PTS_PACKED_REP && (GUPCR_THREADS_SIZE > GUPCR_PTS_THREAD_SIZE)
#error GUPCR_THREADS_MAX exceeds the size of the packed sptr threads field.
#endif

//begin lib_omp_check
#if GUCPR_HAVE_CHECKS
extern void __upc_omp_check (void);
#define GUPCR_OMP_CHECK() __upc_omp_check()
#else
#define GUPCR_OMP_CHECK()
#endif
//end lib_omp_check

/* The filename of the location where a runtime
   error was detected.  This is set by the various
   debug-enabled ('g') UPC runtime library routines.  */
extern const char *gupcr_err_filename;

/* The line number of the location where a runtime
   error was detected.  This is set by the various
   debug-enabled ('g') UPC runtime library routines.  */
extern unsigned int gupcr_err_linenum;

#define GUPCR_SET_ERR_LOC() \
  do \
    { \
      gupcr_err_filename = filename; \
      gupcr_err_linenum  = linenum; \
    } while (0)

#define GUPCR_CLEAR_ERR_LOC() \
  do \
    { \
      gupcr_err_filename = NULL; \
      gupcr_err_linenum  = 0; \
    } while (0)

/* The base address of the UPC shared section.  */
extern char GUPCR_SHARED_SECTION_START[1];

/* The ending address (plus one) of the UPC shared section.  */
extern char GUPCR_SHARED_SECTION_END[1];

/* The base address of the UPC program information section.  */
extern char GUPCR_PGM_INFO_SECTION_START[1];

/* The ending address (plus one) of the UPC program information section.  */
extern char GUPCR_PGM_INFO_SECTION_END[1];

#ifndef __UPC__
/* The value of THREADS when defined at run time.  */
extern int THREADS;

/* Current thread id.  */
extern int MYTHREAD;
#endif /* !__UPC__ */

/* OK to call finalize routines.  */
extern int gupcr_finalize_ok;

/* Prototype for the main finalize routine.  */
extern void gupcr_fini (void);

#endif /* gupcr_defs.h */
