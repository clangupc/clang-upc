/*===-- upc_mem.h - UPC Runtime Support Library --------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _UPC_MEM_H_
#define _UPC_MEM_H_

/* The following memory-to-memory operations have been
   factored into this file because they are needed both
   by upc_access.c and upc_mem.c  */

//begin lib_inline_mem_sup

__attribute__((__always_inline__))
static inline
void
__upc_memcpy (upc_shared_ptr_t dest, upc_shared_ptr_t src, size_t n)
{
  if (GUPCR_PTS_IS_NULL (src))
    __upc_fatal ("Invalid access via null shared pointer");
  if (GUPCR_PTS_IS_NULL (dest))
    __upc_fatal ("Invalid access via null shared pointer");
  for (;;)
    {
      char *srcp = (char *)__upc_sptr_to_addr (src);
      size_t s_offset  = GUPCR_PTS_OFFSET(src);
      size_t ps_offset = (s_offset & GUPCR_VM_OFFSET_MASK);
      size_t ns_copy = GUPCR_VM_PAGE_SIZE - ps_offset;
      char *destp = (char *)__upc_sptr_to_addr (dest);
      size_t d_offset  = GUPCR_PTS_OFFSET(dest);
      size_t pd_offset = (d_offset & GUPCR_VM_OFFSET_MASK);
      size_t nd_copy = GUPCR_VM_PAGE_SIZE - pd_offset;
      size_t n_copy = GUPCR_MIN (GUPCR_MIN (ns_copy, nd_copy), n);
      memcpy (destp, srcp, n_copy);
      n -= n_copy;
      if (!n)
        break;
      GUPCR_PTS_INCR_VADDR (src, n_copy);
      GUPCR_PTS_INCR_VADDR (dest, n_copy);
    }
}

__attribute__((__always_inline__))
static inline
void
__upc_memget (void *dest, upc_shared_ptr_t src, size_t n)
{
  if (!dest)
    __upc_fatal ("Invalid access via null shared pointer");
  if (GUPCR_PTS_IS_NULL (src))
    __upc_fatal ("Invalid access via null shared pointer");
  for (;;)
    {
      char *srcp = (char *)__upc_sptr_to_addr (src);
      size_t offset = GUPCR_PTS_OFFSET(src);
      size_t p_offset = (offset & GUPCR_VM_OFFSET_MASK);
      size_t n_copy = GUPCR_MIN (GUPCR_VM_PAGE_SIZE - p_offset, n);
      memcpy (dest, srcp, n_copy);
      n -= n_copy;
      if (!n)
        break;
      GUPCR_PTS_INCR_VADDR (src, n_copy);
      dest = (char *) dest + n_copy;
    }
}

__attribute__((__always_inline__))
static inline
void
__upc_memput (upc_shared_ptr_t dest, const void *src, size_t n)
{
  if (!src)
    __upc_fatal ("Invalid access via null shared pointer");
  if (GUPCR_PTS_IS_NULL (dest))
    __upc_fatal ("Invalid access via null shared pointer");
  for (;;)
    {
      char *destp = (char *)__upc_sptr_to_addr (dest);
      size_t offset = GUPCR_PTS_OFFSET(dest);
      size_t p_offset = (offset & GUPCR_VM_OFFSET_MASK);
      size_t n_copy = GUPCR_MIN (GUPCR_VM_PAGE_SIZE - p_offset, n);
      memcpy (destp, src, n_copy);
      n -= n_copy;
      if (!n)
        break;
      GUPCR_PTS_INCR_VADDR (dest, n_copy);
      src = (char *) src + n_copy;
    }
}

__attribute__((__always_inline__))
static inline
void
__upc_memset (upc_shared_ptr_t dest, int c, size_t n)
{
  if (GUPCR_PTS_IS_NULL (dest))
    __upc_fatal ("Invalid access via null shared pointer");
  for (;;)
    {
      char *destp = (char *)__upc_sptr_to_addr (dest);
      size_t offset = GUPCR_PTS_OFFSET(dest);
      size_t p_offset = (offset & GUPCR_VM_OFFSET_MASK);
      size_t n_set = GUPCR_MIN (GUPCR_VM_PAGE_SIZE - p_offset, n);
      memset (destp, c, n_set);
      n -= n_set;
      if (!n)
        break;
      GUPCR_PTS_INCR_VADDR (dest, n_set);
    }
}
//end lib_inline_mem_sup

#endif /* _UPC_MEM_H_ */
