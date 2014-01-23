/*===-- upc_pupc.h - UPC Runtime Support Library -------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _UPC_PUPC_H_
#define _UPC_PUPC_H_

/* See GASP Specification (version 1.5)
   http://gasp.hcs.ufl.edu/gasp-1.5-61606.pdf  */

extern int pupc_control (int on);
extern unsigned int pupc_create_event (const char *name, const char *desc);

extern void pupc_event_start (unsigned int evttag, ...);
extern void pupc_event_end (unsigned int evttag, ...);
extern void pupc_event_atomic (unsigned int evttag, ...);

extern void pupc_event_startg (unsigned int evttag, const char *file, int line, ...);
extern void pupc_event_endg (unsigned int evttag, const char *file, int line, ...);
extern void pupc_event_atomicg (unsigned int evttag, const char *file, int line, ...);

extern void __upc_pupc_init (int *, char ***);

/* The "##__VAR_ARGS__" syntax below, is required to support an empty optional argument
   see: http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html  */
#define p_start(evttag, ...)  pupc_event_startg (evttag, filename, linenum, ##__VA_ARGS__)
#define p_end(evttag, ...)    pupc_event_endg (evttag, filename, linenum, ##__VA_ARGS__)
#define p_atomic(evttag, ...) pupc_event_atomicg (evttag, filename, linenum, ##__VA_ARGS__)

#define p_startx(evttag, ...)  pupc_event_startg (evttag, NULL, 0, ##__VA_ARGS__)
#define p_endx(evttag, ...)    pupc_event_endg (evttag, NULL, 0, ##__VA_ARGS__)
#define p_atomicx(evttag, ...) pupc_event_atomicg (evttag, NULL, 0, ##__VA_ARGS__)

#endif /* _UPC_PUPC_H_ */
