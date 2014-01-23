/*===-- upc_nb.h - UPC Runtime Support Library ---------------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/
#include <upc.h>
#include <upc_nb.h>

/**
 * Copy memory with non-blocking explicit handle transfer.
 *
 * @param[in] dst Destination shared memory pointer
 * @param[in] src Source shared memory pointer
 * @param[in] n Number of bytes to transfer
 * @retval UPC non-blocking transfer handle
 */
upc_handle_t
upc_memcpy_nb (shared void *restrict dst,
	       shared const void *restrict src, size_t n)
{
  upc_memcpy (dst, src, n);
  return UPC_COMPLETE_HANDLE;
}

/**
 * Get memory with non-blocking explicit handle transfer.
 *
 * @param[in] dst Destination local memory pointer
 * @param[in] src Source remote memory pointer
 * @param[in] n Number of bytes to transfer
 * @retval UPC non-blocking transfer handle
 */
upc_handle_t
upc_memget_nb (void *restrict dst,
	       shared const void *restrict src, size_t n)
{
  upc_memget (dst, src, n);
  return UPC_COMPLETE_HANDLE;
}

/**
 * Put memory with non-blocking explicit handle transfer.
 *
 * @param[in] dst Destination remote memory pointer
 * @param[in] src Source local memory pointer
 * @param[in] n Number of bytes to transfer
 * @retval UPC non-blocking transfer handle
 */
upc_handle_t
upc_memput_nb (shared void *restrict dst,
	       const void *restrict src, size_t n)
{
  upc_memput (dst, src, n);
  return UPC_COMPLETE_HANDLE;
}

/**
 * Set memory with non-blocking implicit handle transfer.
 *
 * @param[in] dst Shared remote pointer
 * @param[in] c Value for set operation
 * @param[in] n Number of bytes to set
 * @retval UPC non-blocking transfer handle
 */
upc_handle_t
upc_memset_nb (shared void *dst, int c, size_t n)
{
  upc_memset (dst, c, n);
  return UPC_COMPLETE_HANDLE;
}

/**
 * Explicit handle non-blocking transfer sync attempt.
 *
 * @param[in] handle Transfer explicit handle
 * @retval UPC_NB_COMPLETED returned if transfer completed,
 *	   otherwise UPC_NB_NOT_COMPLETED
 */
int
upc_sync_attempt (upc_handle_t ARG_UNUSED(handle))
{
  return UPC_NB_COMPLETED;
}

/**
 * Explicit handle non-blocking transfer sync.
 *
 * @param[in] handle Non-blocking transfer explicit handle
 */
void
upc_sync (upc_handle_t ARG_UNUSED(handle))
{
}

/**
 * Copy memory with non-blocking implicit handle transfer.
 *
 * @param[in] dst Shared remote memory pointer
 * @param[in] src Shared remote memory pointer
 * @param[in] n Number of bytes to transfer
 */
void
upc_memcpy_nbi (shared void *restrict dst,
		shared const void *restrict src, size_t n)
{
  upc_memcpy (dst, src, n);
}

/**
 * Get memory with non-blocking implicit handle transfer.
 *
 * @param[in] dst Local memory pointer
 * @param[in] src Shared remote memory pointer
 * @param[in] n Number of bytes to transfer
 */
void
upc_memget_nbi (void *restrict dst,
		shared const void *restrict src, size_t n)
{
  upc_memget (dst, src, n);
}

/**
 * Put memory with non-blocking implicit handle transfer.
 *
 * @param[in] dst Shared remote memory pointer
 * @param[in] src Local memory pointer
 * @param[in] n Number of bytes to transfer
 */
void
upc_memput_nbi (shared void *restrict dst,
		const void *restrict src, size_t n)
{
  upc_memput (dst, src, n);
}

/**
 * Set memory with non-blocking implicit handle transfer.
 *
 * @param[in] dst Shared remote pointer
 * @param[in] c Value for set operation
 * @param[in] n Number of bytes to set
 */
void
upc_memset_nbi (shared void *dst, int c, size_t n)
{
  upc_memset (dst, c, n);
}

/**
 * Check on implicit handle non-blocking transfers.
 *
 * @retval UPC_NB_COMPLETED if no transfers pending, otherwise
 *         UPC_NB_NOT_COMPLETED is returned
 */
int
upc_synci_attempt (void)
{
  return UPC_NB_COMPLETED;
}

/**
 * Complete implicit handle non-blocking transfers.
 */
void
upc_synci (void)
{
}
