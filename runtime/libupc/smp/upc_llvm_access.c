/*===-- upc_llvm_access.c - UPC Runtime Support Library ------------------===
|*
|*                  The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2015, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include "upc_config.h"
#include "upc_sysdep.h"
#include "upc_defs.h"
#include "upc_access.h"
#include "upc_sync.h"
#include "upc_sup.h"
#include "upc_mem.h"

//begin lib_inline_access


/* LLVM access routines.  */
/* To speed things up, the last two unique (page, thread)
   lookups are cached.  Caller must validate the pointer
   'p' (check for NULL, etc.) before calling this routine. */
//inline
void *
__upc_rptr_to_addr (int thread, size_t vaddr)
{
  extern GUPCR_THREAD_LOCAL unsigned long __upc_page1_ref, __upc_page2_ref;
  extern GUPCR_THREAD_LOCAL void *__upc_page1_base, *__upc_page2_base;
  void *addr;
  size_t p_offset;
  upc_page_num_t pn;
  unsigned long this_page;
  p_offset = vaddr & GUPCR_VM_OFFSET_MASK;
  pn = (vaddr >> GUPCR_VM_OFFSET_BITS) & GUPCR_VM_PAGE_MASK;
  this_page = (pn << GUPCR_THREAD_SIZE) | thread;
  if (this_page == __upc_page1_ref)
    addr = (char *) __upc_page1_base + p_offset;
  else if (this_page == __upc_page2_ref)
    addr = (char *) __upc_page2_base + p_offset;
  else
    addr = __upc_vm_map_remote_offset (thread, vaddr);
  return addr;
}

//inline
static void
__remote_get (long sthread, long saddr, void *dest, size_t n)
{
  char *srcp = (char *) __upc_rptr_to_addr (sthread, saddr);
  for (;;)
    {
      size_t p_offset = (saddr & GUPCR_VM_OFFSET_MASK);
      size_t n_copy = GUPCR_MIN (GUPCR_VM_PAGE_SIZE - p_offset, n);
      memcpy (dest, srcp, n_copy);
      n -= n_copy;
      if (!n)
	break;
      saddr += n_copy;
      dest = (char *) dest + n_copy;
    }
}

//inline
static void
__remote_put (const void *src, long dthread, long daddr, size_t n)
{
  char *destp = (char *) __upc_rptr_to_addr (dthread, daddr);
  for (;;)
    {
      size_t p_offset = (daddr & GUPCR_VM_OFFSET_MASK);
      size_t n_copy = GUPCR_MIN (GUPCR_VM_PAGE_SIZE - p_offset, n);
      memcpy (destp, src, n_copy);
      n -= n_copy;
      if (!n)
	break;
      daddr += n_copy;
      src = (char *) src + n_copy;
    }
}

//inline
u_intQI_t
__getqi3 (long sthread, long saddr)
{
  u_intQI_t result;
  const u_intQI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intQI_t *) __upc_rptr_to_addr (sthread, saddr);
  result = *addr;
  return result;
}

//inline
u_intHI_t
__gethi3 (long sthread, long saddr)
{
  u_intHI_t result;
  const u_intHI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intHI_t *) __upc_rptr_to_addr (sthread, saddr);
  result = *addr;
  return result;
}

//inline
u_intSI_t
__getsi3 (long sthread, long saddr)
{
  u_intSI_t result;
  const u_intSI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intSI_t *) __upc_rptr_to_addr (sthread, saddr);
  result = *addr;
  return result;
}

//inline
u_intDI_t
__getdi3 (long sthread, long saddr)
{
  u_intDI_t result;
  const u_intDI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intDI_t *) __upc_rptr_to_addr (sthread, saddr);
  result = *addr;
  return result;
}

#if GUPCR_TARGET64
//inline
u_intTI_t
__getti3 (long sthread, long saddr)
{
  u_intTI_t result;
  const u_intTI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intTI_t *) __upc_rptr_to_addr (sthread, saddr);
  result = *addr;
  return result;
}
#endif /* GUPCR_TARGET64 */
//inline
float
__getsf3 (long sthread, long saddr)
{
  float result;
  const float *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (float *) __upc_rptr_to_addr (sthread, saddr);
  result = *addr;
  return result;
}

//inline
double
__getdf3 (long sthread, long saddr)
{
  double result;
  const double *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (double *) __upc_rptr_to_addr (sthread, saddr);
  result = *addr;
  return result;
}

//inline
long double
__gettf3 (long sthread, long saddr)
{
  long double result;
  const long double *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (long double *) __upc_rptr_to_addr (sthread, saddr);
  result = *addr;
  return result;
}

//inline
long double
__getxf3 (long sthread, long saddr)
{
  long double result;
  const long double *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (long double *) __upc_rptr_to_addr (sthread, saddr);
  result = *addr;
  return result;
}

//inline
void
__getblk4 (long sthread, long saddr, void *dest, size_t len)
{
  GUPCR_OMP_CHECK ();
  if (!dest)
    __upc_fatal ("Invalid access via null local pointer");
  if (!saddr)
    __upc_fatal ("Invalid access via null remote pointer");
  __remote_get (sthread, saddr, dest, len);
}

//inline
void
__putqi3 (long dthread, long daddr, u_intQI_t v)
{
  u_intQI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intQI_t *) __upc_rptr_to_addr (dthread, daddr);
  *addr = v;
}

//inline
void
__puthi3 (long dthread, long daddr, u_intHI_t v)
{
  u_intHI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intHI_t *) __upc_rptr_to_addr (dthread, daddr);
  *addr = v;
}

//inline
void
__putsi3 (long dthread, long daddr, u_intSI_t v)
{
  u_intSI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intSI_t *) __upc_rptr_to_addr (dthread, daddr);
  *addr = v;
}

//inline
void
__putdi3 (long dthread, long daddr, u_intDI_t v)
{
  u_intDI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intDI_t *) __upc_rptr_to_addr (dthread, daddr);
  *addr = v;
}

#if GUPCR_TARGET64
//inline
void
__putti3 (long dthread, long daddr, u_intTI_t v)
{
  u_intTI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intTI_t *) __upc_rptr_to_addr (dthread, daddr);
  *addr = v;
}
#endif /* GUPCR_TARGET64 */
//inline
void
__putsf3 (long dthread, long daddr, float v)
{
  float *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (float *) __upc_rptr_to_addr (dthread, daddr);
  *addr = v;
}

//inline
void
__putdf3 (long dthread, long daddr, double v)
{
  double *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (double *) __upc_rptr_to_addr (dthread, daddr);
  *addr = v;
}

//inline
void
__puttf3 (long dthread, long daddr, long double v)
{
  long double *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (long double *) __upc_rptr_to_addr (dthread, daddr);
  *addr = v;
}

//inline
void
__putxf3 (long dthread, long daddr, long double v)
{
  long double *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (long double *) __upc_rptr_to_addr (dthread, daddr);
  *addr = v;
}

//inline
void
__putblk4 (const void *src, long dthread, long daddr, size_t n)
{
  GUPCR_OMP_CHECK ();
  __remote_put (src, dthread, daddr, n);
}

/* Strict memory accesses. */
//inline
u_intQI_t
__getsqi3 (long sthread, long saddr)
{
  u_intQI_t result;
  const u_intQI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intQI_t *) __upc_rptr_to_addr (sthread, saddr);
  GUPCR_FENCE ();
  result = *addr;
  GUPCR_READ_FENCE ();
  return result;
}

//inline
u_intHI_t
__getshi3 (long sthread, long saddr)
{
  u_intHI_t result;
  const u_intHI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intHI_t *) __upc_rptr_to_addr (sthread, saddr);
  GUPCR_FENCE ();
  result = *addr;
  GUPCR_READ_FENCE ();
  return result;
}

//inline
u_intSI_t
__getssi3 (long sthread, long saddr)
{
  u_intSI_t result;
  const u_intSI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intSI_t *) __upc_rptr_to_addr (sthread, saddr);
  GUPCR_FENCE ();
  result = *addr;
  GUPCR_READ_FENCE ();
  return result;
}

//inline
u_intDI_t
__getsdi3 (long sthread, long saddr)
{
  u_intDI_t result;
  const u_intDI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intDI_t *) __upc_rptr_to_addr (sthread, saddr);
  GUPCR_FENCE ();
  result = *addr;
  GUPCR_READ_FENCE ();
  return result;
}

#if GUPCR_TARGET64
//inline
u_intTI_t
__getsti3 (long sthread, long saddr)
{
  u_intTI_t result;
  const u_intTI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intTI_t *) __upc_rptr_to_addr (sthread, saddr);
  GUPCR_FENCE ();
  result = *addr;
  GUPCR_READ_FENCE ();
  return result;
}
#endif /* GUPCR_TARGET64 */
//inline
float
__getssf3 (long sthread, long saddr)
{
  float result;
  const float *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (float *) __upc_rptr_to_addr (sthread, saddr);
  GUPCR_FENCE ();
  result = *addr;
  GUPCR_READ_FENCE ();
  return result;
}

//inline
double
__getsdf3 (long sthread, long saddr)
{
  double result;
  const double *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (double *) __upc_rptr_to_addr (sthread, saddr);
  GUPCR_FENCE ();
  result = *addr;
  GUPCR_READ_FENCE ();
  return result;
}

//inline
long double
__getstf3 (long sthread, long saddr)
{
  long double result;
  const long double *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (long double *) __upc_rptr_to_addr (sthread, saddr);
  GUPCR_FENCE ();
  result = *addr;
  GUPCR_READ_FENCE ();
  return result;
}

//inline
long double
__getsxf3 (long sthread, long saddr)
{
  long double result;
  const long double *addr;
  GUPCR_OMP_CHECK ();
  if (!saddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (long double *) __upc_rptr_to_addr (sthread, saddr);
  GUPCR_FENCE ();
  result = *addr;
  GUPCR_READ_FENCE ();
  return result;
}

//inline
void
__getsblk4 (long sthread, long saddr, void *dest, size_t len)
{
  GUPCR_OMP_CHECK ();
  GUPCR_FENCE ();
  if (!dest)
    __upc_fatal ("Invalid access via null local pointer");
  if (!saddr)
    __upc_fatal ("Invalid access via null remote pointer");
  __remote_get (sthread, saddr, dest, len);
  GUPCR_READ_FENCE ();
}

//inline
void
__putsqi3 (long dthread, long daddr, u_intQI_t v)
{
  u_intQI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intQI_t *) __upc_rptr_to_addr (dthread, daddr);
  GUPCR_WRITE_FENCE ();
  *addr = v;
  GUPCR_FENCE ();
}

//inline
void
__putshi3 (long dthread, long daddr, u_intHI_t v)
{
  u_intHI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intHI_t *) __upc_rptr_to_addr (dthread, daddr);
  GUPCR_WRITE_FENCE ();
  *addr = v;
  GUPCR_FENCE ();
}

//inline
void
__putssi3 (long dthread, long daddr, u_intSI_t v)
{
  u_intSI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intSI_t *) __upc_rptr_to_addr (dthread, daddr);
  GUPCR_WRITE_FENCE ();
  *addr = v;
  GUPCR_FENCE ();
}

//inline
void
__putsdi3 (long dthread, long daddr, u_intDI_t v)
{
  u_intDI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intDI_t *) __upc_rptr_to_addr (dthread, daddr);
  GUPCR_WRITE_FENCE ();
  *addr = v;
  GUPCR_FENCE ();
}

#if GUPCR_TARGET64
//inline
void
__putsti3 (long dthread, long daddr, u_intTI_t v)
{
  u_intTI_t *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (u_intTI_t *) __upc_rptr_to_addr (dthread, daddr);
  GUPCR_WRITE_FENCE ();
  *addr = v;
  GUPCR_FENCE ();
}
#endif /* GUPCR_TARGET64 */
//inline
void
__putssf3 (long dthread, long daddr, float v)
{
  float *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (float *) __upc_rptr_to_addr (dthread, daddr);
  GUPCR_WRITE_FENCE ();
  *addr = v;
  GUPCR_FENCE ();
}

//inline
void
__putsdf3 (long dthread, long daddr, double v)
{
  double *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (double *) __upc_rptr_to_addr (dthread, daddr);
  GUPCR_WRITE_FENCE ();
  *addr = v;
  GUPCR_FENCE ();
}

//inline
void
__putstf3 (long dthread, long daddr, long double v)
{
  long double *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (long double *) __upc_rptr_to_addr (dthread, daddr);
  GUPCR_WRITE_FENCE ();
  *addr = v;
  GUPCR_FENCE ();
}

//inline
void
__putsxf3 (long dthread, long daddr, long double v)
{
  long double *addr;
  GUPCR_OMP_CHECK ();
  if (!daddr)
    __upc_fatal ("Invalid access via null remote offset");
  addr = (long double *) __upc_rptr_to_addr (dthread, daddr);
  GUPCR_WRITE_FENCE ();
  *addr = v;
  GUPCR_FENCE ();
}

//inline
void
__putsblk4 (const void *src, long dthread, long daddr, size_t n)
{
  GUPCR_OMP_CHECK ();
  GUPCR_WRITE_FENCE ();
  __remote_put (src, dthread, daddr, n);
  GUPCR_FENCE ();
}

//end lib_inline_access
