/*===-- upc_llvm_access.c - UPC Runtime Support Library ------------------===
|*
|*                  The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_access.h"
#include "gupcr_sync.h"
#include "gupcr_sup.h"
#include "gupcr_portals.h"
#include "gupcr_node.h"
#include "gupcr_gmem.h"
#include "gupcr_utils.h"

/**
 * @file gupcr_llvm_access.c
 * Clang UPC compiler llvm access functions.
 */

/**
 * @addtogroup IFACE UPC Interface Routines
 * @{
 */

//begin lib_inline_access

/**
 * Relaxed remote "char (8 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Char (8 bits) value at the remote address given by 'p'.
 */
//inline
u_intQI_t
__getqi3 (long thread, size_t offset)
{
  u_intQI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER R QI LOCAL");
      result = *(u_intQI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER R QI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%x",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Relaxed remote "short (16 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Short (16 bits) value at the remote address given by 'p'.
 */
//inline
u_intHI_t
__gethi3 (long thread, size_t offset)
{
  u_intHI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER R HI LOCAL");
      result = *(u_intHI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER R HI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%x",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Relaxed remote "int (32 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Int (32 bits) value at the remote address given by 'p'.
 */
//inline
u_intSI_t
__getsi3 (long thread, size_t offset)
{
  u_intSI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER R SI LOCAL");
      result = *(u_intSI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER R SI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%x",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Relaxed remote "long (64 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Long (64 bits) value at the remote address given by 'p'.
 */
//inline
u_intDI_t
__getdi3 (long thread, size_t offset)
{
  u_intDI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER R DI LOCAL");
      result = *(u_intDI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER R DI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%llx",
	       thread, (long unsigned) offset, (long long unsigned) result);
  return result;
}

#if GUPCR_TARGET64
/**
 * Relaxed remote "long long (128 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Long long (128 bits) value at the remote address given by 'p'.
 */
//inline
u_intTI_t
__getti3 (long thread, size_t offset)
{
  u_intTI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER R TI LOCAL");
      result = *(u_intTI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER R TI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%llx",
	       thread, (long unsigned) offset, (long long unsigned) result);
  return result;
}
#endif /* GUPCR_TARGET64 */
/**
 * Relaxed remote "float" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Float value at the remote address given by 'p'.
 */
//inline
float
__getsf3 (long thread, size_t offset)
{
  float result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER R SF LOCAL");
      result = *(float *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER R SF REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx %6g",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Relaxed remote "double" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Double value at the remote address given by 'p'.
 */
//inline
double
__getdf3 (long thread, size_t offset)
{
  double result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER R DF LOCAL");
      result = *(double *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER R DF REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx %6g",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Relaxed remote memory block get operation.
 * Copy the data at the remote address 'src' into the local memory
 * destination at the address 'dest'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] dest Local address of the destination memory block.
 * @param [in] src Remote address of the source memory block.
 * @param [in] n Number of bytes to transfer.
 */
//inline
void
__getblk4 (long thread, size_t offset, void *dest, size_t n)
{
  gupcr_trace (FC_MEM, "GETBLK ENTER R");
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      GUPCR_MEM_BARRIER ();
      memcpy (dest, GUPCR_GMEM_OFF_TO_LOCAL (thread, offset), n);
      GUPCR_READ_MEM_BARRIER ();
    }
  else
    {
      gupcr_gmem_get (dest, thread, offset, n);
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GETBLK EXIT R %d:0x%lx 0x%lx %lu",
	       thread, (long unsigned) offset,
	       (long unsigned) dest, (long unsigned) n);
}

/**
 * Relaxed remote "char (8 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putqi3 (long thread, size_t offset, u_intQI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER R QI LOCAL "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      *(u_intQI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER R QI REMOTE "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	  /* There can be only one outstanding unordered put.  */
	  gupcr_pending_strict_put = 1;
	}
    }
  gupcr_trace (FC_MEM, "PUT EXIT R QI");
}

/**
 * Relaxed remote "short (16 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__puthi3 (long thread, size_t offset, u_intHI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER R HI LOCAL "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      *(u_intHI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER R HI REMOTE "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	  /* There can be only one outstanding unordered put.  */
	  gupcr_pending_strict_put = 1;
	}
    }
  gupcr_trace (FC_MEM, "PUT EXIT R HI");
}

/**
 * Relaxed remote "int (32 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putsi3 (long thread, size_t offset, u_intSI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER R SI LOCAL "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      *(u_intSI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER R SI REMOTE "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	  /* There can be only one outstanding unordered put.  */
	  gupcr_pending_strict_put = 1;
	}
    }
  gupcr_trace (FC_MEM, "PUT EXIT R SI");
}

/**
 * Relaxed remote "long (64 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putdi3 (long thread, size_t offset, u_intDI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER R DI LOCAL "
		   "0x%llx %d:0x%lx",
		   (long long unsigned) v, thread, (long unsigned) offset);
      *(u_intDI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER R DI REMOTE "
		   "0x%llx %d:0x%lx",
		   (long long unsigned) v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	  /* There can be only one outstanding unordered put.  */
	  gupcr_pending_strict_put = 1;
	}
    }
  gupcr_trace (FC_MEM, "PUT EXIT R DI");
}

#if GUPCR_TARGET64
/**
 * Relaxed remote "long long (128 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putti3 (long thread, size_t offset, u_intTI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER R TI LOCAL "
		   "0x%llx %d:0x%lx",
		   (long long unsigned) v, thread, (long unsigned) offset);
      *(u_intTI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER R TI REMOTE "
		   "0x%llx %d:0x%lx",
		   (long long unsigned) v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	  /* There can be only one outstanding unordered put.  */
	  gupcr_pending_strict_put = 1;
	}
    }
  gupcr_trace (FC_MEM, "PUT EXIT R TI");
}
#endif /* GUPCR_TARGET64 */
/**
 * Relaxed remote "float" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putsf3 (long thread, size_t offset, float v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER R SF LOCAL "
		   "%6g %d:0x%lx", v, thread, (long unsigned) offset);
      *(float *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER R SF REMOTE "
		   "%6g %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	  /* There can be only one outstanding unordered put.  */
	  gupcr_pending_strict_put = 1;
	}
    }
  gupcr_trace (FC_MEM, "PUT EXIT R SF");
}

/**
 * Relaxed remote "double" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putdf3 (long thread, size_t offset, double v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER R DF LOCAL "
		   "%6g %d:0x%lx", v, thread, (long unsigned) offset);
      *(double *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER R DF REMOTE "
		   "%6g %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	  /* There can be only one outstanding unordered put.  */
	  gupcr_pending_strict_put = 1;
	}
    }
  gupcr_trace (FC_MEM, "PUT EXIT R DF");
}

/**
 * Relaxed remote memory block put operation.
 * Copy the data at the local address 'src' into the remote memory
 * destination at the address 'dest'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] dest Remote address of the destination memory block.
 * @param [in] src Local address of the source memory block.
 * @param [in] n Number of bytes to transfer.
 */
//inline
void
__putblk4 (void *src, long thread, size_t offset, size_t n)
{
  gupcr_trace (FC_MEM, "PUTBLK ENTER R 0x%lx %d:0x%lx %lu",
	       (long unsigned) src, thread,
	       (long unsigned) offset, (long unsigned) n);
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      memcpy (GUPCR_GMEM_OFF_TO_LOCAL (thread, offset), src, n);
    }
  else
    {
      gupcr_gmem_put (thread, offset, src, n);
    }
  gupcr_trace (FC_MEM, "PUT_BLK EXIT R");
}

/**
 * Relaxed remote memory block copy operation.
 * Copy the data at the remote address 'src' into the remote memory
 * destination at the address 'dest'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] dest Remote address of destination memory block.
 * @param [in] src Remote address of source memory block.
 * @param [in] n Number of bytes to transfer.
 */
//inline
void
__copyblk5 (long dthread, size_t doffset,
	    long sthread, size_t soffset, size_t n)
{
  gupcr_trace (FC_MEM, "COPYBLK ENTER R %d:0x%lx %d:0x%lx %lu",
	       sthread, (long unsigned) soffset,
	       dthread, (long unsigned) doffset, (long unsigned) n);
  gupcr_assert (dthread < THREADS);
  gupcr_assert (doffset != 0);
  gupcr_assert (sthread < THREADS);
  gupcr_assert (soffset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (dthread) && GUPCR_GMEM_IS_LOCAL (sthread))
    {
      memcpy (GUPCR_GMEM_OFF_TO_LOCAL (dthread, doffset),
	      GUPCR_GMEM_OFF_TO_LOCAL (sthread, soffset), n);
    }
  else if (GUPCR_GMEM_IS_LOCAL (dthread))
    {
      gupcr_gmem_get (GUPCR_GMEM_OFF_TO_LOCAL (dthread, doffset),
		      sthread, soffset, n);
      gupcr_gmem_sync_gets ();
    }
  else if (GUPCR_GMEM_IS_LOCAL (sthread))
    {
      gupcr_gmem_put (dthread, doffset,
		      GUPCR_GMEM_OFF_TO_LOCAL (sthread, soffset), n);
    }
  else
    {
      gupcr_gmem_copy (dthread, doffset, sthread, soffset, n);
    }
  gupcr_trace (FC_MEM, "COPY_BLK EXIT R");
}

/**
 * Strict remote "char (8 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Char (8 bits) value at the remote address given by 'p'.
 */
//inline
u_intQI_t
__getsqi3 (long thread, size_t offset)
{
  u_intQI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER S QI LOCAL");
      GUPCR_MEM_BARRIER ();
      result = *(u_intQI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
      GUPCR_READ_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER S QI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%x",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Strict remote "short (16 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Short (16 bits) value at the remote address given by 'p'.
 */
//inline
u_intHI_t
__getshi3 (long thread, size_t offset)
{
  u_intHI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER S HI LOCAL");
      GUPCR_MEM_BARRIER ();
      result = *(u_intHI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
      GUPCR_READ_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER S HI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%x",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Strict remote "int (32 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Int (32 bits) value at the remote address given by 'p'.
 */
//inline
u_intSI_t
__getssi3 (long thread, size_t offset)
{
  u_intSI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER S SI LOCAL");
      GUPCR_MEM_BARRIER ();
      result = *(u_intSI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
      GUPCR_READ_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER S SI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%x",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Strict remote "long (64 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Long (64 bits) value at the remote address given by 'p'.
 */
//inline
u_intDI_t
__getsdi3 (long thread, size_t offset)
{
  u_intDI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER S DI LOCAL");
      GUPCR_MEM_BARRIER ();
      result = *(u_intDI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
      GUPCR_READ_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER S DI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%llx",
	       thread, (long unsigned) offset, (long long unsigned) result);
  return result;
}

#if GUPCR_TARGET64
/**
 * Strict remote "long long (128 bits)" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Long long (128 bits) value at the remote address given by 'p'.
 */
//inline
u_intTI_t
__getsti3 (long thread, size_t offset)
{
  u_intTI_t result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER S TI LOCAL");
      GUPCR_MEM_BARRIER ();
      result = *(u_intTI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
      GUPCR_READ_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER S TI REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx 0x%llx",
	       thread, (long unsigned) offset, (long long unsigned) result);
  return result;
}
#endif /* GUPCR_TARGET64 */
/**
 * Strict remote "float" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Float value at the remote address given by 'p'.
 */
//inline
float
__getssf3 (long thread, size_t offset)
{
  float result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER S SF LOCAL");
      GUPCR_MEM_BARRIER ();
      result = *(float *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
      GUPCR_READ_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER S SF REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx %6g",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Strict remote "double" get operation.
 * Return the value at the remote address 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the source operand.
 * @return Double value at the remote address given by 'p'.
 */
//inline
double
__getsdf3 (long thread, size_t offset)
{
  double result;
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "GET ENTER S DF LOCAL");
      GUPCR_MEM_BARRIER ();
      result = *(double *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset);
      GUPCR_READ_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "GET ENTER S DF REMOTE");
      gupcr_gmem_get (&result, thread, offset, sizeof (result));
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GET EXIT %d:0x%lx %6g",
	       thread, (long unsigned) offset, result);
  return result;
}

/**
 * Strict remote memory block get operation.
 * Copy the data at the remote address 'src' into the local memory
 * destination at the address 'dest'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] dest Local address of the destination memory block.
 * @param [in] src Remote address of the source memory block.
 * @param [in] n Number of bytes to transfer.
 */
//inline
void
__getsblk4 (long thread, size_t offset, void *dest, size_t n)
{
  gupcr_trace (FC_MEM, "GETBLK ENTER S");
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      GUPCR_MEM_BARRIER ();
      memcpy (dest, GUPCR_GMEM_OFF_TO_LOCAL (thread, offset), n);
      GUPCR_READ_MEM_BARRIER ();
    }
  else
    {
      gupcr_gmem_get (dest, thread, offset, n);
      /* All 'get' operations are synchronous.  */
      gupcr_gmem_sync_gets ();
    }
  gupcr_trace (FC_MEM, "GETBLK EXIT S %d:0x%lx 0x%lx %lu",
	       thread, (long unsigned) offset,
	       (long unsigned) dest, (long unsigned) n);
}

/**
 * Strict remote "char (8 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putsqi3 (long thread, size_t offset, u_intQI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER S QI LOCAL "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      GUPCR_WRITE_MEM_BARRIER ();
      *(u_intQI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
      GUPCR_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER S QI REMOTE "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      gupcr_pending_strict_put = 1;
    }
  gupcr_trace (FC_MEM, "PUT EXIT S QI");
}

/**
 * Strict remote "short (16 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putshi3 (long thread, size_t offset, u_intHI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER S HI LOCAL "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      GUPCR_WRITE_MEM_BARRIER ();
      *(u_intHI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
      GUPCR_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER S HI REMOTE "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      gupcr_pending_strict_put = 1;
    }
  gupcr_trace (FC_MEM, "PUT EXIT S HI");
}

/**
 * Strict remote "int (32 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putssi3 (long thread, size_t offset, u_intSI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER S SI LOCAL "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      GUPCR_WRITE_MEM_BARRIER ();
      *(u_intSI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
      GUPCR_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER S SI REMOTE "
		   "0x%x %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      gupcr_pending_strict_put = 1;
    }
  gupcr_trace (FC_MEM, "PUT EXIT S SI");
}

/**
 * Strict remote "long (64 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putsdi3 (long thread, size_t offset, u_intDI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER S DI LOCAL "
		   "0x%llx %d:0x%lx",
		   (long long unsigned) v, thread, (long unsigned) offset);
      GUPCR_WRITE_MEM_BARRIER ();
      *(u_intDI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
      GUPCR_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER S DI REMOTE "
		   "0x%llx %d:0x%lx",
		   (long long unsigned) v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      gupcr_pending_strict_put = 1;
    }
  gupcr_trace (FC_MEM, "PUT EXIT S DI");
}

#if GUPCR_TARGET64
/**
 * Strict remote "long long (128 bits)" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putsti3 (long thread, size_t offset, u_intTI_t v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER S TI LOCAL "
		   "0x%llx %d:0x%lx",
		   (long long unsigned) v, thread, (long unsigned) offset);
      GUPCR_WRITE_MEM_BARRIER ();
      *(u_intTI_t *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
      GUPCR_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER S TI REMOTE "
		   "0x%llx %d:0x%lx",
		   (long long unsigned) v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      gupcr_pending_strict_put = 1;
    }
  gupcr_trace (FC_MEM, "PUT EXIT S TI");
}
#endif /* GUPCR_TARGET64 */
/**
 * Strict remote "float" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putssf3 (long thread, size_t offset, float v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER S SF LOCAL "
		   "%6g %d:0x%lx", v, thread, (long unsigned) offset);
      GUPCR_WRITE_MEM_BARRIER ();
      *(float *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
      GUPCR_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER S SF REMOTE "
		   "%6g %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      gupcr_pending_strict_put = 1;
    }
  gupcr_trace (FC_MEM, "PUT EXIT S SF");
}

/**
 * Strict remote "double" put operation.
 * Store the value given by 'v' into the remote memory destination at 'p'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] p Remote address of the destination address.
 * @param [in] v Source value.
 */
//inline
void
__putsdf3 (long thread, size_t offset, double v)
{
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      gupcr_trace (FC_MEM, "PUT ENTER S DF LOCAL "
		   "%6g %d:0x%lx", v, thread, (long unsigned) offset);
      GUPCR_WRITE_MEM_BARRIER ();
      *(double *) GUPCR_GMEM_OFF_TO_LOCAL (thread, offset) = v;
      GUPCR_MEM_BARRIER ();
    }
  else
    {
      gupcr_trace (FC_MEM, "PUT ENTER S DF REMOTE "
		   "%6g %d:0x%lx", v, thread, (long unsigned) offset);
      if (sizeof (v) <= (size_t) GUPCR_MAX_PUT_ORDERED_SIZE)
	{
	  /* Ordered puts can proceed in parallel.  */
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      else
	{
	  /* Wait for any outstanding 'put' operation.  */
	  gupcr_gmem_sync_puts ();
	  gupcr_gmem_put (thread, offset, &v, sizeof (v));
	}
      gupcr_pending_strict_put = 1;
    }
  gupcr_trace (FC_MEM, "PUT EXIT S DF");
}

/**
 * Strict remote memory block put operation.
 * Copy the data at the local address 'src' into the remote memory
 * destination at the address 'dest'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] dest Remote address of the destination memory block.
 * @param [in] src Local address of the source memory block.
 * @param [in] n Number of bytes to transfer.
 */
//inline
void
__putsblk4 (void *src, long thread, size_t offset, size_t n)
{
  gupcr_trace (FC_MEM, "PUTBLK ENTER S 0x%lx %d:0x%lx %lu",
	       (long unsigned) src, thread,
	       (long unsigned) offset, (long unsigned) n);
  gupcr_assert (thread < THREADS);
  gupcr_assert (offset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (thread))
    {
      GUPCR_WRITE_MEM_BARRIER ();
      memcpy (GUPCR_GMEM_OFF_TO_LOCAL (thread, offset), src, n);
      GUPCR_MEM_BARRIER ();
    }
  else
    {
      gupcr_gmem_put (thread, offset, src, n);
      gupcr_pending_strict_put = 1;
    }
  gupcr_trace (FC_MEM, "PUT_BLK EXIT S");
}

/**
 * Strict remote memory block copy operation.
 * Copy the data at the remote address 'src' into the remote memory
 * destination at the address 'dest'.
 *
 * The interface to this procedure is defined by the UPC compiler API.
 *
 * @param [in] dest Remote address of destination memory block.
 * @param [in] src Remote address of source memory block.
 * @param [in] n Number of bytes to transfer.
 */
//inline
void
__copysblk5 (long dthread, size_t doffset,
	     long sthread, size_t soffset, size_t n)
{
  gupcr_trace (FC_MEM, "COPYBLK ENTER S %d:0x%lx %d:0x%lx %lu",
	       sthread, (long unsigned) soffset,
	       dthread, (long unsigned) doffset, (long unsigned) n);
  gupcr_assert (dthread < THREADS);
  gupcr_assert (doffset != 0);
  gupcr_assert (sthread < THREADS);
  gupcr_assert (soffset != 0);
  if (gupcr_pending_strict_put)
    gupcr_gmem_sync_puts ();
  if (GUPCR_GMEM_IS_LOCAL (dthread) && GUPCR_GMEM_IS_LOCAL (sthread))
    {
      GUPCR_WRITE_MEM_BARRIER ();
      memcpy (GUPCR_GMEM_OFF_TO_LOCAL (dthread, doffset),
	      GUPCR_GMEM_OFF_TO_LOCAL (sthread, soffset), n);
      GUPCR_MEM_BARRIER ();
    }
  else if (GUPCR_GMEM_IS_LOCAL (dthread))
    {
      gupcr_gmem_get (GUPCR_GMEM_OFF_TO_LOCAL (dthread, doffset),
		      sthread, soffset, n);
      gupcr_gmem_sync_gets ();
    }
  else if (GUPCR_GMEM_IS_LOCAL (sthread))
    {
      gupcr_gmem_put (dthread, doffset,
		      GUPCR_GMEM_OFF_TO_LOCAL (sthread, soffset), n);
      gupcr_pending_strict_put = 1;
    }
  else
    {
      gupcr_gmem_copy (dthread, doffset, sthread, soffset, n);
      gupcr_pending_strict_put = 1;
    }
  gupcr_trace (FC_MEM, "COPY_BLK EXIT S");
}

//end lib_inline_access
/** @} */
