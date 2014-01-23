/*===-- upc_sysdep.h - UPC Runtime Support Library -----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#ifndef _UPC_OS_H_
#define _UPC_OS_H_


/* An actual heap is required only for the SGI Irix
   based systems, because spin lock related data
   structures must live there.  The runtime doesn't
   otherwise need its own heap, so a null handle is
   passed around.  */
#ifdef __sgi__
typedef usptr_t os_heap_t;
#else
typedef void *os_heap_t;
#endif
typedef os_heap_t *os_heap_p;

#ifdef __sgi__
typedef volatile ptrdiff_t os_atomic_t;
#else
typedef volatile int os_atomic_t;
#endif
typedef os_atomic_t *os_atomic_p;

#define OS_ATOMIC_WORD_SIZE (sizeof(os_atomic_t))
#define OS_BITS_PER_ATOMIC_WORD (OS_ATOMIC_WORD_SIZE * 8)

#ifdef __sgi__
typedef ulock_t os_lock_t;
#else
typedef os_atomic_t os_lock_t;
#endif
typedef os_lock_t *os_lock_p;

extern void __upc_sys_init (void);

extern int __upc_atomic_get_bit (os_atomic_p, int);
extern void __upc_atomic_set_bit (os_atomic_p, int);

extern void __upc_init_lock (os_lock_p);
extern void __upc_acquire_lock (os_lock_p);
extern int __upc_try_acquire_lock (os_lock_p);
extern void __upc_release_lock (os_lock_p);

extern os_heap_p __upc_create_runtime_heap (size_t, const char **);
extern void *__upc_runtime_alloc (size_t, os_heap_p *, const char **);
extern int __upc_create_temp_file (const char *tag, char *tmp_fname, 
				   const char **err_msg);
extern int __upc_create_global_mem_file (char *tmp_fname, const char **err_msg);
extern char *__upc_strsignal (int);

#endif /* !_UPC_OS_H_ */
