/*===-- gupcr_coll_sup.c - UPC Runtime Support Library -------------------===
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
#include "gupcr_iface.h"
#include "gupcr_utils.h"
#include "gupcr_coll_sup.h"

/**
 * @file gupcr_coll_sup.c
 * GUPC Libfabric collectives implementation support routines.
 *
 * @addtogroup COLLECTIVES GUPCR Collectives Functions
 * @{
 */

/** Collectives number of received puts on NC */
static size_t gupcr_coll_signal_cnt;

/** Collectives number of received ACKs on local MR */
static size_t gupcr_coll_ack_cnt;

/* Collectives thread tree.  */
/** Collectives tree parent thread */
int gupcr_coll_parent_thread;
/** Collectives tree number of children */
int gupcr_coll_child_cnt;
/** Collectives tree child's index */
int gupcr_coll_child_index;
/** Collectives tree children threads */
int gupcr_coll_child[GUPCR_TREE_FANOUT];

/**
 * Initialize collectives thread tree.
 *
 * A collectives tree starts from the "start" thread number and
 * includes only "nthreads" (e.g. threads involved in
 * the reduce process).  The simplest case is when all the
 * threads are involved in which case start=0 and
 * nthreads=THREADS (e.g. used for broadcast).
 *
 * The collectives thread tree can be organized in a
 * form where the "newroot" value identitifies
 * the root thread (only if the "newroot" thread
 * is participating in the operation).
 * @param [in] newroot A hint for the tree root thread.
 * @param [in] start Start thread for reduce
 * @param [in] nthreads Number of threads participating
 *
 */
void
gupcr_coll_tree_setup (size_t newroot, size_t start, int nthreads)
{
/* Convert from/to 0-(THREADS-1) to start-(nthreads-1) range.  */
#define NEWID(id,first) ((id - first + THREADS) % THREADS)
#define OLDID(nid,first) ((nid + first) % THREADS)

/* Remap into the new root (from root 0 to "root").  */
#define NEWIDROOT(id,top,cnt) ((cnt + id - top) % cnt)
#define OLDIDROOT(nid,top,cnt) ((nid + top) % cnt)
  int i;
  int ok_to_root = 0;
  int myid;
  int root = NEWID (newroot, start);

  gupcr_debug (FC_COLL, "newroot: %lu, start: %lu nthreads: %d",
	       (long unsigned) newroot, (long unsigned) start, nthreads);

  /* Check if root node is participating.  If yes, use that for the
     root, otherwise 0.  */
  if (root < nthreads)
    ok_to_root = 1;

  /* Get myid - first convert into the new range (0-nthreads),
     then, if needed and possible, into the range where newroot becomes 0.  */
  myid = NEWID (MYTHREAD, start);
  if (ok_to_root)
    myid = NEWIDROOT (myid, root, nthreads);

  /* Calculate the thread id's of the children and parent.  */
  gupcr_coll_child_cnt = 0;
  for (i = 0; i < GUPCR_TREE_FANOUT; i++)
    {
      int child = (GUPCR_TREE_FANOUT * myid + i + 1);
      if (child < nthreads)
	{
	  ++gupcr_coll_child_cnt;
	  if (ok_to_root)
	    child = OLDIDROOT (child, root, nthreads);
	  gupcr_coll_child[i] = OLDID (child, start);
	}
    }
  if (myid)
    {
      gupcr_coll_parent_thread = (myid - 1) / GUPCR_TREE_FANOUT;
      gupcr_coll_child_index =
	myid - gupcr_coll_parent_thread * GUPCR_TREE_FANOUT - 1;
      if (ok_to_root)
	gupcr_coll_parent_thread =
	  OLDIDROOT (gupcr_coll_parent_thread, root, nthreads);
      gupcr_coll_parent_thread = OLDID (gupcr_coll_parent_thread, start);
    }
  else
    gupcr_coll_parent_thread = ROOT_PARENT;
}

/**
 * Collective PUT operation
 *
 * @param [in] dthread Destination thread
 * @param [in] doffset Destination offset in the shared space
 * @param [in] soffset Source offset in the shared space
 * @param [in] nbytes Number of bytes to copy
 */

void
gupcr_coll_put (size_t dthread, size_t doffset, size_t soffset, size_t nbytes)
{
  gupcr_debug (FC_COLL, "%d:0x%lx %lu:0x%lx %lu",
	       MYTHREAD, (long unsigned) soffset,
	       (long unsigned) dthread, (long unsigned) doffset,
	       (long unsigned) nbytes);
}

/**
 * Collective triggered PUT operation
 *
 * Schedule put operation once number of signals reaches
 * the specified value.
 *
 * @param [in] dthread Destination thread
 * @param [in] doffset Destination offset in the shared space
 * @param [in] soffset Source offset in the shared space
 * @param [in] nbytes Number of bytes to copy
 * @param [in] cnt Trigger count
 */
void
gupcr_coll_trigput (size_t dthread, size_t doffset, size_t soffset,
		    size_t nbytes, size_t cnt)
{
  gupcr_debug (FC_COLL, "%d:0x%lx -> %lu:0x%lx %lu trig %lu",
	       MYTHREAD, (long unsigned) soffset,
	       (long unsigned) dthread, (long unsigned) doffset,
	       (long unsigned) nbytes, (long unsigned) cnt);
}

/**
 * Collective atomic PUT operation.
 *
 * @param [in] dthread Destination thread
 * @param [in] doffset Destination offset in the shared space
 * @param [in] soffset Source offset in the shared space
 * @param [in] nbytes Number of bytes to copy
 * @param [in] op atomic operation
 * @param [in] datatype atomic data type
 */

void
gupcr_coll_put_atomic (size_t dthread, size_t doffset, size_t soffset,
		       size_t nbytes, enum fi_op op, enum fi_datatype datatype)
{
  gupcr_debug (FC_COLL, "%d:0x%lx %lu:0x%lx %lu %s %s",
	       MYTHREAD, (long unsigned) soffset,
	       (long unsigned) dthread, (long unsigned) doffset,
	       (long unsigned) nbytes,
	       gupcr_strop (op), gupcr_strdatatype (datatype));
}

/**
 * Collective triggered atomic PUT operation.
 *
 * Schedule atomic put operation once number of signals reaches
 * the specified value.
 *
 * @param [in] dthread Destination thread
 * @param [in] doffset Destination offset in the shared space
 * @param [in] soffset Source offset in the shared space
 * @param [in] nbytes Number of bytes to copy
 * @param [in] op atomic operation
 * @param [in] datatype atomic data type
 * @param [in] cnt Number of signals that triggers
 */
void
gupcr_coll_trigput_atomic (size_t dthread, size_t doffset, size_t soffset,
			   size_t nbytes, enum fi_op op,
			   enum fi_datatype datatype, size_t cnt)
{
  gupcr_debug (FC_COLL, "%d:0x%lx %lu:0x%lx %lu %s %s trig %lu",
	       MYTHREAD, (long unsigned) soffset,
	       (long unsigned) dthread, (long unsigned) doffset,
	       (long unsigned) nbytes,
	       gupcr_strop (op), gupcr_strdatatype (datatype),
	       (long unsigned) cnt);
}

/**
 * Collectives wait for operation completion
 * This function is used in cases where threads needs to wait
 * for the completion of remote operations.
 *
 * @param [in] cnt Wait count
 */
void
gupcr_coll_ack_wait (size_t cnt)
{
  gupcr_debug (FC_COLL, "wait for %lu (%lu)",
               (long unsigned) cnt,
	       (long unsigned) (gupcr_coll_ack_cnt + cnt));
  gupcr_coll_ack_cnt += cnt;
}

/**
 * Collectives wait for signaling events
 * This function is used to wait for other threads to complete
 * operations in the thread's shared space (e.g. children performing
 * atomic ops in the parent's shared space).
 *
 * @param [in] cnt Wait count
 */
void
gupcr_coll_signal_wait (size_t cnt)
{
  gupcr_debug (FC_COLL, "wait for %lu (%lu)",
	       (long unsigned) cnt,
	       (long unsigned) (gupcr_coll_signal_cnt + cnt));
  gupcr_coll_signal_cnt += cnt;
}

/**
 * Initialize collectives resources.
 * @ingroup INIT
 *
 * A thread's shared space is mapped via a NC for other
 * threads to write to, and an MR as a source for remote
 * operations.  In this way, the address filed of the shared pointer
 * can be used as an offset into NC/MR.
 */
void
gupcr_coll_init (void)
{
  gupcr_log (FC_COLL, "coll init called");

  /* Reset the number of signals/acks.  */
  gupcr_coll_signal_cnt = 0;
  gupcr_coll_ack_cnt = 0;
}

/**
 * Release collectives resources.
 * @ingroup INIT
 */
void
gupcr_coll_fini (void)
{
  gupcr_log (FC_COLL, "coll fini called");
}

/** @} */
