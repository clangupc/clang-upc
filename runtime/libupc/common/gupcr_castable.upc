/*===-- gupcr_castable.upc - UPC Runtime Support Library -----------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include <upc.h>
#include <upc_castable.h>
#include <stdio.h>
#include "gupcr_pts.h"
#include "gupcr_iface.h"
#include "gupcr_node.h"
#include "gupcr_utils.h"

void *
upc_cast (const shared void *ptr)
{
  const upc_shared_ptr_t sptr = GUPCR_PTS_TO_REP (ptr);
  void *local_ptr = NULL;
  if (!GUPCR_PTS_IS_NULL (sptr))
    {
      const size_t thread = GUPCR_PTS_THREAD (sptr);
      const int thread_as_int = (int) thread;
      if (thread_as_int >= THREADS)
	gupcr_fatal_error ("thread number %d in shared address "
	                   "is out of range", thread_as_int);
      if (GUPCR_GMEM_IS_LOCAL (thread))
	{
	  size_t offset = GUPCR_PTS_OFFSET (sptr);
	  local_ptr = GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
	}
    }
  return local_ptr;
}

upc_thread_info_t
upc_thread_info (size_t thread)
{
  const int thread_as_int = (int) thread;
  upc_thread_info_t cast_info = { 0, 0 };
  if (thread_as_int >= THREADS)
    gupcr_fatal_error ("thread number %d in shared address "
		       "is out of range", thread_as_int);
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      cast_info.guaranteedCastable = UPC_CASTABLE_ALL;
      cast_info.probablyCastable = UPC_CASTABLE_ALL;
    }
  return cast_info;
}
