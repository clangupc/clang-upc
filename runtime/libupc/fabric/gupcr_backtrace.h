/*===-- gupcr_backtrace.h - UPC Runtime Support Library ------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef GUPCR_BACKTRACE_H_
#define GUPCR_BACKTRACE_H_

/* Environment variables. */
/** Enable/Disable backtrace env variable. */
#define GUPCR_BACKTRACE_ENV "UPC_BACKTRACE"
/** Enable/Disable STAT backtrace env variable. */
#define GUPCR_BACKTRACE_FILE_ENV "UPC_BACKTRACEFILE"
/** GDB command for backtrace env variable. */
#define GUPCR_BACKTRACE_GDB_ENV "UPC_BACKTRACE_GDB"

/* Interfaces. */
extern void gupcr_backtrace (void);
extern void gupcr_fatal_backtrace (void);
extern void gupcr_backtrace_init (const char *execname);
extern void gupcr_backtrace_restore_handlers (void);

#endif /* gupc_backtrace.h */
