/*===-- gupcr_node_mem_mmap.c - UPC Runtime Support Library --------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_node_mem_mmap.c
 * GUPC Node Local Memory - Mmap Support.
 */

/**
 * @addtogroup NODE GUPCR Node Local Memory
 * @{
 */

#include <stdio.h>
#include <sys/time.h>
#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_utils.h"
#include "gupcr_fabric.h"
#include "gupcr_node.h"

/** Thread's node local memory shared space.  */
static char gupcr_thread_local_name[FILENAME_MAX];

/**
 * Map specified thread's shared memory into the current
 * thread's memory.
 *
 * @param [in] thread Thread ID
 * @param [in] size Shared memory size
 * @retval Memory address of the map.
 */
char *
gupcr_mem_local_map (int thread, size_t size)
{
  int fd;
  char fname[FILENAME_MAX];
  char *maddr;
  /* Create file name for mmap.  */
  gupcr_unique_local_name (fname, GUPCR_LOCAL_NAME_PREFIX, thread, 1);
  if (thread == MYTHREAD)
    {
      strcpy (gupcr_thread_local_name, fname);
      gupcr_log (FC_MEM, "shared memory file: %s", fname);
    }
  fd = open (fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd < 0)
    gupcr_fatal_error ("cannot create file for mmap %s", fname);
  if (ftruncate (fd, size))
    gupcr_fatal_error ("cannot resize file %s 0x%lx", fname, size);
  maddr = mmap (NULL, size, PROT_READ | PROT_WRITE,
		MAP_SHARED, fd, OFFSET_ZERO);
  if (!maddr || maddr == MAP_ERROR)
    gupcr_fatal_error ("cannot map 0x%lx bytes of %s", size, fname);
  return maddr;
}

/**
 * Remove the file used for mmap.
 * On Unix systems, this removes the file name, but since
 * the file is open, the underlying file storage will remain
 * until the program terminates.
 */
void
gupcr_mem_local_unlink (void)
{
  unlink (gupcr_thread_local_name);
}

/*
 * Initialize mmap support.
 */
void
gupcr_mem_local_init (void)
{
  gupcr_log (FC_MEM, "using node local mmap shared memory file");
}

/** @} */
