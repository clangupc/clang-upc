/*===-- gupcr_clock.c - UPC Runtime Support Library ----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_clock.c
 * GUPC Clock routines.
 */

/**
 * @addtogroup UPCCLOCK UPC Clock Functions
 * @{
 */

#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_utils.h"

static double gupcr_clock_rez;
static double gupcr_clock_base;

#if HAVE_CLOCK_GETTIME

#if defined (CLOCK_MONOTONIC_RAW) && defined (CLOCK_MONOTONIC)
#define GUPCR_CLOCK_ID CLOCK_MONOTONIC_RAW
/* On some RHEL/CentOS systems, the timer resolution returned for
   CLOCK_MONOTONIC_RAW is incorrect.  Use CLOCK_MONOTONIC instead.  */
#define GUPCR_CLOCK_REZ_ID CLOCK_MONOTONIC
#elif defined (CLOCK_MONOTONIC)
#define GUPCR_CLOCK_ID CLOCK_MONOTONIC
#define GUPCR_CLOCK_REZ_ID CLOCK_MONOTONIC
#else
#error missing system clock name definition.
#endif

double
gupcr_clock (void)
{
  struct timespec ts;
  double t;
  gupcr_syscall (clock_gettime, (GUPCR_CLOCK_ID, &ts));
  t = (double) ts.tv_sec + (double) ts.tv_nsec * 1.0e-9;
  t -= gupcr_clock_base;
  return t;
}

double
gupcr_clock_resolution (void)
{
  return gupcr_clock_rez;
}

void
gupcr_clock_init (void)
{
  struct timespec clock_rez;
  gupcr_syscall (clock_getres, (GUPCR_CLOCK_REZ_ID, &clock_rez));
  gupcr_assert (clock_rez.tv_sec == 0);
  gupcr_clock_rez = clock_rez.tv_nsec * 1.0e-9;
  gupcr_clock_base = gupcr_clock ();
}

#else /* Use gettimeofday().  */

double
gupcr_clock (void)
{
  struct timeval tv;
  double t;
  gupcr_syscall (gettimeofday, (&tv, NULL));
  t = (double) tv.tv_sec + (double) tv.tv_usec * 1.0e-6;
  t -= gupcr_clock_base;
  return t;
}

double
gupcr_clock_resolution (void)
{
  return gupcr_clock_rez;
}

void
gupcr_clock_init (void)
{
  int i;
  gupcr_clock_base = gupcr_clock ();
  gupcr_clock_rez = 1.0;
  for (i = 1; i <= 10; ++i)
    {
      double t1, t2, diff;
      t1 = gupcr_clock ();
      do
	{
	  t2 = gupcr_clock ();
	}
      while (t1 == t2);
      diff = t2 - t1;
      if (diff < gupcr_clock_rez)
	gupcr_clock_rez = diff;
    }
  /* Round the clock resolution to some common values
     if it is within range of one of them.  */
  if (gupcr_clock_rez > 0.9e-6 && gupcr_clock_rez < 1.1e-6)
    gupcr_clock_rez = 1.0e-6;
  else if (gupcr_clock_rez > 0.9e-3 && gupcr_clock_rez < 1.1e-3)
    gupcr_clock_rez = 1.0e-3;
  else if (gupcr_clock_rez > 0.9e-2 && gupcr_clock_rez < 1.1e-2)
    gupcr_clock_rez = 1.0e-2;
  else if (gupcr_clock_rez > 1.63e-2 && gupcr_clock_rez < 1.69e-2)
    gupcr_clock_rez = 1.0 / 60.0;
  else if (gupcr_clock_rez > 1.95e-2 && gupcr_clock_rez < 2.05e-2)
    gupcr_clock_rez = 1.0 / 50.0;
}

#endif
/** @} */
