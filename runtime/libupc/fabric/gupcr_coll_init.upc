/*===-- gupcr_coll_init.upc - UPC Runtime Support Library ----------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

#include <upc.h>
#include <upc_collective.h>
#include "upc_coll.h"
#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_sup.h"
#include "gupcr_fabric.h"
#include "gupcr_iface.h"
#include "gupcr_utils.h"
#include "gupcr_coll_sup.h"

/**
 * @file gupcr_coll_init.upc
 * GUPC Portals4 collectives initialization.
 *
 * @addtogroup COLLECTIVES GUPCR Collectives Functions
 * @{
 */
int upc_coll_init_flag = 0;

/**
 * Collectives initialization function.
 *
 * Initialize necessary storage area for the broadcast/reduce
 * thread trees.
 */
void
upc_coll_init ()
{
  if (upc_coll_init_flag)
    gupcr_fatal_error ("multiple attempts to initialize collectives");
  upc_coll_init_flag = 1;

  /* Allocate the "all reduce" storage area.  */
  gupcr_reduce_storage = (gupcr_reduce_str_t)
    upc_all_alloc (THREADS, sizeof (struct gupcr_reduce_str));
  if (gupcr_reduce_storage == NULL)
    gupcr_fatal_error ("cannot allocate collectives reduce shared storage");
}

/* @} */
