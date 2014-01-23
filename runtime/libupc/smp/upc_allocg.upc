/*===-- upc_allocg.upc - UPC Runtime Support Library ---------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include <upc.h>
#include "gasp_upc.h"
#include "upc_pupc.h"

#ifndef NULL
#define NULL (void *)0
#endif

/* The filename of the location where a runtime
   error was detected.  This is set by the various
   debug-enabled ('g') UPC runtime library routines.  */
extern GUPCR_THREAD_LOCAL const char *__upc_err_filename;

/* The line number of the location where a runtime
   error was detected.  This is set by the various
   debug-enabled ('g') UPC runtime library routines.  */
extern GUPCR_THREAD_LOCAL unsigned int __upc_err_linenum;

#define GUPCR_SET_ERR_LOC() \
  do \
    { \
      __upc_err_filename = filename; \
      __upc_err_linenum  = linenum; \
    } while (0)

#define GUPCR_CLEAR_ERR_LOC() \
  do \
    { \
      __upc_err_filename = NULL; \
      __upc_err_linenum  = 0; \
    } while (0)

shared void *
upc_global_allocg (size_t nblocks, size_t nbytes, const char *filename,
		   int linenum)
{
  shared void *result;
  p_start (GASP_UPC_GLOBAL_ALLOC, nblocks, nbytes);
  GUPCR_SET_ERR_LOC();
  result = upc_global_alloc (nblocks, nbytes);
  GUPCR_CLEAR_ERR_LOC();
  p_end (GASP_UPC_GLOBAL_ALLOC, nblocks, nbytes, &result);
  return result;
}

shared void *
upc_all_allocg (size_t nblocks, size_t nbytes, const char *filename, int linenum)
{
  shared void *result;
  p_start (GASP_UPC_ALL_ALLOC, nblocks, nbytes);
  GUPCR_SET_ERR_LOC();
  result = upc_all_alloc (nblocks, nbytes);
  GUPCR_CLEAR_ERR_LOC();
  p_end (GASP_UPC_ALL_ALLOC, nblocks, nbytes, &result);
  return result;
}

shared void *
upc_allocg (size_t nbytes, const char *filename, int linenum)
{
  shared void *val;
  p_start (GASP_UPC_ALLOC, nbytes);
  GUPCR_SET_ERR_LOC();
  val = upc_alloc (nbytes);
  GUPCR_CLEAR_ERR_LOC();
  p_end (GASP_UPC_ALLOC, nbytes, &val);
  return val;
}

void
upc_freeg (shared void *ptr, const char *filename, int linenum)
{
  p_start (GASP_UPC_FREE, &ptr);
  GUPCR_SET_ERR_LOC();
  upc_free (ptr);
  GUPCR_CLEAR_ERR_LOC();
  p_end (GASP_UPC_FREE, &ptr);
}

void
upc_all_freeg (shared void *ptr, const char *filename, int linenum)
{
  p_start (GASP_UPC_FREE, &ptr);
  GUPCR_SET_ERR_LOC();
  upc_all_free (ptr);
  GUPCR_CLEAR_ERR_LOC();
  p_end (GASP_UPC_FREE, &ptr);
}
