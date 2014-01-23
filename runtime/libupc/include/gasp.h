/*===-- gasp.h - UPC Runtime Support Library -----------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GASP_H_
#define _GASP_H_

#include <stdarg.h>

/* See GASP Specification (version 1.5)
   http://gasp.hcs.ufl.edu/gasp-1.5-61606.pdf  */

#define GASP_VERSION 20060914

typedef enum
{
  GASP_MODEL_UPC,
  GASP_MODEL_TITANIUM,
  GASP_MODEL_CAF,
  GASP_MODEL_MPI,
  GASP_MODEL_SHMEM
} gasp_model_t;

typedef enum
{
  GASP_START,
  GASP_END,
  GASP_ATOMIC,
} gasp_evttype_t;

struct _gasp_context_S;
typedef struct _gasp_context_S *gasp_context_t;

gasp_context_t gasp_init (gasp_model_t srcmodel, int *argc, char ***argv);

void gasp_event_notify (gasp_context_t context, unsigned int evttag,
			gasp_evttype_t evttype, const char *filename,
			int linenum, int colnum, ...);

void gasp_event_notifyVA (gasp_context_t context, unsigned int evttag,
			  gasp_evttype_t evttype, const char *filename,
			  int linenum, int colnum, va_list varargs);

int gasp_control (gasp_context_t context, int on);

unsigned int gasp_create_event (gasp_context_t context,
				const char *name, const char *desc);

#endif /* _GASP_H_ */
