/*===-- upc_gasp.c - UPC Runtime Support Library -------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include "upc_config.h"
#include "upc_sysdep.h"
#include "upc_defs.h"
#include "gasp.h"

/* Since libgupc contains references to these functions, we provide dummy
   implementations to prevent linker warnings when GASP support has been
   compiled into GNU UPC, but the user compiles their app regularly.
   We define these as weak symbols so tools can override them
   appropriately.  */

#pragma weak gasp_init
#pragma weak gasp_event_notify
#pragma weak gasp_event_notifyVA
#pragma weak gasp_control
#pragma weak gasp_create_event

gasp_context_t
gasp_init (gasp_model_t ARG_UNUSED (srcmodel),
	   int *ARG_UNUSED (argc), char ***ARG_UNUSED (argv))
{
  return 0;
}

void
gasp_event_notify (gasp_context_t ARG_UNUSED (context),
		   unsigned int ARG_UNUSED (evttag),
		   gasp_evttype_t ARG_UNUSED (evttype),
		   const char *ARG_UNUSED (filename),
		   int ARG_UNUSED (linenum), int ARG_UNUSED (colnum), ...)
{
}

void
gasp_event_notifyVA (gasp_context_t ARG_UNUSED (context),
		     unsigned int ARG_UNUSED (evttag),
		     gasp_evttype_t ARG_UNUSED (evttype),
		     const char *ARG_UNUSED (filename),
		     int ARG_UNUSED (linenum),
		     int ARG_UNUSED (colnum), va_list ARG_UNUSED (varargs))
{
}

int
gasp_control (gasp_context_t ARG_UNUSED (context), int ARG_UNUSED (on))
{
  return 0;
}

unsigned int
gasp_create_event (gasp_context_t ARG_UNUSED (context),
		   const char *ARG_UNUSED (name),
		   const char *ARG_UNUSED (desc))
{
  return 0;
}
