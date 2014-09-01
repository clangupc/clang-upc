/*===-- gupcr_broadcast.c - UPC Runtime Support Library ------------------===
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
#include "gupcr_utils.h"
#include "gupcr_barrier.h"
#include "gupcr_broadcast.h"

/**
 * @file gupcr_broadcast.c
 * GUPC Portals4 broadcast implementation.
 *
 * The broadcast utility functions are internal functions used by
 * the UPC runtime when it is necessary to propagate (broadcast)
 * a value from thread 0 to all other threads.
 * For example, upc_all_alloc and upc_all_lock_alloc will
 * call the broadcast functions to propagate their result to all threads.
 *
 * The broadcast functions use an algorithm that is a variant of
 * the one used to implement a barrier.  The "up phase" signals
 * that each thread is ready to receive the broadcast value, while the
 * "down phase" is used to receive the actual value.
 */

/**
 * @addtogroup BROADCAST GUPCR Broadcast Functions
 * @{
 */

/**
 * Receive the broadcast value.
 *
 * A thread signals to its parent (by writing to its parent's UP PTE)
 * that it is ready to receive a broadcast value, and then waits on
 * its down LE counting event until the message arrives.
 * @param [in] value Pointer to received value
 * @param [in] nbytes Number of bytes to receive
 */
void
gupcr_broadcast_get (void *value, size_t nbytes)
{
  if (!MYTHREAD)
    gupcr_fatal_error ("called from thread 0");
  if (nbytes > GUPCR_MAX_BROADCAST_SIZE)
    gupcr_fatal_error ("size of broadcast message (%ld) is greater then "
		       "the maximum allowed (%d)",
		       (long int) nbytes, GUPCR_MAX_BROADCAST_SIZE);
  /* Wait to receive the broadcast message.  */
  gupcr_bcast_recv (value, nbytes);
}

/**
 * Send the broadcast value.  This function must be called
 * only by thread 0 (the root thread).  The broadcast value
 * is pushed down the tree by first sending the value
 * to the children of the root thread.
 *
 * @param [in] value Pointer to send value
 * @param [in] nbytes Number of bytes to send
 */
void
gupcr_broadcast_put (void *value, size_t nbytes)
{
  if (THREADS == 1)
    return;
  if (MYTHREAD)
    gupcr_fatal_error ("called from thread other then 0");
  if (nbytes > GUPCR_MAX_BROADCAST_SIZE)
    gupcr_fatal_error ("size of broadcast message (%ld) is greater then "
		       "maximum allowed (%d)",
		       (long int) nbytes, GUPCR_MAX_BROADCAST_SIZE);
  /* Send the broadcast message to the children of the root thread.  */
  gupcr_bcast_send (value, nbytes);
}

/**
 * Initialize broadcast resources.
 * @ingroup INIT
 */
void
gupcr_broadcast_init (void)
{
  gupcr_log (FC_BROADCAST, "broadcast init called");
}

/**
 * Release broadcast resources.
 * @ingroup INIT
 */
void
gupcr_broadcast_fini (void)
{
  gupcr_log (FC_BROADCAST, "broadcast fini called");
}

/** @} */
