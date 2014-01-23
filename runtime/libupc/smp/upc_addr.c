/*===-- upc_addr.c - UPC Runtime Support Library -------------------------===
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
#include "upc_sup.h"

void *
__cvtaddr (upc_shared_ptr_t p)
{
  upc_info_p u = __upc_info;
  void *addr;
  if (!u)
    __upc_fatal ("UPC runtime not initialized");
  if (GUPCR_PTS_IS_NULL (p))
    return (void *) 0;
  addr = __upc_sptr_to_addr (p);
  return addr;
}

void *
__getaddr (upc_shared_ptr_t p)
{
  upc_info_p u = __upc_info;
  void *addr;
  if (!u)
    __upc_fatal ("UPC runtime not initialized");
  if (GUPCR_PTS_IS_NULL (p))
    return (void *) 0;
  if ((int)GUPCR_PTS_THREAD(p) != MYTHREAD)
    __upc_fatal ("Invalid conversion of shared address to local pointer;\nthread does not have affinity to shared address");
  addr = __upc_sptr_to_addr (p);
  return addr;
}

size_t
upc_threadof (upc_shared_ptr_t p)
{
  upc_info_p u = __upc_info;
  if (!u)
    __upc_fatal ("UPC runtime not initialized");
  if ((int)GUPCR_PTS_THREAD(p) >= THREADS)
    __upc_fatal ("Thread number in shared address is out of range");
  return GUPCR_PTS_THREAD (p);
}

size_t
upc_phaseof (upc_shared_ptr_t p)
{
  upc_info_p u = __upc_info;
  if (!u)
    __upc_fatal ("UPC runtime not initialized");
  if ((int)GUPCR_PTS_THREAD(p) >= THREADS)
    __upc_fatal ("Thread number in shared address is out of range");
  return GUPCR_PTS_PHASE (p);
}

upc_shared_ptr_t
upc_resetphase (upc_shared_ptr_t p)
{
  upc_shared_ptr_t result;
  result = p;
  GUPCR_PTS_SET_PHASE (result, 0);
  return result;
}

size_t
upc_addrfield (upc_shared_ptr_t p)
{
  upc_info_p u = __upc_info;
  if (!u)
    __upc_fatal ("UPC runtime not initialized");
  if ((int)GUPCR_PTS_THREAD(p) >= THREADS)
    __upc_fatal ("Thread number in shared address is out of range");
  return (size_t) GUPCR_PTS_VADDR (p);
}

size_t
upc_affinitysize (size_t totalsize, size_t nbytes, size_t threadid)
{ 
  size_t result;
  if (nbytes == 0 || totalsize == 0 || nbytes >= totalsize)
      result = (size_t) (threadid == 0 ? totalsize : 0);
  else
    {
      size_t const nblocks = (totalsize / nbytes);
      size_t const cutoff = (nblocks % THREADS);
      if (threadid < cutoff)
	result = (size_t) ((nblocks + THREADS - 1) / THREADS) * nbytes;
      else if (threadid > cutoff)
	result = (size_t) (nblocks / THREADS) * nbytes;
      else /* threadid == cutoff */
	result = (size_t) ((nblocks / THREADS) * nbytes)
			   + totalsize - nblocks * nbytes;
    }
  return result;
}
