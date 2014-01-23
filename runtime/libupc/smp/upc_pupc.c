/*===-- upc_pupc.c - UPC Runtime Support Library -------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include "gasp.h"
#include "upc_config.h"
#include "upc_sysdep.h"
#include "upc_defs.h"
#include "upc_pupc.h"

static GUPCR_THREAD_LOCAL gasp_context_t __upc_gasp_ctx;

int
pupc_control (int on)
{
  return gasp_control (__upc_gasp_ctx, on);
}

unsigned int
pupc_create_event (const char *name, const char *desc)
{
  return gasp_create_event (__upc_gasp_ctx, name, desc);
}

void
pupc_event_start (unsigned int evttag, ...)
{
  va_list argptr;
  va_start (argptr, evttag);
  gasp_event_notifyVA (__upc_gasp_ctx, evttag, GASP_START, NULL, 0, 0,
		       argptr);
  va_end (argptr);
}

void
pupc_event_end (unsigned int evttag, ...)
{
  va_list argptr;
  va_start (argptr, evttag);
  gasp_event_notifyVA (__upc_gasp_ctx, evttag, GASP_END, NULL, 0, 0, argptr);
  va_end (argptr);
}

void
pupc_event_atomic (unsigned int evttag, ...)
{
  va_list argptr;
  va_start (argptr, evttag);
  gasp_event_notifyVA (__upc_gasp_ctx, evttag, GASP_ATOMIC, NULL, 0, 0,
		       argptr);
  va_end (argptr);
}

void
pupc_event_startg (unsigned int evttag, const char *filename, int linenum, ...)
{
  va_list argptr;
  va_start (argptr, linenum);
  gasp_event_notifyVA (__upc_gasp_ctx, evttag, GASP_START, filename, linenum, 0,
		       argptr);
  va_end (argptr);
}

void
pupc_event_endg (unsigned int evttag, const char *filename, int linenum, ...)
{
  va_list argptr;
  va_start (argptr, linenum);
  gasp_event_notifyVA (__upc_gasp_ctx, evttag, GASP_END, filename, linenum, 0,
		       argptr);
  va_end (argptr);
}

void
pupc_event_atomicg (unsigned int evttag, const char *filename, int linenum, ...)
{
  va_list argptr;
  va_start (argptr, linenum);
  gasp_event_notifyVA (__upc_gasp_ctx, evttag, GASP_ATOMIC,
		       filename, linenum, 0, argptr);
  va_end (argptr);
}

void
__upc_pupc_init (int *argc, char ***argv)
{
  __upc_gasp_ctx =  gasp_init (GASP_MODEL_UPC, argc, argv);
}
