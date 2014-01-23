/*===-- upc_castable - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#include <upc.h>
#include <upc_castable.h>

void *
upc_cast (const shared void *ptr)
{
  const size_t thread = upc_threadof ((shared void *) ptr);
  void *local_ptr = NULL;
  if (thread == (size_t) MYTHREAD)
    {
      local_ptr = (void *) ptr;
    }
  return local_ptr;
}

upc_thread_info_t
upc_thread_info (size_t thread)
{
  upc_thread_info_t cast_info = { 0, 0 };
  if (thread == (size_t) MYTHREAD)
    {
      cast_info.guaranteedCastable = UPC_CASTABLE_ALL;
      cast_info.probablyCastable = UPC_CASTABLE_ALL;
    }
  return cast_info;
}
