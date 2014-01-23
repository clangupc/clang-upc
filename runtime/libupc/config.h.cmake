/*===-- config.h.cmake - UPC Runtime Support Library ---------------------===*
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/* config.h.in.  Generated from configure.ac by autoheader.  */

#ifndef __CONFIG_H__
#define __CONFIG_H__ 1


//begin gupcr_config_h

/* Define path to preferred addr2line for backtrace */
#cmakedefine GUPCR_BACKTRACE_ADDR2LINE "@GUPCR_BACKTRACE_ADDR2LINE@"

/* Define path to preferred GDB for backtrace */
#cmakedefine GUPCR_BACKTRACE_GDB "@GUPCR_BACKTRACE_GDB@"

/* Define to preferred signal for UPC backtrace. */
#cmakedefine GUPCR_BACKTRACE_SIGNAL @GUPCR_BACKTRACE_SIGNAL@

/* Size of get/put bounce buffer */
#cmakedefine GUPCR_BOUNCE_BUFFER_SIZE @GUPCR_BOUNCE_BUFFER_SIZE@

/* upc_global_exit() timeout in seconds. */
#cmakedefine GUPCR_GLOBAL_EXIT_TIMEOUT @GUPCR_GLOBAL_EXIT_TIMEOUT@

/* Define to 1 if UPC runtime checks are supported. */
#cmakedefine GUPCR_HAVE_CHECKS 1

/* Define to 1 if UPC runtime debugging mode is enabled. */
#cmakedefine GUPCR_HAVE_DEBUG 1

/* Define if UPC GUM debug server is supported. */
#cmakedefine GUPCR_HAVE_GUM_DEBUG 1

/* Define to 1 if UPC runtime statistics collection is supported. */
#cmakedefine GUPCR_HAVE_STATS 1

/* Define to 1 if UPC runtime tracing is supported. */
#cmakedefine GUPCR_HAVE_TRACE 1

/* Maximum number of locks held per thread */
#cmakedefine GUPCR_MAX_LOCKS @GUPCR_MAX_LOCKS@

/* Define to 1 if UPC runtime will use node local memory accesses. */
#cmakedefine GUPCR_NODE_LOCAL_MEM 1

/* Define to 1 if UPC node local access uses mmap-ed file. */
#cmakedefine GUPCR_NODE_LOCAL_MEM_MMAP 1

/* Define to 1 if UPC node local access uses Posix shared memory. */
#cmakedefine GUPCR_NODE_LOCAL_MEM_POSIX 1

/* Portals4 PTE base index. */
#cmakedefine GUPCR_PTE_BASE @GUPCR_PTE_BASE@

/* Maximum number of children at each level of a collective operation tree. */
#cmakedefine GUPCR_TREE_FANOUT @GUPCR_TREE_FANOUT@

/* Define to 1 if UPC runtime will use Portals4 triggered operations. */
#cmakedefine GUPCR_USE_PORTALS4_TRIGGERED_OPS 1

//end gupcr_config_h

/* Define to 1 if you have the `clock_gettime' function. */
#cmakedefine HAVE_CLOCK_GETTIME 1

/* Define to 1 if you have the <execinfo.h> header file. */
#cmakedefine HAVE_EXECINFO_H 1

/* Use BFD library for improved backtrace. */
#cmakedefine HAVE_LIBBFD 1

/* Define to 1 if you have the <limits.h> header file. */
#cmakedefine HAVE_LIMITS_H 1

/* Define to 1 if you have the 'shm_open' function. */
#cmakedefine HAVE_SHM_OPEN 1

/* Define to 1 if the compiler provides the __sync_fetch_and_add function for
   uint32 */
#cmakedefine HAVE_SYNC_FETCH_AND_ADD_4 1

/* Define to 1 if the compiler provides the __sync_fetch_and_add function for
   uint64 */
#cmakedefine HAVE_SYNC_FETCH_AND_ADD_8 1

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine HAVE_SYS_TIME_H 1

/* Define to 1 if UPC backtrace is enabled. */
#cmakedefine HAVE_UPC_BACKTRACE 1

/* Define to 1 if UPC backtrace with ADDR2LINE is enabled. */
#cmakedefine HAVE_UPC_BACKTRACE_ADDR2LINE 1

/* Define to 1 if UPC backtrace with GDB is enabled. */
#cmakedefine HAVE_UPC_BACKTRACE_GDB 1

/* Define to 1 if UPC backtrace signal is enabled. */
#cmakedefine HAVE_UPC_BACKTRACE_SIGNAL 1

/* Define to 1 if UPC link script is supported. */
#cmakedefine HAVE_UPC_LINK_SCRIPT 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#cmakedefine TIME_WITH_SYS_TIME 1


#ifndef ARG_UNUSED
# define ARG_UNUSED(NAME) NAME __attribute__ ((__unused__))
#endif


#endif /* __CONFIG_H__ */

