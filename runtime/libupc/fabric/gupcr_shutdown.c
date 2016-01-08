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
 * This pthread uses fabric's tagged interface to receive a
 * global exit code from another UPC thread.  A simple tagged send of the
 * exit code issued to the shutdown EP on some other UPC thread
 * triggers exit of the receiving thread.
 *
 * The following steps are taken to initialize, wait, and signal
 * the UPC global exit:
 *
 * - Each thread initializes an EP to receive an exit code that
 *     was passed in as the argument to upc_global_exit().
 * - Each thread creates a helper pthread - gupcr_shutdown_pthread()
 *     that waits on the shutdown EP counting event (one count only).
 * - The main UPC thread installs a signal handler for SHUTDWON_SIGNAL
 *     that is used by the shutdown pthread to signal a need for
 *     global exit.
 * - Remote shutdown takes the following steps:
 *     -# A UPC thread executing a call to upc_global_exit() sends the
 *        exit code to all other UPC threads by using the shutdown EP.
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
#include "gupcr_sync.h"
#include "gupcr_utils.h"
#include "gupcr_fabric.h"
#include "gupcr_shutdown.h"
#include "gupcr_iface.h"

/** Shutdown signal to main thread */
#define SHUTDOWN_SIGNAL SIGUSR2
/** Shutdown check interval (100 miliseconds) */
#define SHUTDOWN_MICROSEC_WAIT 100000L

/** Index of the local memory location */
#define GUPCR_REMOTE_OFFSET(addr) \
	(uint64_t)((char *) addr - (char *) &gupcr_shutdown_memory)

/** Shutdown exit status sent/received */
int gupcr_shutdown_status;

/** Shutdown pthread ID */
static pthread_t gupcr_shutdown_pthread_id;
/** Shutdown pthread declaration */
static void *gupcr_shutdown_pthread (void *arg) __attribute__ ((noreturn));

/** Fabric communications endpoint resources */
/** Shutdown endpoint */
static gupcr_epinfo_t gupcr_shutdown_ep;
/** Completion counter for write */
static fab_cntr_t gupcr_shutdown_tcnt;
/** Completion counter for read */
static fab_cntr_t gupcr_shutdown_rcnt;
/** Completion queue on transmit side */
static fab_cq_t gupcr_shutdown_tcq;
/** Completion queue on receive side */
static fab_cq_t gupcr_shutdown_rcq;
#if LOCAL_MR_NEEDED
/** Local memory access memory region  */
static fab_mr_t gupcr_shutdown_lmr;
#endif

/** Target address */
#define GUPCR_TARGET_ADDR(target) \
	fi_rx_addr ((fi_addr_t)target, \
	GUPCR_FABRIC_SCALABLE_CTX() ? GUPCR_SERVICE_SHUTDOWN : 0, \
	GUPCR_FABRIC_SCALABLE_CTX() ? GUPCR_SERVICE_BITS : 1)

/**
 * Send a remote shutdown request to all threads.
 *
 * Wait for the thread's pthread and ACKs from sending
 * messages to other threads (with timeout).
 *
 * @param [in] status exit code passed to other threads
 */
void
gupcr_signal_exit (int status)
{
  ssize_t ret;
  int thread;
  int wait_cnt = GUPCR_GLOBAL_EXIT_TIMEOUT *
    (1000000L / SHUTDOWN_MICROSEC_WAIT);
  int done = 0;

  /* Protect local global exit from remote shutdown request.  */
  gupcr_signal_disable (SHUTDOWN_SIGNAL);

  gupcr_shutdown_status = status;
  /* Send global exit code to all threads.  */
  for (thread = 0; thread < THREADS; thread++)
    {
      gupcr_fabric_call_size (fi_tsend, ret,
			      (gupcr_shutdown_ep.tx_ep,
			       &gupcr_shutdown_status,
			       sizeof (gupcr_shutdown_status),
			       NULL, GUPCR_TARGET_ADDR (thread),
			       GUPCR_SHUTDOWN_TAG, NULL));
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
      gupcr_fabric_call_nc (fi_cntr_read, cnt, (gupcr_shutdown_tcnt));
      // Check that we received all acks
      gupcr_log (FC_MISC, "Shutdown cnt: %d", (int) cnt);
      if (cnt >=
	  (uint64_t) (GUPCR_FABRIC_RMA_EVENT ()? THREADS : 2 * THREADS))
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
 * be sent by this thread to its own shutdown EP.  The main thread
 * then waits for pthread to exit.
 */
static void
gupcr_shutdown_terminate_pthread (void)
{
  ssize_t ret;
  /* Disable interrupts before sending a signal to
     shutdown pthread.  */
  gupcr_signal_disable (SHUTDOWN_SIGNAL);

  gupcr_shutdown_status = 0;
  gupcr_fabric_call_size (fi_tsend, ret,
			  (gupcr_shutdown_ep.tx_ep,
			   &gupcr_shutdown_status,
			   sizeof (gupcr_shutdown_status),
			   NULL, GUPCR_TARGET_ADDR (MYTHREAD),
			   GUPCR_SHUTDOWN_TAG, NULL));
  gupcr_fabric_call_cntr_wait ((gupcr_shutdown_tcnt, 1,
				GUPCR_TRANSFER_NO_TIMEOUT), "shutdown put",
			       gupcr_shutdown_tcq);
  pthread_join (gupcr_shutdown_pthread_id, NULL);
}

/**
 * Shutdown pthread that waits for remote shutdown requests.
 *
 * This pthread waits on a shutdown EP to receive a shutdown
 * request from any other thread that executed upc_global_exit().
 * Then, it uses signal (SHUTDOWN_SIGNAL) to inform the main thread
 * of a need to shutdown this UPC thread.
 * @param [in] arg Pthread arguments (not used in this case)
 * @retval Pthread's exit value
 */
static void *
gupcr_shutdown_pthread (void *arg __attribute ((unused)))
{
  ssize_t ret;

  gupcr_log (FC_MISC, "Shutdown pthread started");
  /* Wait for the shutdown request.  */
  gupcr_fabric_call_size (fi_trecv, ret,
			  (gupcr_shutdown_ep.rx_ep,
			   &gupcr_shutdown_status,
			   sizeof (gupcr_shutdown_status),
			   NULL, FI_ADDR_UNSPEC,
			   GUPCR_SHUTDOWN_TAG, 0, NULL));
  gupcr_fabric_call_cntr_wait ((gupcr_shutdown_rcnt, 1,
				GUPCR_TRANSFER_NO_TIMEOUT), "shutdown get",
			       gupcr_shutdown_rcq);

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

  gupcr_log (FC_MISC, "shutdown init called");

  /* Create endpoints for shutdown.  */
  gupcr_shutdown_ep.name = "shutdown";
  gupcr_shutdown_ep.service = GUPCR_SERVICE_SHUTDOWN;
  gupcr_fabric_ep_create (&gupcr_shutdown_ep);

  /* ... and completion counter for write.  */
  cntr_attr.events = FI_CNTR_EVENTS_COMP;
  cntr_attr.flags = 0;
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr,
				    &gupcr_shutdown_tcnt, NULL));
  gupcr_fabric_call (fi_ep_bind, (gupcr_shutdown_ep.tx_ep,
				  &gupcr_shutdown_tcnt->fid, FI_SEND));

  /* ... and completion queue for write errors.  */
  cq_attr.size = GUPCR_CQ_ERROR_SIZE;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr, &gupcr_shutdown_tcq,
				  NULL));
  /* Bind to transmit context. Use FI_SELECTIVE_COMPLETION flag to
     report errors only.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_shutdown_ep.tx_ep,
				  &gupcr_shutdown_tcq->fid,
				  FI_TRANSMIT | FI_SELECTIVE_COMPLETION));

#if LOCAL_MR_NEEDED
  /* NOTE: Create a local memory region before enabling endpoint.  */
  /* ... and memory region for local memory accesses.  */
  gupcr_fabric_call (fi_mr_reg, (gupcr_fd, USER_PROG_MEM_START,
				 USER_PROG_MEM_SIZE, FI_READ | FI_WRITE, 0, 0,
				 0, &gupcr_shutdown_lmr, NULL));
  /* NOTE: There is no need to bind local memory region to endpoint.  */
  /*       Hmm ... ? We can probably use only one throughout the runtime,  */
  /*       as counters and events are bound to endpoint.  */
  gupcr_fabric_call (fi_ep_bind, (gupcr_shutdown_ep.tx_ep,
				  &gupcr_shutdown_lmr->fid,
				  FI_READ | FI_WRITE));
#endif

  /* ... and counter for receive messages.  */
  cntr_attr.events = FI_CNTR_EVENTS_COMP;
  cntr_attr.flags = 0;
  gupcr_fabric_call (fi_cntr_open, (gupcr_fd, &cntr_attr,
				    &gupcr_shutdown_rcnt, NULL));
  gupcr_fabric_call (fi_ep_bind, (gupcr_shutdown_ep.rx_ep,
				  &gupcr_shutdown_rcnt->fid, FI_RECV));
  /* ... and completion queue for receive message errors.  */
  cq_attr.size = GUPCR_CQ_ERROR_SIZE;
  cq_attr.format = FI_CQ_FORMAT_MSG;
  cq_attr.wait_obj = FI_WAIT_NONE;
  gupcr_fabric_call (fi_cq_open, (gupcr_fd, &cq_attr,
				  &gupcr_shutdown_rcq, NULL));
  gupcr_fabric_call (fi_ep_bind, (gupcr_shutdown_ep.rx_ep,
				  &gupcr_shutdown_rcq->fid,
				  FI_TRANSMIT | FI_SELECTIVE_COMPLETION));
  /* Enable endpoints.  */
  gupcr_fabric_call (fi_enable, (gupcr_shutdown_ep.tx_ep));
  gupcr_fabric_call (fi_enable, (gupcr_shutdown_ep.rx_ep));

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
  gupcr_log (FC_MISC, "shutdown fini called");

  /* Terminate the shutdown pthread.  */
  gupcr_shutdown_terminate_pthread ();

  /* Close fabric services.  */
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_tcnt->fid));
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_rcnt->fid));
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_rcq->fid));
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_tcq->fid));
#if MR_LOCAL_NEEDED
  gupcr_fabric_call (fi_close, (&gupcr_shutdown_lmr->fid));
#endif
  gupcr_fabric_ep_delete (&gupcr_shutdown_ep);
}

/** @} */
