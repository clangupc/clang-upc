/*===-- gupcr_shutdown.c - UPC Runtime Support Library -------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_shutdown.c
 * GUPC Libfabric shutdown support.
 *
 * Support for upc_global_exit ().
 *
 * Each UPC thread (process) creates a helper (shutdown) pthread
 * with the sole purpose of waiting for receipt of a remote request
 * to shutdown, as a result of the other thread issuing a call
 * to upc_global_exit.
 *
 * This pthread uses a special MC (GUPCR_SHUTDOWN) to receive a
 * global exit code from another UPC thread.  A simple PtlPut of the
 * exit code issued to the shutdown NC on some other UPC thread
 * triggers exit of the receiving thread.
 *
 * The following steps are taken to initialize, wait, and signal
 * the UPC global exit:
 *
 * - Each thread initializes a NC to receive an exit code that
 *     was passed in as the argument to upc_global_exit().
 * - Each thread creates a helper pthread - gupcr_shutdown_pthread()
 *     that waits on the shutdown NC's counting event (one count only).
 * - The main UPC thread installs a signal handler for SHUTDWON_SIGNAL
 *     that is used by the shutdown pthread to signal a need for
 *     global exit.
 * - Remote shutdown takes the following steps:
 *     -# A UPC thread executing a call to upc_global_exit() sends the
 *        exit code to all other UPC threads by using the shutdown PTE.
 *     -# The pthread associated with each UPC thread receives the
 *        exit code and returns from the counting event wait call.
 *     -# The receiving pthread sends the SHUTDOWN_SIGNAL to the main
 *     	  UPC thread and calls pthread_exit().
 *     -# The main UPC thread receives the signal, which invokes
 *        the signal handler.
 *     -# The signal handler waits for the shutdown pthread to exit,
 *        and then calls exit() with the code received from
 *        the thread that sent the shutdown request.  The invoking thread
 *        also waits for ACKs from the first step with the configured timeout,
 *
 *  @note
 *  -# The gupcr_exit() function is registered with atexit()
 *     and will be executed when exit() is called.
 *  -# Upon regular exit, the main UPC thread disables the
 *     SHUTDOWN_SIGNAL signal, and terminates the shutdown pthread
 *     by writing a dummy value using its own shutdown NC.
 *
 * @addtogroup SHUTDOWN GUPCR Shutdown Functions
 * @{
 */

#include <pthread.h>
#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_utils.h"
#include "gupcr_fabric.h"
#include "gupcr_shutdown.h"
#include "gupcr_iface.h"

/** Shutdown signal to main thread */
#define SHUTDOWN_SIGNAL SIGUSR2
/** Shutdown check interval (100 miliseconds) */
#define SHUTDOWN_MICROSEC_WAIT 100000L

/** Index of the local memory location */
#define GUPCR_LOCAL_INDEX(addr) \
	(void *) ((char *) addr - (char *) USER_PROG_MEM_START)

/** Shutdown NC buffer */
static int gupcr_shutdown_status;

/** Shutdown memory buffer for sending data */
static int gupcr_shutdown_send_status;

/** Shutdown pthread ID */
static pthread_t gupcr_shutdown_pthread_id;
/** Shutdown pthread declaration */
static void *gupcr_shutdown_pthread (void *arg) __attribute__ ((noreturn));

/** Shutdown memory region counter */
static size_t gupcr_shutdown_lmr_count;
/** Shutdown memory remote access counter */
static size_t gupcr_shutdown_mr_count;

/** Fabric communications endpoint resources */
/** RX Endpoint comm resource  */
fab_ep_t gupcr_shutdown_rx_ep;
/** TX Endpoint comm resource  */
fab_ep_t gupcr_shutdown_tx_ep;
/** Completion remote counter (target side) */
fab_cntr_t gupcr_shutdown_ct;
/** Completion counter */
fab_cntr_t gupcr_shutdown_lct;
/** Completion remote queue (target side) */
fab_cq_t gupcr_shutdown_cq;
/** Completion queue for errors  */
fab_cq_t gupcr_shutdown_lcq;
/** Remote access memory region  */
fab_mr_t gupcr_shutdown_mr;
/** Local memory access memory region  */
fab_mr_t gupcr_shutdown_lmr;

/**
 * Send a remote shutdown request to all threads.
 *
 * Wait for the thread's pthread and ACks from sending
 * messages to other threads (with timeout).
 *
 * @param [in] status exit code passed to other threads
 */
void
gupcr_signal_exit (int status)
{
  int thread;
  int wait_cnt = GUPCR_GLOBAL_EXIT_TIMEOUT *
    (1000000L / SHUTDOWN_MICROSEC_WAIT);
  int done = 0;

  /* Protect local global exit from remote shutdown request.  */
  gupcr_signal_disable (SHUTDOWN_SIGNAL);

  gupcr_shutdown_send_status = status;
  /* Send global exit code to all threads.  */
  for (thread = 0; thread < THREADS; thread++)
    {
      gupcr_fabric_call (fi_writeto,
			 (gupcr_shutdown_tx_ep,
			  GUPCR_LOCAL_INDEX (&gupcr_shutdown_send_status),
			  sizeof (gupcr_shutdown_send_status),
			  NULL, fi_rx_addr ((fi_addr_t) thread,
					    GUPCR_SERVICE_SHUTDOWN,
					    GUPCR_SERVICE_BITS),
			  (uint64_t) GUPCR_LOCAL_INDEX (&gupcr_shutdown_send_status), 0,
			  NULL));
    }
  /* It is NOT ok to call finalize routines as there might
     be outstanding transactions.  */
  gupcr_finalize_ok = 0;
  /* Wait for our own shutdown pthread to complete.  */
  pthread_join (gupcr_shutdown_pthread_id, NULL);
  /* Wait for ACKs from all threads.  It should happen quickly
     if everything is ok, otherwise timeout after configured
     number of seconds.  */
  do
    {
      uint64_t cnt;
      gupcr_fabric_call_nc (fi_cntr_read, cnt, (gupcr_shutdown_ct));
      // Check that we received all acks
      if (cnt == (uint64_t) THREADS)
	done = 1;
      else
	gupcr_cpu_delay (SHUTDOWN_MICROSEC_WAIT);
    }
  while (!done && wait_cnt--);
}

/**
 * Terminate shutdown pthread
 *
 * To terminate the local shutdown pthread a dummy value must
 * be sent by this thread to its own shutdown PTE.  The main thread
 * then waits for pthread to exit.
 */
static void
gupcr_shutdown_terminate_pthread (void)
{
  /* Disable interrupts before sending a signal to
     shutdown pthread.  */
  gupcr_signal_disable (SHUTDOWN_SIGNAL);

  gupcr_shutdown_send_status = 0;
  gupcr_fabric_call (fi_writeto,
		     (gupcr_shutdown_tx_ep,
		      GUPCR_LOCAL_INDEX (&gupcr_shutdown_send_status),
		      sizeof (gupcr_shutdown_send_status),
		      NULL, fi_rx_addr ((fi_addr_t) MYTHREAD,
					GUPCR_SERVICE_SHUTDOWN,
					GUPCR_SERVICE_BITS),
		      (uint64_t) GUPCR_LOCAL_INDEX (&gupcr_shutdown_send_status), 0,
		      NULL));
  gupcr_shutdown_lmr_count += 1;
  pthread_join (gupcr_shutdown_pthread_id, NULL);
}

/**
 * Shutdown pthread that waits for remote shutdown requests.
 *
 * This pthread waits on a shutdown PTE to receive a shutdown
 * request from any other thread that executed upc_global_exit().
 * Then, it uses signal (SHUTDOWN_SIGNAL) to inform the main thread
 * of a need to shutdown this UPC thread.
 * @param [in] arg Pthread arguments (not used in this case)
 * @retval Pthread's exit value
 */
static void *
gupcr_shutdown_pthread (void *arg __attribute ((unused)))
{
  uint64_t cnt;

  gupcr_log (FC_MISC, "Shutdown pthread started");
  /* Wait for the shutdown request.  Yield control of the
     CPU frequently as this is a low priority activity
     that should minimize competition for resources with the
     main thread.  */
  do
    {
      gupcr_cpu_delay (SHUTDOWN_MICROSEC_WAIT);
      gupcr_fabric_call_nc (fi_cntr_read, cnt, (gupcr_shutdown_ct));
    }
  while (!cnt);
  gupcr_debug (FC_MISC, "Shutdown pthread received exit %d",
	       gupcr_shutdown_status);

  /* Signal the main thread to exit.  */
  kill (getpid (), SHUTDOWN_SIGNAL);
  /* No need for this helper pthread any more.  */
  pthread_exit (NULL);
}

/**
 * Signal handler that performs shutdown.
 *
 * This signal handler will terminate the pthread
 * that listens for shutdown requests and then
 * exit the current process (which is associated with
 * a UPC thread).  The UPC will exit with the
 * the status code received from the remote thread.
 * @param [in] signum Signal number (unused)
 */
void
gupcr_shutdown_signal_handler (int signum __attribute__ ((unused)))
{
  gupcr_debug (FC_MISC, "Shutdown signal handler for signal %d", signum);
  /* Wait for shutdown pthread to exit.  */
  pthread_join (gupcr_shutdown_pthread_id, NULL);
  /* It is NOT ok to call finalize routines as there might
     be outstanding transactions.  */
  gupcr_finalize_ok = 0;
  /* Exit with global exit code.  */
  exit (gupcr_shutdown_status);
}

/**
 * Initialize remote shutdown resources.
 * @ingroup INIT
 */
void
gupcr_shutdown_init (void)
{
  cntr_attr_t cntr_attr = { 0 };
  cq_attr_t cq_attr = { 0 };
  tx_attr_t tx_attr = { 0 };

  gupcr_log (FC_MISC, "shutdown init called");

  /* Create context endpoints for shutdown signalling.  */
  tx_attr.op_flags = FI_REMOTE_COMPLETE;
  gupcr_fabric_call (fi_tx_context,
		     (gupcr_ep, GUPCR_SERVICE_SHUTDOWN, &tx_attr, &gupcr_shutdown_tx_ep,
		      NULL));
  gupcr_fabric_call (fi_rx_context,
		     (gupcr_ep, GUPCR_SERVICE_SHUTDOWN, NULL, &gupcr_shutdown_rx_ep,
		      NULL));

  /* ... and completion counter/eq for remote read/write.  */
  cntr_attr.events = FI_CNTR_EVENTS_COMP;
  cntr_attr.flags = 0;
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr,
				    &gupcr_shutdown_lct, NULL));
  gupcr_fabric_call (fi_bind, (&gupcr_shutdown_tx_ep->fid,
			       &gupcr_shutdown_lct->fid, FI_READ | FI_WRITE));

  /* ... and completion queue for remote target transfer errors.  */
  cq_attr.size = 1;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr, &gupcr_shutdown_lcq, NULL));
  /* Use FI_EVENT flag to report errors only.  */
  gupcr_fabric_call (fi_bind, (&gupcr_shutdown_tx_ep->fid,
			       &gupcr_shutdown_lcq->fid,
			       FI_READ | FI_WRITE | FI_EVENT));

  /* NOTE: Create a local memory region before enabling endpoint.  */
  /* ... and memory region for local memory accesses.  */
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, USER_PROG_MEM_START,
				 USER_PROG_MEM_SIZE, FI_READ | FI_WRITE,
				 0, 0, 0, &gupcr_shutdown_lmr, NULL));
  /* NOTE: There is no need to bind local memory region to endpoint.  */
  /*       Hmm ... ? We can probably use only one throughout the runtime,  */
  /*       as counters and events are bound to endpoint.  */
#if 0
  gupcr_fabric_call (fi_bind, (&gupcr_shutdown_tx_ep->fid,
			       &gupcr_shutdown_lmr->fid, FI_READ | FI_WRITE));
#endif

  /* Enable endpoints.  */
  gupcr_fabric_call (fi_enable, (gupcr_shutdown_tx_ep));
  gupcr_fabric_call (fi_enable, (gupcr_shutdown_rx_ep));

  /* ... and memory region for remote inbound accesses.  */
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, gupcr_gmem_base, gupcr_gmem_size,
				 FI_REMOTE_READ | FI_REMOTE_WRITE, 0,
				 0, 0, &gupcr_shutdown_mr, NULL));
  /* ... and counter for remote inbound writes.  */
  cntr_attr.events = FI_CNTR_EVENTS_COMP;
  cntr_attr.flags = 0;
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr,
				    &gupcr_shutdown_ct, NULL));
  gupcr_fabric_call (fi_bind, (&gupcr_shutdown_mr->fid,
			       &gupcr_shutdown_ct->fid, FI_REMOTE_WRITE));
  /* ... and completion queue for remote inbound errors.  */
  cq_attr.size = 1;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr, &gupcr_shutdown_cq, NULL));
  gupcr_fabric_call (fi_bind, (&gupcr_shutdown_mr->fid,
			       &gupcr_shutdown_cq->fid,
			       FI_REMOTE_WRITE | FI_EVENT));
  /* ... local/remote transaction counts.  */
  gupcr_shutdown_lmr_count = 0;
  gupcr_shutdown_mr_count = 0;

  /* ... bind MR to endpoint.  */
  gupcr_fabric_call (fi_bind, (&gupcr_shutdown_rx_ep->fid,
			       &gupcr_shutdown_mr->fid,
			       FI_REMOTE_READ | FI_REMOTE_WRITE));

  /* Start a pthread that listens for remote shutdown requests.  */
  gupcr_syscall (pthread_create,
		 (&gupcr_shutdown_pthread_id, (pthread_attr_t *) NULL,
		  &gupcr_shutdown_pthread, NULL));

  /* Install a signal handler that processes remote
     shutdown global exit requests.  */
  gupcr_signal_enable (SHUTDOWN_SIGNAL, gupcr_shutdown_signal_handler);
}

/**
 * Release remote shutdown resources.
 * @ingroup INIT
 */
void
gupcr_shutdown_fini (void)
{
  int status;

  gupcr_log (FC_MISC, "shutdown fini called");

  /* Terminate the shutdown pthread.  */
  gupcr_shutdown_terminate_pthread ();

  /* Close fabric services.  */
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_ct->fid));
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_lct->fid));
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_cq->fid));
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_mr->fid));
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_lmr->fid));
  /* NOTE: Do not check for errors.  Fails occasionally.  */
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_shutdown_rx_ep->fid));
  gupcr_fabric_call_nc (fi_close, status, (&gupcr_shutdown_tx_ep->fid));
}

/** @} */
