/*===-- gupcr_barrier.c - UPC Runtime Support Library --------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_barrier.c
 * GUPC Libfabric barrier implementation.
 *
 * The UPC barrier synchronization statements are:
 *  - upc_notify <i>expression</i>
 *  - upc_wait <i>expression</i>
 *  - upc_barrier <i>expression</i>
 *
 * The upc barrier statement is equivalent to the compound statement:
 *   <i>{ upc_notify barreg.wait; upc_wait barreg.wait; }</i>
 *
 * Important rules:
 *  - Each thread executes an alternating sequence of upc_notify and upc_wait
 *    statements.
 *  - A thread completes a <i>upc_wait</i> statement when all threads
 *    have executed a matching <i>upc_notify</i> statement.
 *  - <i>upc_notify</i> and <i>upc_wait</i> are collective operations and
 *    <i>expression</i> (if available) must match across all threads.
 *  - An empty <i>expression</i> matches any barrier ID.
 *
 * The UPC runtime barrier implementation uses an "all reduce"
 * algorithm as outlined in the paper <i>Enabling Flexible Collective
 * Communication Offload with Triggered Operations</i> by Keith Underwood
 * et al. January, 2007. Libfabric atomic operations and triggered
 * atomic operations are used to propagate and verify
 * that all UPC threads have entered the same synchronization phase
 * with matching barrier IDs.
 *
 * For the purposes of implementing UPC barriers, all UPC threads
 * in a given job are organized as a tree.  Thread 0 is the
 * root thread (at the top of the tree). Other threads can be
 * either an inner thread (has at least one child), or a leaf
 * thread (has no children).
 *
 * A UPC barrier is implemented in two distinctive steps: notify and wait.
 *
 * A notify step uses the barrier NC to pass its barrier ID to the
 * parent.  The result of an atomic FI_MIN operation among children and
 * their parent is further sent up the tree until thread 0 is reached.
 *
 * A wait step uses the barrier NC to pass the derived by consensus
 * barrier ID to all threads.  An error is raised if the derived ID
 * does not match the thread's barrier ID.
 *
 * This implementation supports a split phase barrier where a given
 * thread completes its wait statement once all other threads
 * have reached their matching notify statement.
 *
 * Each thread uses the following resources:
 *
 *   - Endpoint for passing barrier IDs UP and DOWN the tree
 *   - MRs for sending a thread's barrier ID to parents and children.  Each
 *     barrier phase (notify, wait) uses its own MR to provide for RMA event
 *     signaling.
 *   - Counting events for endpoint and MRs
 *   - Event queues for failure events on endpoint and MRs
 *
 * Extensive use of Libfabric triggered functions allow for the efficient
 * implementation of a split phase barrier.
 *
 * @addtogroup BARRIER GUPCR Barrier Functions
 * @{
 */

#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_sup.h"
#include "gupcr_sync.h"
#include "gupcr_broadcast.h"
#include "gupcr_fabric.h"
#include "gupcr_iface.h"
#include "gupcr_utils.h"
#include "gupcr_runtime.h"
#include "gupcr_barrier_sup.h"

/** Per-thread flag set by upc_notify() and cleared by upc_wait() */
static int gupcr_barrier_active = 0;

/** Max barrier ID used by the barrier implementation.
 * The FI_MIN atomic function is used by
 * each thread to report its barrier ID to its parents.
 * The MAX barrier ID value is used to initialize the memory
 * location targeted by FI_MIN function.
 */
#define BARRIER_ID_MAX INT_MAX
/** Anonymous barrier ID used by the barrier implementation.
 * This barrier ID is used for barrier statements that do not
 * specify a barrier ID and it matches any other barrier ID.
 */
#define BARRIER_ANONYMOUS INT_MIN
/** Size of the barrier ID */
#define BARRIER_ID_SIZE (sizeof (barreg.wait))

/** Leaf thread check */
#define LEAF_THREAD  ((THREADS != 1) && (gupcr_child_cnt == 0))
/** Root thread check */
#define ROOT_THREAD  (gupcr_parent_thread == -1)
/** Inner thread check */
#define INNER_THREAD ((gupcr_child_cnt != 0) && (gupcr_parent_thread != -1))

/** Thread's current barrier ID */
int gupcr_barrier_id;

/** Barrier memory region  */
static struct
{
  /** Thread's barrier ID for upstream push */
  int value;
  /** Maximum barrier ID used to re-initialize notify barrier ID.  */
  int value_max;
  /** Min value barrier ID among thread and its children */
  int notify;
  /** notify_completion signal field */
  int notify_signal;
  /** Consensus barrier ID among all threads */
  int wait;
  /** Wait completion signal field */
  int wait_signal;
  /** Broadcast signal location.  */
  int bcast_signal;
  /** Broadcast received value memory buffer.  */
  char bcast[GUPCR_MAX_BROADCAST_SIZE];
} barreg;

/* Trigger events.  */
#define NOTIFY_EVENT 0		/* Value written into notify filed.  */
#define WAIT_EVENT 1		/* Value written into wait field.  */

/**
 * @fn __upc_notify (int barrier_id)
 * UPC <i>upc_notify<i> statement implementation
 *
 * This procedure sets the necessary triggers to implement
 * the pass that derives a consensus barrier ID value across all
 * UPC threads.  The inner threads use triggered operations
 * to pass the barrier ID negotiated among itself and its children
 * up the tree its parent.
 * @param [in] barrier_id Barrier ID
 */
void
__upc_notify (int barrier_id)
{
  gupcr_trace (FC_BARRIER, "BARRIER NOTIFY ENTER %d", barrier_id);

  if (gupcr_barrier_active)
    gupcr_error ("two successive upc_notify statements executed "
		 "without an intervening upc_wait");
  gupcr_barrier_active = 1;
  gupcr_barrier_id = barrier_id;

  /* The UPC shared memory consistency model requires all outstanding
     read/write operations to complete on the thread's
     current synchronization phase.  */
  gupcr_gmem_sync ();

#if NOT_NOW_GUPCR_USE_TRIGGERED_OPS
  if (THREADS == 1)
    return;

  /* Use barrier MAX number if barrier ID is "match all"
     This effectively excludes the thread from setting the min ID
     among the threads.  */
  barreg.wait = (barrier_id == BARRIER_ANONYMOUS) ?
    BARRIER_ID_MAX : barrier_id;

  if (LEAF_THREAD)
    {
      /* Send the barrier ID to the parent - use atomic FI_MIN to allow
         parent to find the minimum barrier ID among itself and its
         children.  */
      gupcr_debug (FC_BARRIER, "Send atomic FI_MIN %d to (%d)",
		   barreg.wait, gupcr_parent_thread);
      gupcr_barrier_notify_put (&barreg.wait, gupcr_parent_thread,
				&barreg.notify);
    }
  else
    {
      int i;
      if (ROOT_THREAD)
	{
	  /* The consensus MIN barrier ID derived in the notify (UP) phase
	     must be transferred to the wait for delivery to all children.
	     Trigger: Barrier ID received in the notify phase.
	     Action: Send the barrier ID to the wait buffer of the
	     barrier DOWN LE.  */
	  gupcr_barrier_tr_put (&barreg.notify, MYTHREAD, &barreg.wait,
				sizeof (barreg.notify), gupcr_child_cnt + 1,
				NOTIFY_EVENT);
	}
      else
	{
	  /* The consensus MIN barrier ID of the inner thread and
	     its children is sent to the parent UPC thread.
	     Trigger: All children and this thread execute an
	     atomic FIL_MIN using each thread's UP NC.
	     Action: Transfer the consensus minimum barrier ID to the
	     this thread's parent.  */
	  gupcr_barrier_notify_tr_put (&barreg.notify, gupcr_parent_thread,
				       &barreg.notify, gupcr_child_cnt + 1);
	}

      /* Trigger: Barrier ID received in the wait buffer.
         Action: Reinitialize the barrier UP ID to barrier MAX value
         for the next call to upc_notify.  */
      gupcr_barrier_tr_put (&barreg.value_max, MYTHREAD, &barreg.notify,
			    sizeof (barreg.notify), 1, WAIT_EVENT);

      /* Trigger: The barrier notify ID is reinitialized to MAX.
         Action: Send the consensus barrier ID to all children.  */
      for (i = 0; i < gupcr_child_cnt; i++)
	{
	  gupcr_barrier_tr_put (&barreg.wait, gupcr_child[i], &barreg.wait,
				sizeof (barreg.wait), 1, NOTIFY_EVENT);
	}

      /* Allow notify to proceed and to possibly complete the wait
         phase on other threads.  */

      /* Find the minimum barrier ID among children and the root.  */
      gupcr_debug (FC_BARRIER, "Send atomic FI_MIN %d to (%d)",
		   barreg.wait, MYTHREAD);
      gupcr_barrier_notify_put (&barreg.wait, MYTHREAD, &barreg.notify);
    }
#else
  /* The UPC runtime barrier implementation that does not use
     fabric triggered operations does not support split phase barriers.
     In this case, all Portals actions related to the barrier
     are performed in the __upc_wait() function.  */
#endif
  gupcr_trace (FC_BARRIER, "BARRIER NOTIFY EXIT %d", barrier_id);
}

/**
 * @fn __upc_wait (int barrier_id)
 * UPC <i>upc_wait</i> statement implementation
 *
 * This procedure waits to receive the derived consensus
 * barrier ID from the parent (leaf thread) or acknowledges that
 * all children received the consensus barrier ID (inner
 * and root threads).  The consensus barrier ID is checked
 * against the barrier ID passed in as an argument.
 * @param [in] barrier_id Barrier ID
 */
void
__upc_wait (int barrier_id)
{
  gupcr_trace (FC_BARRIER, "BARRIER WAIT ENTER %d", barrier_id);

  if (!gupcr_barrier_active)
    gupcr_error ("upc_wait statement executed without a "
		 "preceding upc_notify");

  /* Check if notify/wait barrier IDs match.
     BARRIER_ANONYMOUS matches any other barrier ID.  */
  if ((barrier_id != BARRIER_ANONYMOUS &&
       gupcr_barrier_id != BARRIER_ANONYMOUS) &&
      (gupcr_barrier_id != barrier_id))
    {
      gupcr_error ("UPC barrier identifier mismatch - notify %d, wait %d",
		   gupcr_barrier_id, barrier_id);
    }

  if (THREADS == 1)
    {
      gupcr_barrier_active = 0;
      return;
    }

#if NOT_NOW_GUPCR_USE_TRIGGERED_OPS
  /* Wait for the barrier ID to propagate down the tree.  */
  if (LEAF_THREAD)
    {
      gupcr_barrier_wait_event ();
    }
  else
    {
      /* Wait for the barrier ID to flow down to the children.  */
      /* This is needed so inner nodes don't start a new barrier before
         leaf receives its wait barrier id.  */
      gupcr_barrier_wait_put_completion ();
    }
#else
  /* UPC Barrier implementation without Triggered Functions.
     Split phase is not supported and barrier id going up and
     down happens during the call to wait.  */

  /* Propagate minimal barrier ID to the root thread.  */

  /* Use the barrier maximum ID number if the barrier ID is "match all".
     This effectively excludes the thread from setting the minimum ID
     among the threads.  */
  barreg.value = (barrier_id == BARRIER_ANONYMOUS) ?
    BARRIER_ID_MAX : barrier_id;

  if (LEAF_THREAD)
    {
      /* Send barrier ID up the tree.  */
      gupcr_debug (FC_BARRIER, "Send atomic FI_MIN %d to (%d)",
		   barreg.wait, gupcr_parent_thread);
      gupcr_barrier_notify_put (&barreg.value,
				gupcr_parent_thread, &barreg.notify);
      /* Wait for tree agreed on barrier ID.  */
      gupcr_barrier_wait_event ();
    }
  else
    {
      int i;
      /* Parent participates in FI_MIN atomic with children.  */
      gupcr_debug (FC_BARRIER, "Send atomic FI_MIN %d to (%d)",
		   barreg.wait, MYTHREAD);
      gupcr_barrier_notify_put (&barreg.value, MYTHREAD, &barreg.notify);

      /* Wait for children threads to report their barrier IDs.
         Account for this thread's atomic FI_MIN in its own space.  */
      gupcr_barrier_notify_event (gupcr_child_cnt + 1);

      if (INNER_THREAD)
	{
	  /* Send the barrier ID to the parent - use atomic FI_MIN on the value
	     in the parents notify LE (derived minimal ID for the parent and its
	     children.  */
	  gupcr_debug (FC_BARRIER, "Send atomic FI_MIN %d to (%d)",
		       barreg.wait, gupcr_parent_thread);
	  gupcr_barrier_notify_put (&barreg.notify,
				    gupcr_parent_thread, &barreg.notify);
	  /* Wait for agreed on barrier ID.  */
	  gupcr_barrier_wait_event ();
	}
      else
	{
	  /* ROOT - agreed barrier ID arrived at the top.  */
	  gupcr_debug (FC_BARRIER, "root: barrier ID at %lx := %d",
		       (unsigned long) &barreg.notify, barreg.notify);
	  barreg.wait = barreg.notify;
	}

      /* Re-initialize the barrier ID maximum range value.  */
      barreg.notify = barreg.value_max;

      /* Send the agreed barrier ID to children.  */
      for (i = 0; i < gupcr_child_cnt; i++)
	{
	  gupcr_barrier_wait_put (&barreg.wait, gupcr_child[i], &barreg.wait,
				  sizeof (barreg.wait));
	}
      gupcr_barrier_wait_put_completion ();
    }

#endif /* GUPCR_USE_TRIGGERED_OPS */

  /* Verify that the barrier ID matches.  */
  if (barrier_id != INT_MIN &&
      barrier_id != barreg.wait && barreg.wait != BARRIER_ID_MAX)
    gupcr_error ("thread %d: UPC barrier identifier mismatch among threads - "
		 "expected %d, received %d",
		 MYTHREAD, barrier_id, barreg.wait);

  /* UPC Shared Memory Consistency Model requires all outstanding
     read/write operations to complete on the thread's enter
     into the next synchronization phase.  */
  gupcr_gmem_sync ();

  gupcr_barrier_active = 0;
  gupcr_trace (FC_BARRIER, "BARRIER WAIT EXIT %d", barrier_id);
}

/**
 * @fn __upc_barrier (int barrier_id)
 * UPC language upc_barrier implementation.
 *
 * @param [in] barrier_id Barrier ID
 */
void
__upc_barrier (int barrier_id)
{
  __upc_notify (barrier_id);
  __upc_wait (barrier_id);
}

/* This broadcast implementation uses barrier resources
 * to pass the broadcast message from thread 0 to all other threads.  */

/**
 * @fn gupcr_bcast_send (void *value, size_t nbytes)
 * Send broadcast message to all thread's children.
 *
 * The broadcast is a collective operation where thread 0 (root thread)
 * sends a message to all other threads.  This function must be
 * called by the thread 0 only from a public function
 * "gupcr_broadcast_put".
 *
 * @param [in] value Pointer to send value
 * @param [in] nbytes Number of bytes to send
 * @ingroup BROADCAST
 */
void
gupcr_bcast_send (void *value, size_t nbytes)
{
  int i;

  gupcr_trace (FC_BROADCAST, "BROADCAST SEND ENTER 0x%lx %lu",
	       (long unsigned) value, (long unsigned) nbytes);
  gupcr_assert (nbytes <= sizeof (barreg.bcast));

  /* This broadcast operation is implemented as a collective operation.
     Before proceeding, complete all outstanding shared memory
     read/write operations.  */
  gupcr_gmem_sync ();
  /* Initialize the buffer used for delivery to children.  */
  memcpy (barreg.bcast, value, nbytes);

  /* Wait for all children to arrive.  */
  gupcr_barrier_notify_event (gupcr_child_cnt);

  /* Send broadcast to children threads.  */
  for (i = 0; i < gupcr_child_cnt; i++)
    {
      gupcr_debug (FC_BROADCAST, "Send broadcast message to child (%d)",
		   gupcr_child[i]);
      gupcr_barrier_wait_put (barreg.bcast, gupcr_child[i],
			      barreg.bcast, nbytes);
    }

  /* Wait for message delivery to all children.  This ensures that
     the source buffer is not overwritten by back-to-back
     broadcast operations.  */
  gupcr_barrier_wait_put_completion ();

  gupcr_trace (FC_BROADCAST, "BROADCAST SEND EXIT");
}

/**
 * @fn gupcr_bcast_recv (void *value, size_t nbytes)
 * Wait to receive the broadcast message and return its value.
 *
 * Broadcast is a collective operation where thread 0 (the root thread)
 * sends a message to all other threads.  This function must be
 * called by every thread other then thread 0.
 *
 * @param [in] value Pointer to received value
 * @param [in] nbytes Number of bytes to receive
 * @ingroup BROADCAST
 */
void
gupcr_bcast_recv (void *value, size_t nbytes)
{
  int i;

  gupcr_trace (FC_BROADCAST, "BROADCAST RECV ENTER 0x%lx %lu",
	       (long unsigned) value, (long unsigned) nbytes);

  gupcr_gmem_sync ();

#if NOT_NOW_GUPCR_USE_TRIGGERED_OPS
  if (INNER_THREAD)
    {
      /* Prepare triggers for message push to all children.  */
      /* Trigger: message received from the parent.
         Action: send message to a child.  */
      for (i = 0; i < gupcr_child_cnt; i++)
	{
	  gupcr_debug (FC_BROADCAST,
		       "Set put trigger to the child (%d)", gupcr_child[i]);
	  gupcr_barrier_tr_put (barreg.bcast, gupcr_child[i], barreg.bcast,
				nbytes, 1, WAIT_EVENT);
	}

      /* Prepare a trigger to send notification to the parent.  */
      gupcr_debug (FC_BROADCAST,
		   "Set notification trigger to the parent (%d)",
		   gupcr_parent_thread);
      /* Trigger: message received from children.
         Action: send message to the parent.  */
      gupcr_barrier_notify_tr_put (&barreg.bcast_signal,
				   gupcr_parent_thread, &barreg.bcast_signal,
				   gupcr_child_cnt);
      /* Wait for delivery to all children.  */
      gupcr_barrier_wait_put_completion ();
    }
  else
    {
      /* A leaf thread sends notification to its parent that
         it is ready to receive the broadcast value.  */
      gupcr_debug (FC_BROADCAST, "Send notification to the parent (%d)",
		   gupcr_parent_thread);
      gupcr_barrier_notify_put (&barreg.bcast_signal, gupcr_parent_thread,
				&barreg.bcast_signal);

      /* Wait to receive a message from the parent.  */
      gupcr_barrier_wait_event ();
    }
  memcpy (value, barreg.bcast, nbytes);
#else
  /* Inner thread must wait for children threads to arrive.  */
  if (INNER_THREAD)
    {
      gupcr_debug (FC_BROADCAST, "Waiting for %d notifications",
		   gupcr_child_cnt);
      gupcr_barrier_notify_event (gupcr_child_cnt);
    }

  /* Inform the parent that this thread and all its children arrived.
     Send barrier MAX value as we share PTEs with the barrier
     implementation.  */
  gupcr_debug (FC_BROADCAST, "Send notification to the parent %d",
	       gupcr_parent_thread);
  gupcr_barrier_notify_put (&barreg.bcast_signal, gupcr_parent_thread,
			    &barreg.bcast_signal);

  /* Receive the broadcast message from the parent.  */
  gupcr_barrier_wait_event ();

  /* Copy out the received message.  */
  memcpy (value, barreg.bcast, nbytes);

  if (INNER_THREAD)
    {
      /* An inner thread must pass the message to its children.  */
      for (i = 0; i < gupcr_child_cnt; i++)
	{
	  gupcr_debug (FC_BROADCAST, "Sending a message to %d",
		       gupcr_child[i]);
	  gupcr_barrier_wait_put (barreg.bcast, gupcr_child[i],
				  barreg.bcast, nbytes);
	}
      /* Wait for delivery to all children.  */
      gupcr_barrier_wait_put_completion ();
    }
#endif
  gupcr_trace (FC_BROADCAST, "BROADCAST RECV EXIT");
}

/**
 * @fn gupcr_barrier_init (void)
 * Initialize barrier resources.
 * @ingroup INIT
 */
void
gupcr_barrier_init (void)
{
  gupcr_log (FC_BARRIER, "barrier init called");
  barreg.value_max = BARRIER_ID_MAX;
  barreg.notify = BARRIER_ID_MAX;
  barreg.notify_signal = 0;
  barreg.wait_signal = 0;
  gupcr_barrier_sup_init (&barreg, sizeof barreg);
}

/**
 * @fn gupcr_barrier_fini (void)
 * Release barrier resources.
 * @ingroup INIT
 */
void
gupcr_barrier_fini (void)
{
  gupcr_log (FC_BARRIER, "barrier fini called");
  gupcr_barrier_sup_fini ();
  gupcr_log (FC_BARRIER, "barrier fini completed");
}

/** @} */
