/*===-- pupc.h - UPC Runtime Support Library -----------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#ifndef _PUPC_H_
#define _PUPC_H_

/* See GASP Specification (version 1.5)
   http://gasp.hcs.ufl.edu/gasp-1.5-61606.pdf.  */

#if __UPC_PUPC__
extern int pupc_control (int on);
extern unsigned int pupc_create_event (const char *name, const char *desc);
#else
#define pupc_control(on) 0
#define pupc_create_event(name, desc) 0
#endif

#if __UPC_PUPC__ && __UPC_PUPC_INST__
extern void pupc_event_startg
  (unsigned int evttag, const char *file, int line, ...);
extern void pupc_event_endg
  (unsigned int evttag, const char *file, int line, ...);
extern void pupc_event_atomicg
  (unsigned int evttag, const char *file, int line, ...);
#define pupc_event_start(evttag, args...) \
		pupc_event_startg (evttag, __FILE__, __LINE__, args)
#define pupc_event_end(evttag, args...) \
		pupc_event_endg (evttag, __FILE__, __LINE__, args)
#define pupc_event_atomic(evttag, args...) \
		pupc_event_atomicg (evttag, __FILE__, __LINE__, args)
#else
#define pupc_event_start(evttag, args...)
#define pupc_event_end(evttag, args...)
#define pupc_event_atomic(evttag, args...)
#endif

#ifndef pupc_event_start
/* These prototypes won't be compiled, because the
   macro definitions above will over-ride them.
   The prototypes are here for documentation purposes only.  */
extern void pupc_event_start (unsigned int evttag, ...);
extern void pupc_event_end (unsigned int evttag, ...);
extern void pupc_event_atomic (unsigned int evttag, ...);
#endif

#endif /* _PUPC_H_ */
