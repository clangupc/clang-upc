/*===-- gupcr_nb_sup.h - UPC Runtime Support Library ---------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_nb_sup.h
 * GUPC non-blocking transfer implementation support routines.
 *
 * @addtogroup NON-BLOCKING GUPCR Non-Blocking Transfer Support Functions
 * @{
 */

#ifndef _GUPCR_NB_SUP_H_
#define _GUPCR_NB_SUP_H_ 1

/** Maximum number of outstanding non-blocking transfers */
#define GUPCR_NB_MAX_OUTSTANDING 128

extern void gupcr_nb_put (size_t, size_t, const void *,
			  size_t, unsigned long *);
extern void gupcr_nb_get (size_t, size_t, char *, size_t,
			  unsigned long *);
extern int gupcr_nb_completed (unsigned long);
extern void gupcr_sync (unsigned long);
extern int gupcr_nbi_outstanding (void);
extern void gupcr_synci (void);
extern void gupcr_nb_init (void);
extern void gupcr_nb_fini (void);

#endif /* gupcr_nb_sup.h */

/** }@ */
