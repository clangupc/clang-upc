/*===-- gupcr_broadcast.h - UPC Runtime Support Library ------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _GUPCR_BROADCAST_H_
#define _GUPCR_BROADCAST_H_

/**
 * @file gupcr_broadcast.h
 * GUPC Portals4 broadcast implementation.
 */

/**
 * @addtogroup BROADCAST GUPCR Broadcast Functions
 * @{
 */

/** Maximum message size that can be sent via broadcast.  */
#define GUPCR_MAX_BROADCAST_SIZE 32

/** @} */

extern void gupcr_broadcast_get (void *value, size_t nbytes);
extern void gupcr_broadcast_put (void *value, size_t nbytes);
extern void gupcr_broadcast_init (void);
extern void gupcr_broadcast_fini (void);

#endif /* gupcr_broadcast.h */
