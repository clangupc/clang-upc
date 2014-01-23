/*===-- upc_tick.c - UPC Runtime Support Library -------------------------===
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
#include "upc_lib.h"

#if HAVE_CLOCK_GETTIME
#ifdef CLOCK_MONOTONIC_RAW
/* System clock id passed to clock_gettime. CLOCK_MONOTONIC_RAW
   is preferred.  It has been available in the Linux kernel
   since version 2.6.28 */
#define SYS_RT_CLOCK_ID CLOCK_MONOTONIC_RAW
#else
#define SYS_RT_CLOCK_ID CLOCK_MONOTONIC
#endif

upc_tick_t
upc_ticks_now (void)
{
  struct timespec ts;
  upc_tick_t t;
  if (clock_gettime (SYS_RT_CLOCK_ID, &ts) != 0)
    {
      perror ("clock_gettime");
      abort ();
    }
  t = (upc_tick_t) ts.tv_sec * 1000000000LL + (upc_tick_t) ts.tv_nsec;
  return t;
}

#else /* !HAVE_CLOCK_GETTIME */

upc_tick_t
upc_ticks_now (void)
{
  struct timeval tv;
  upc_tick_t t;
  if (gettimeofday (&tv, NULL) != 0)
    {
      perror ("gettimeofday");
      abort ();
    }
  t = (upc_tick_t) tv.tv_sec * 1000000000LL + (upc_tick_t) tv.tv_usec * 1000;
  return t;
}

#endif

uint64_t
upc_ticks_to_ns (upc_tick_t ticks)
{
  return ticks;
}
