/*===-- gupcr_coll_broadcast.upc - UPC Runtime Support Library -----------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include <upc.h>
#include <upc_collective.h>
#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_sup.h"
#include "gupcr_portals.h"
#include "gupcr_gmem.h"
#include "gupcr_utils.h"
#include "gupcr_coll_sup.h"

/**
 * @file gupcr_coll_broadcast.upc
 * GUPC Portals4 collectives broadcast implementation.
 *
 * @addtogroup COLLECTIVES GUPCR Collectives Functions
 * @{
 */

/**
 * @fn upc_all_broadcast (shared void *dst,
 *		   shared const void *src,
 *		   size_t nbytes, upc_flag_t sync_mode)
 * Broadcast data referenced by the src pointer.
 *
 * @param [in] dst Destination shared pointer
 * @param [in] src Source shared pointer
 * @param [in] nbytes Number of bytes to broadcast
 * @param [in] sync_mode Synchronization mode
 * @ingroup COLLECTIVES
 */

void
upc_all_broadcast (shared void *dst, shared const void *src,
		   size_t nbytes, upc_flag_t sync_mode)
{
  size_t src_thread = upc_threadof ((shared void *) src);
  size_t send_cnt = nbytes;
  int i, blk_cnt;

  gupcr_trace (FC_COLL, "COLL ALL_BROADCAST ENTER %lu %lu",
	       (long unsigned) src_thread, (long unsigned) nbytes);
#ifdef _UPC_COLL_CHECK_ARGS
  upc_coll_err (dst, src, NULL, nbytes, sync_mode, 0, 0, 0, UPC_BRDCST);
#endif

  /* Initialize the collectives broadcast tree.  */
  gupcr_coll_tree_setup (src_thread, 0, THREADS);

  /* Optional IN synchronization mode.  */
  if (UPC_IN_MYSYNC & sync_mode || !(UPC_IN_NOSYNC & sync_mode))
    upc_barrier;

  blk_cnt = 0;
  while (send_cnt)
    {
      size_t blk_size = (send_cnt > GUPCR_MAX_MSG_SIZE) ?
	GUPCR_MAX_MSG_SIZE : send_cnt;
      send_cnt -= blk_size;

      if (MYTHREAD != (int) src_thread)
	{
	  /* Wait for parent to deliver data.  */
	  gupcr_coll_signal_wait (1);
	}
      else
	{
	  /* Copy data into the thread's own memory.  */
	  size_t doffset = upc_addrfield ((shared char *) dst + MYTHREAD);
	  size_t soffset = upc_addrfield ((shared void *) src);
	  doffset += blk_cnt * GUPCR_MAX_MSG_SIZE;
	  soffset += blk_cnt * GUPCR_MAX_MSG_SIZE;
	  gupcr_debug (FC_COLL,
		       "Local copy - doffset: %lld soffset: %lld nbytes: %lld",
		       (long long int) doffset, (long long int) soffset,
		       (long long int) nbytes);
	  memcpy ((char *) gupcr_gmem_base + doffset,
		  (char *) gupcr_gmem_base + soffset, blk_size);
	}

      /* Send data to all children.  */
      if (gupcr_coll_child_cnt)
	{
	  for (i = 0; i < gupcr_coll_child_cnt; i++)
	    {
	      int dthread = gupcr_coll_child[i];
	      size_t doffset = upc_addrfield ((shared char *) dst + dthread);
	      size_t soffset = upc_addrfield ((shared char *) dst + MYTHREAD);
	      doffset += blk_cnt * GUPCR_MAX_MSG_SIZE;
	      soffset += blk_cnt * GUPCR_MAX_MSG_SIZE;
	      gupcr_coll_put (dthread, doffset, soffset, blk_size);
	    }
	  gupcr_coll_ack_wait (gupcr_coll_child_cnt);
	}
      ++blk_cnt;
    }

  /* Optional OUT synchronization mode.  */
  if (UPC_OUT_MYSYNC & sync_mode || !(UPC_OUT_NOSYNC & sync_mode))
    upc_barrier;
  gupcr_trace (FC_COLL, "COLL ALL_BROADCAST EXIT");
}

/* @} */
