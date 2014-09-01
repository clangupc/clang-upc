/*===-- gupcr_atomic_sup.c - UPC Runtime Support Library -----------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_lib.h"
#include "gupcr_sup.h"
#include "gupcr_fabric.h"
#include "gupcr_gmem.h"
#include "gupcr_utils.h"
#include "gupcr_atomic_sup.h"

/**
 * @file gupcr_atomic_sup.c
 * GUPC Libfabric atomic support routines.
 *
 * @addtogroup ATOMIC GUPCR Atomics Support Functions
 * @{
 */

/** Atomic local access MR handle */
static struct fid_mr gupcr_atomic_mr;
/** Atomic local access MR counting events handle */
static struct fid_cntr gupcr_atomic_mr_ct;
/** Atomic local access MR event queue handle */
static struct fid_eq gupcr_atomic_mr_eq;
/** Atomic number of received ACKs on local MR */
static size_t gupcr_atomic_mr_count;

/** Atomic operations use remote gmem network connection */
#define GUPCR_NC_ATOMIC GUPCR_NC_GMEM

/**
 * Atomic GET operation.
 *
 * A simple get operation is sufficient for data
 * types supported by UPC.
 *
 * @param[in] thread Destination thread
 * @param[in] doffset Destination offset
 * @param[in] fetch_ptr Fetch value pointer
 * @param[in] type Atomic data type
 */
void
gupcr_atomic_get (size_t dthread, size_t doffset, void *fetch_ptr,
		  enum fi_datatype type)
{
  char tmpbuf[128] __attribute__ ((unused));
  size_t size;

  gupcr_debug (FC_ATOMIC, "%lu:0x%lx", dthread, doffset);
  if (fetch_ptr == NULL)
    gupcr_error ("UPC_GET fetch pointer is NULL");

  size = gupcr_get_atomic_size (type);

  // TODO Atomic GET operation

  gupcr_atomic_mr_count += 1;

  /* Wait for atomic completion.  */

  /* Check for failures.  */


  gupcr_debug (FC_ATOMIC, "ov(%s)",
	       gupcr_get_buf_as_hex (tmpbuf, fetch_ptr, size));
}

/**
 * Atomic set operation.
 *
 * Execute atomic SWAP operation.
 *
 * @param[in] thread Destination thread
 * @param[in] doffset Destination offset
 * @param[in] fetch_ptr Fetch value pointer (optional)
 * @param[in] value New value of atomic variable
 * @param[in] type Atomic data type
 */
void
gupcr_atomic_set (size_t dthread, size_t doffset, void *fetch_ptr,
		  const void *value, enum fi_datatype type)
{
  char tmpbuf[128] __attribute__ ((unused));
  char atomic_tmp_buf[GUPC_MAX_ATOMIC_SIZE];
  size_t size = gupcr_get_atomic_size (type);
  gupcr_debug (FC_ATOMIC, "%lu:0x%lx v(%s)", dthread, doffset,
	       gupcr_get_buf_as_hex (tmpbuf, value, size));

  // TODO Atomic SWAP operation
  gupcr_atomic_mr_count += 1;

  /* Wait for SWAP completion.  */

  /* Check for SWAP failure.  */

  /* Copy result into the user's space if necessary.  */
  if (fetch_ptr)
    {
      gupcr_debug (FC_ATOMIC, "ov(%s)",
		   gupcr_get_buf_as_hex (tmpbuf, atomic_tmp_buf, size));
      memcpy (fetch_ptr, atomic_tmp_buf, size);
    }
}

/**
 * Atomic CSWAP operation.
 *
 * @param[in] thread Destination thread
 * @param[in] doffset Destination offset
 * @param[in] fetch_ptr Fetch value pointer (optional)
 * @param[in] expected Expected value of atomic variable
 * @param[in] value New value of atomic variable
 * @param[in] type Atomic data type
 */
void
gupcr_atomic_cswap (size_t dthread, size_t doffset, void *fetch_ptr,
		    const void *expected, const void *value,
		    enum fi_datatype type)
{
  char tmpbuf[128] __attribute__ ((unused));
  char atomic_tmp_buf[GUPC_MAX_ATOMIC_SIZE];
  size_t size = gupcr_get_atomic_size (type);
  gupcr_debug (FC_ATOMIC, "%lu:0x%lx v(%s) e(%s)", dthread, doffset,
	       gupcr_get_buf_as_hex (tmpbuf, value, size),
	       gupcr_get_buf_as_hex (tmpbuf, expected, size));

  gupcr_atomic_mr_count += 1;

  /* Wait for CSWAP completion.  */

  /* Check for CSWAP failure.  */

  /* Copy result into the user's space if necessary.  */
  if (fetch_ptr)
    {
      gupcr_debug (FC_ATOMIC, "ov(%s)",
		   gupcr_get_buf_as_hex (tmpbuf, atomic_tmp_buf, size));
      memcpy (fetch_ptr, atomic_tmp_buf, size);
    }
}

/**
 * Atomic operation.
 *
 * Execute atomic function and return the old value
 * if requested.
 * @param[in] thread Destination thread
 * @param[in] doffset Destination offset
 * @param[in] fetch_ptr Fetch value pointer (optional)
 * @param[in] value Atomic value for the operation
 * @param[in] op Atomic operation
 * @param[in] type Atomic data type
 */
void
gupcr_atomic_op (size_t dthread, size_t doffset, void *fetch_ptr,
		 const void *value, enum fi_op op, enum fi_datatype type)
{
  char tmpbuf[128] __attribute__ ((unused));
  char atomic_tmp_buf[GUPC_MAX_ATOMIC_SIZE];
  size_t size = gupcr_get_atomic_size (type);
  gupcr_debug (FC_ATOMIC, "%lu:0x%lx %s:%s v(%s)", dthread, doffset,
	       gupcr_strptlop (op), gupcr_strptldatatype (type),
	       gupcr_get_buf_as_hex (tmpbuf, value, size));
  if (fetch_ptr)
    {
      //
    }
  else
    {
      //
    }
  gupcr_atomic_mr_count += 1;
  /* Wait for atomic completion.  */

  /* Check for atomic errors.  */

  /* Copy result into the user space if necessary.  */
  if (fetch_ptr)
    {
      gupcr_debug (FC_ATOMIC, "ov(%s)",
		   gupcr_get_buf_as_hex (tmpbuf, atomic_tmp_buf, size));
      memcpy (fetch_ptr, atomic_tmp_buf, size);
    }
}

/**
 * Initialize atomics resources.
 * @ingroup INIT
 */
void
gupcr_atomic_init (void)
{
  gupcr_log (FC_ATOMIC, "atomic init called");

  /* Setup the Fabric MR for local source/destination copying.
     We need to map the whole user's space (same as gmem).  */

  /* Reset number of acknowledgments.  */
  gupcr_atomic_mr_count = 0;
}

/**
 * Release atomics resources.
 * @ingroup INIT
 */
void
gupcr_atomic_fini (void)
{
  gupcr_log (FC_ATOMIC, "atomic fini called");
  /* Release atomic MR and its resources.  */
}

/** @} */
