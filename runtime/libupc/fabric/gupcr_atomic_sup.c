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
#include "gupcr_iface.h"
#include "gupcr_atomic_sup.h"

/**
 * @file gupcr_atomic_sup.c
 * GUPC Libfabric atomic support routines.
 *
 * @addtogroup ATOMIC GUPCR Atomics Support Functions
 * @{
 */

/** Fabric communications endpoint resources */
/** RX Endpoint comm resource  */
static fab_ep_t gupcr_atomic_rx_ep;
/** TX Endpoint comm resource  */
static fab_ep_t gupcr_atomic_tx_ep;
/** Atomic local access MR handle */
static fab_mr_t gupcr_atomic_mr;
/** Atomic local access MR counting event handle */
static fab_cntr_t gupcr_atomic_ct;
/** Atomic local access MR complete queue handle */
static fab_cq_t gupcr_atomic_cq;
/** Atomic number of received ACKs on local MR */
static size_t gupcr_atomic_mr_count;
#if MR_LOCAL_NEEDED
/** Local memory access memory region  */
static fab_mr_t gupcr_atomic_lmr;
#endif

/** Index of the local memory location */
#define GUPCR_LOCAL_INDEX(addr) \
        (void *) ((char *) addr - (char *) USER_PROG_MEM_START)

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
  int status;

  gupcr_debug (FC_ATOMIC, "%lu:0x%lx", dthread, doffset);
  if (fetch_ptr == NULL)
    gupcr_error ("UPC_GET fetch pointer is NULL");

  // Atomic GET operation
  gupcr_fabric_call (fi_fetch_atomic,
		     (gupcr_atomic_tx_ep, NULL,
		      1, NULL, GUPCR_LOCAL_INDEX (fetch_ptr),
		      NULL, fi_rx_addr ((fi_addr_t) dthread,
						   GUPCR_SERVICE_ATOMIC,
						   GUPCR_SERVICE_BITS),
		      doffset, GUPCR_MR_ATOMIC, type,
		      FI_ATOMIC_READ, NULL));
  gupcr_atomic_mr_count += 1;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_atomic_ct, gupcr_atomic_mr_count,
			 GUPCR_TRANSFER_TIMEOUT));
  GUPCR_CNT_ERROR_CHECK (status, "atomic_get", gupcr_atomic_cq);
  gupcr_debug (FC_ATOMIC, "ov(%s)",
	       gupcr_get_buf_as_hex (tmpbuf, fetch_ptr,
				     gupcr_get_atomic_size (type)));
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
  int status;
  char tmpbuf[128] __attribute__ ((unused));
  char atomic_tmp_buf[GUPC_MAX_ATOMIC_SIZE];
  size_t size = gupcr_get_atomic_size (type);
  gupcr_debug (FC_ATOMIC, "%lu:0x%lx v(%s)", dthread, doffset,
	       gupcr_get_buf_as_hex (tmpbuf, value, size));


  gupcr_fabric_call (fi_fetch_atomic,
		     (gupcr_atomic_tx_ep, GUPCR_LOCAL_INDEX (value),
		      1, NULL, GUPCR_LOCAL_INDEX (atomic_tmp_buf),
		      NULL, fi_rx_addr ((fi_addr_t) dthread,
						   GUPCR_SERVICE_ATOMIC,
						   GUPCR_SERVICE_BITS),
		      doffset, GUPCR_MR_ATOMIC, type,
		      FI_ATOMIC_WRITE, NULL));

  gupcr_atomic_mr_count += 1;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_atomic_ct, gupcr_atomic_mr_count,
			 GUPCR_TRANSFER_TIMEOUT));
  GUPCR_CNT_ERROR_CHECK (status, "atomic_set", gupcr_atomic_cq);
  /* Copy result into the user's space if necessary.  */
  if (fetch_ptr)
    {
      gupcr_debug (FC_ATOMIC, "ov(%s)",
		   gupcr_get_buf_as_hex (tmpbuf, atomic_tmp_buf, size));
      memcpy (fetch_ptr, atomic_tmp_buf, gupcr_get_atomic_size (type));
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
  int status;
  char tmpbuf[128] __attribute__ ((unused));
  char atomic_tmp_buf[GUPC_MAX_ATOMIC_SIZE];
  size_t size = gupcr_get_atomic_size (type);
  gupcr_debug (FC_ATOMIC, "%lu:0x%lx v(%s) e(%s)", dthread, doffset,
	       gupcr_get_buf_as_hex (tmpbuf, value, size),
	       gupcr_get_buf_as_hex (tmpbuf, expected, size));

  gupcr_fabric_call (fi_compare_atomic,
		     (gupcr_atomic_tx_ep, GUPCR_LOCAL_INDEX (value),
		      1, NULL, GUPCR_LOCAL_INDEX (expected),
		      NULL, GUPCR_LOCAL_INDEX (atomic_tmp_buf),
		      NULL, fi_rx_addr ((fi_addr_t) dthread,
						   GUPCR_SERVICE_ATOMIC,
						   GUPCR_SERVICE_BITS),
		      doffset, GUPCR_MR_ATOMIC, type,
		      FI_CSWAP, NULL));

  gupcr_atomic_mr_count += 1;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_atomic_ct, gupcr_atomic_mr_count,
			 GUPCR_TRANSFER_TIMEOUT));
  GUPCR_CNT_ERROR_CHECK (status, "atomic_cswap", gupcr_atomic_cq);

  /* Copy result into the user's space if necessary.  */
  if (fetch_ptr)
    {
      gupcr_debug (FC_ATOMIC, "ov(%s)",
		   gupcr_get_buf_as_hex (tmpbuf, atomic_tmp_buf, size));
      memcpy (fetch_ptr, atomic_tmp_buf, gupcr_get_atomic_size (type));
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
  int status;
  char tmpbuf[128] __attribute__ ((unused));
  char atomic_tmp_buf[GUPC_MAX_ATOMIC_SIZE];
  size_t size = gupcr_get_atomic_size (type);
  gupcr_debug (FC_ATOMIC, "%lu:0x%lx %s:%s v(%s)", dthread, doffset,
	       gupcr_strop (op), gupcr_strdatatype (type),
	       gupcr_get_buf_as_hex (tmpbuf, value, size));
  if (fetch_ptr)
    {
      gupcr_fabric_call (fi_fetch_atomic,
			 (gupcr_atomic_tx_ep, GUPCR_LOCAL_INDEX (value),
			 1, NULL, GUPCR_LOCAL_INDEX (atomic_tmp_buf),
			 NULL, fi_rx_addr ((fi_addr_t) dthread,
					   GUPCR_SERVICE_ATOMIC,
					   GUPCR_SERVICE_BITS),
			 doffset, GUPCR_MR_ATOMIC, type,
			 op, NULL));
    }
  else
    {
      gupcr_fabric_call (fi_atomic,
			 (gupcr_atomic_tx_ep, GUPCR_LOCAL_INDEX (value),
			 1, NULL, fi_rx_addr ((fi_addr_t) dthread,
					      GUPCR_SERVICE_ATOMIC,
					      GUPCR_SERVICE_BITS),
			 doffset, GUPCR_MR_ATOMIC, type,
			 op, NULL));
    }
  gupcr_atomic_mr_count += 1;
  gupcr_fabric_call_nc (fi_cntr_wait, status,
			(gupcr_atomic_ct, gupcr_atomic_mr_count,
			 GUPCR_TRANSFER_TIMEOUT));
  GUPCR_CNT_ERROR_CHECK (status, "atomic_cswap", gupcr_atomic_cq);

  /* Copy result into the user space if necessary.  */
  if (fetch_ptr)
    {
      gupcr_debug (FC_ATOMIC, "ov(%s)",
		   gupcr_get_buf_as_hex (tmpbuf, atomic_tmp_buf, size));
      memcpy (fetch_ptr, atomic_tmp_buf, gupcr_get_atomic_size (type));
    }
}

/**
 * Initialize atomics resources.
 * @ingroup INIT
 */
void
gupcr_atomic_init (void)
{
  cntr_attr_t cntr_attr = { 0 };
  cq_attr_t cq_attr = { 0 };
  tx_attr_t tx_attr = { 0 };
  rx_attr_t rx_attr = { 0 };

  gupcr_log (FC_ATOMIC, "atomic init called");

  /* Create context endpoints for ATOMIC transfers.  */
  tx_attr.op_flags = FI_DELIVERY_COMPLETE;
  gupcr_fabric_call (fi_tx_context,
		     (gupcr_ep, GUPCR_SERVICE_ATOMIC, &tx_attr, &gupcr_atomic_tx_ep,
		      NULL));
  gupcr_fabric_call (fi_rx_context,
		     (gupcr_ep, GUPCR_SERVICE_ATOMIC, &rx_attr, &gupcr_atomic_rx_ep,
		      NULL));

  /* ... and completion cntr/cq for remote read/write.  */
  cntr_attr.events = FI_CNTR_EVENTS_COMP;
  cntr_attr.flags = 0;
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr,
				    &gupcr_atomic_ct, NULL));
  gupcr_fabric_call (fi_ep_bind, (gupcr_atomic_tx_ep, &gupcr_atomic_ct->fid,
				  FI_READ | FI_WRITE));
  gupcr_atomic_mr_count = 0;

  /* ... and completion queue for remote target transfer errors.  */
  cq_attr.size = 1;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr, &gupcr_atomic_cq, NULL));
  /* Use FI_SELECTIVE_COMPLETION flag to report errors only.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_atomic_tx_ep, &gupcr_atomic_cq->fid,
				  FI_WRITE | FI_READ |
				  FI_SELECTIVE_COMPLETION));

#if LOCAL_MR_NEEDED
  /* NOTE: Create a local memory region before enabling endpoint.  */
  /* ... and memory region for local memory accesses.  */
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, USER_PROG_MEM_START,
				 USER_PROG_MEM_SIZE, FI_READ | FI_WRITE,
				 0, 0, 0, &gupcr_atomic_lmr, NULL));
  /* NOTE: There is no need to bind local memory region to endpoint.  */
  /*       Hmm ... ? We can probably use only one throughout the runtime,  */
  /*       as counters and events are bound to endpoint.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_atomic_tx_ep, &gupcr_atomic_lmr->fid,
				  FI_READ | FI_WRITE));
#endif

  /* Enable TX endpoint.  */
  gupcr_fabric_call (fi_enable, (gupcr_atomic_tx_ep));

  /* ... and memory region for remote inbound accesses.  */
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, gupcr_gmem_base, gupcr_gmem_size,
				 FI_REMOTE_READ | FI_REMOTE_WRITE, 0,
				 GUPCR_MR_ATOMIC, 0, &gupcr_atomic_mr, NULL));

  /* Enable RX endpoint and bind MR.  */
  gupcr_fabric_call (fi_enable, (gupcr_atomic_rx_ep));
  gupcr_fabric_call (fi_ep_bind, (gupcr_atomic_rx_ep, &gupcr_atomic_mr->fid,
			          FI_REMOTE_READ | FI_REMOTE_WRITE));

  gupcr_log (FC_ATOMIC, "atomic init completed");
}

/**
 * Release atomics resources.
 * @ingroup INIT
 */
void
gupcr_atomic_fini (void)
{
  int status;
  gupcr_log (FC_ATOMIC, "atomic fini called");
  gupcr_fabric_call (fi_close, (&gupcr_atomic_ct->fid));
  gupcr_fabric_call (fi_close, (&gupcr_atomic_cq->fid));
  gupcr_fabric_call (fi_close, (&gupcr_atomic_mr->fid));
#if LOCAL_MR_NEEDED
  gupcr_fabric_call (fi_close, (&gupcr_atomic_lmr->fid));
#endif
  /* NOTE: Do not check for errors.  Fails occasionally.  */
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_atomic_rx_ep->fid));
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_atomic_tx_ep->fid));
  gupcr_log (FC_ATOMIC, "atomic fini completed");
}

/** @} */
