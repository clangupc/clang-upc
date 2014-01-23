/*===-- upc_nb.h - UPC Runtime Support Library ---------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#ifndef _UPC_NB_H_
#define _UPC_NB_H_

/* Sync attempt return values.  */
#define UPC_NB_NOT_COMPLETED 0
#define UPC_NB_COMPLETED 1

/* UPC non-blocking handle.  */
typedef unsigned long upc_handle_t;
#define UPC_COMPLETE_HANDLE (unsigned long) 0

/* Non-blocking memory transfers with explicit handle.  */
extern upc_handle_t upc_memcpy_nb (
	shared void *restrict dst, shared const void *restrict src, size_t n);
extern upc_handle_t upc_memget_nb (
	void *restrict dst, shared const void *restrict src, size_t n);
extern upc_handle_t upc_memput_nb (
	shared void *restrict dst, const void *restrict src, size_t n);
extern upc_handle_t upc_memset_nb (shared void *dst, int c, size_t n);
extern int upc_sync_attempt (upc_handle_t handle);
extern void upc_sync (upc_handle_t handle);

/* Non-blocking memory transfers with implicit handle.  */
extern void
upc_memcpy_nbi (shared void *restrict dst,
		shared const void *restrict src, size_t n);
extern void
upc_memget_nbi (void *restrict dst,
		shared const void *restrict src, size_t n);
extern void
upc_memput_nbi (shared void *restrict dst,
		const void *restrict src, size_t n);
extern void upc_memset_nbi (shared void *dst, int c, size_t n);
extern int upc_synci_attempt (void);
extern void upc_synci (void);

#endif /* !_UPC_NB_H_ */
