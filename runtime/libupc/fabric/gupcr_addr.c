/*===-- gupcr_addr.c - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_sup.h"
#include "gupcr_fabric.h"
#include "gupcr_iface.h"
#include "gupcr_node.h"
#include "gupcr_gmem.h"
#include "gupcr_utils.h"

/**
 * @file gupcr_addr.c
 * GUPC Portals4 shared address utility routines
 */

/**
 * @addtogroup IFACE GUPC Interface Routines
 * @{
 */

//begin lib_inline_access
/**
 * Find local pointer from pointer-to-shared.
 *
 * @param [in] p Pointer-to-shared
 * @return Local address associated with "p"
 */
//inline
void *
__cvtaddr (upc_shared_ptr_t p)
{
  void *addr;
  if (GUPCR_PTS_IS_NULL (p))
    return (void *) 0;
  addr = GUPCR_GMEM_OFF_TO_LOCAL (GUPCR_PTS_THREAD (p), GUPCR_PTS_OFFSET (p));
  return addr;
}

//end lib_inline_access

/**
 * Find local pointer from pointer-to-shared.
 *
 * The pointer-to-shared value must have affinity to the current thread.
 * @param [in] p Pointer-to-shared
 * @return Local address associated with "p"
 */
void *
__getaddr (upc_shared_ptr_t p)
{
  void *addr;
  if (GUPCR_PTS_IS_NULL (p))
    return (void *) 0;
  if ((int) GUPCR_PTS_THREAD (p) != MYTHREAD)
    gupcr_fatal_error
      ("invalid conversion of shared address to local pointer;\n"
       "thread does not have affinity to shared address");
  addr = GUPCR_GMEM_OFF_TO_LOCAL (MYTHREAD, GUPCR_PTS_OFFSET (p));
  return addr;
}

/** @} */

/**
 * @addtogroup PTSMANIP UPC Pointer-to-shared Manipulation Functions
 * @{
 */

/**
 * Return the thread of a pointer-to-shared value.
 *
 * The upc_threadof function returns the index of the thread
 * that has affinity to the shared object pointed to by the argument.
 * @param [in] p Pointer-to-shared argument
 * @retval Thread ID of the argument
 */
size_t
upc_threadof (upc_shared_ptr_t p)
{
  if ((int) GUPCR_PTS_THREAD (p) >= THREADS)
    gupcr_fatal_error ("thread number %d in shared address is out of range",
		       (int) GUPCR_PTS_THREAD (p));
  return (size_t) GUPCR_PTS_THREAD (p);
}

/**
 * Return the phase of a pointer-to-shared value.
 *
 * The upc_phaseof function returns the phase component of the
 * pointer-to-shared argument.
 * @param [in] p Pointer-to-shared argument
 * @retval Phase of the argument
 */
size_t
upc_phaseof (upc_shared_ptr_t p)
{
  if ((int) GUPCR_PTS_THREAD (p) >= THREADS)
    gupcr_fatal_error ("thread number %d in shared address is out of range",
		       (int) GUPCR_PTS_THREAD (p));
  return (size_t) GUPCR_PTS_PHASE (p);
}

/**
 * Reset the phase field of a pointer-to-shared value.
 *
 * The upc_resetphase function returns a pointer-to-shared value which
 * is identical to its input except that it has zero phase.
 * @param [in] p Pointer-to-shared argument
 * @retval Pointer-to-shared with zero phase
 */
upc_shared_ptr_t
upc_resetphase (upc_shared_ptr_t p)
{
  upc_shared_ptr_t result;
  result = p;
  GUPCR_PTS_SET_PHASE (result, 0);
  return result;
}

/**
 * Return the address field of a pointer-to-shared value.
 *
 * The upc_addrfield function returns an implementation-defined
 * value reflecting the 'local address' of the object pointed to
 * by the pointer-to-shared argument.
 * @param [in] p Pointer-to-shared argument
 * @retval Address field of the argument
 */
size_t
upc_addrfield (upc_shared_ptr_t p)
{
  if ((int) GUPCR_PTS_THREAD (p) >= THREADS)
    gupcr_fatal_error ("thread number %d in shared address is out of range",
		       (int) GUPCR_PTS_THREAD (p));
  return (size_t) GUPCR_PTS_VADDR (p);
}

/**
 * Return the size of the local portion of the shared data
 * with a layout described by the input parameters.
 *
 * A convenience function which calculates the exact size
 * of the local portion of the data in a shared object with affinity to
 * the thread identified by the 'threadid' parameter.
 * @param [in] totalsize Size of the shared data
 * @param [in] nbytes Size of the block
 * @param [in] threadid Requested thread number
 * @retval Size of the shared space described by the function arguments
 */
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
      else
	result = (size_t) ((nblocks / THREADS) * nbytes)
	  + totalsize - nblocks * nbytes;
    }
  return result;
}

/** @} */
