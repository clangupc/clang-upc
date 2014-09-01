/*===-- gupcr_shutdown.h - UPC Runtime Support Library -------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_SHUTDOWN_H_
#define _GUPCR_SHUTDOWN_H_

/**
 * @file gupcr_shutdown.h
 * GUPC Libfabric shutdown support.
 */

extern void gupcr_signal_exit (int status);
extern void gupcr_shutdown_init (void);
extern void gupcr_shutdown_fini (void);

#endif /* gupcr_shutdown.h */
