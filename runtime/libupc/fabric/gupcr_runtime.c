/*===------ gupcr_runtime.c - Runtime Support Library --------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/* Copyright (c) 2011-2012, Sandia Corporation.
   All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 . Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 . Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 . Neither the name of the Sandia Corporation nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.  */

/**
 * @file gupcr_runtime.c
 * GUPC Libfabric Runtime.
 */

/**
 * @addtogroup RUNTIME GUPCR PMI
 * @{
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rdma/fabric.h>
#if HAVE_PMI2_H && LIBUPC_JOB_PMI2_API
#include <pmi2.h>
#endif
#if HAVE_PMI_H
#include <pmi.h>
#endif

/** Process rank */
static int rank = -1;
/** Number of processes */
static int size = 0;

static int max_name_len, max_key_len, max_val_len;
static char *kvs_name, *kvs_key, *kvs_val;

static int
encode (const void *inval, int invallen, char *outval, int outvallen)
{
  static unsigned char encodings[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
  };
  int i;
  if (invallen * 2 + 1 > outvallen)
    return 1;
  for (i = 0; i < invallen; i++)
    {
      outval[2 * i] = encodings[((unsigned char *) inval)[i] & 0xf];
      outval[2 * i + 1] = encodings[((unsigned char *) inval)[i] >> 4];
    }
  outval[invallen * 2] = '\0';
  return 0;
}

static int
decode (const char *inval, void *outval, int outvallen)
{
  int i;
  char *ret = (char *) outval;

  if (outvallen != (int) (strlen (inval) / 2))
    return 1;
  for (i = 0; i < outvallen; ++i)
    {
      if (*inval >= '0' && *inval <= '9')
	ret[i] = *inval - '0';
      else
	ret[i] = *inval - 'a' + 10;
      inval++;
      if (*inval >= '0' && *inval <= '9')
	ret[i] |= ((*inval - '0') << 4);
      else
	ret[i] |= ((*inval - 'a' + 10) << 4);
      inval++;
    }
  return 0;
}

#define CHECK_PMI(status) \
  if (status != PMI_SUCCESS) \
    return status;

/**
 * Initialize runtime.
 */
int
gupcr_runtime_init (void)
{
  int initialized;
  int __attribute__((unused)) appnum;
  int status;

#if LIBUPC_JOB_PMI2_API
  status = PMI2_Init (&initialized, &size, &rank, &appnum);
  CHECK_PMI (status);
  max_name_len = 1024;
  max_key_len = PMI2_MAX_KEYLEN;
  max_val_len = PMI2_MAX_VALLEN;
#else
  status = PMI_Init (&initialized);
  CHECK_PMI (status);
  status = PMI_Get_rank (&rank);
  CHECK_PMI (status);
  status = PMI_Get_size (&size);
  CHECK_PMI (status);
  /* Initialize key/val work strings.  */
  status = PMI_KVS_Get_name_length_max (&max_name_len);
  CHECK_PMI (status);
  status = PMI_KVS_Get_key_length_max (&max_key_len);
  CHECK_PMI (status);
  status = PMI_KVS_Get_value_length_max (&max_val_len);
  CHECK_PMI (status);
#endif
  kvs_name = calloc (max_name_len, 1);
  if (!kvs_name)
    return 1;
  kvs_key = calloc (max_key_len, 1);
  if (!kvs_key)
    return 1;
  kvs_val = calloc (max_val_len, 1);
  if (!kvs_val)
    return 1;
#if LIBUPC_JOB_PMI2_API
  status = PMI2_Job_GetId (kvs_name, max_name_len);
  CHECK_PMI (status);
#else
  status = PMI_KVS_Get_my_name (kvs_name, max_name_len);
  CHECK_PMI (status);
#endif
  return 0;
}

/**
 * Finalize runtime.
 */
int
gupcr_runtime_fini (void)
{
#if LIBUPC_JOB_PMI2_API
  PMI2_Finalize ();
#else
  PMI_Finalize ();
#endif
  return 0;
}

/**
 * Share the key via PMI.
 */
int
gupcr_runtime_put (const char *key, void *val, size_t len)
{
  int status;
  snprintf (kvs_key, max_key_len, "gupcr-%s-%lu", key, (long unsigned) rank);
  if (encode (val, len, kvs_val, max_val_len))
    return 1;
#if LIBUPC_JOB_PMI2_API
  status = PMI2_KVS_Put(kvs_key, kvs_val);
#else
  status = PMI_KVS_Put(kvs_name, kvs_key, kvs_val);
#endif
  CHECK_PMI (status);
  return 0;
}

/**
 * Get the key from PMI.
 */
int
gupcr_runtime_get(int rank, const char *key, void *val, size_t len)
{
  int __attribute__((unused)) keylen;
  int status;
  snprintf(kvs_key, max_key_len, "gupcr-%s-%lu", key, (long unsigned) rank);
#if LIBUPC_JOB_PMI2_API
  status = PMI2_KVS_Get(kvs_name, PMI2_ID_NULL, kvs_key, kvs_val,
			max_val_len, &keylen);
#else
  status = PMI_KVS_Get(kvs_name, kvs_key, kvs_val, max_val_len);
#endif
  CHECK_PMI (status);
  status = decode(kvs_val, val, len);
  return status;
}

/**
 * Register and exchange predetermined keys.
 */
int
gupcr_runtime_exchange (const char *key, void *val, size_t len, void *res)
{
  int i, status;
  char *dst;

  if (gupcr_runtime_put (key, val, len))
    return 1;

  /* Commit local values.  */
#if LIBUPC_JOB_PMI2_API
  status = PMI2_KVS_Fence ();
  CHECK_PMI (status);
#else
  status = PMI_KVS_Commit (kvs_name);
  CHECK_PMI (status);
  status = PMI_Barrier ();
  CHECK_PMI (status);
#endif

  /* Collect info on other threads.  */
  dst = res;
  for (i = 0; i < size; ++i)
    {
      if (i == rank)
        memcpy (dst, val, len);
      else
        if (gupcr_runtime_get (i, key, dst, len))
	  return status;
      dst += len;
    }
  return 0;
}

void
gupcr_runtime_get_ni_mapping 
(char *key, void *val, size_t len, void *res)
{
}

/**
 * Return this process rank.
 */
int
gupcr_runtime_get_rank (void)
{
  return rank;
}

/**
 * Return number of processes in the system.
 */
int
gupcr_runtime_get_size (void)
{
  return size;
}

/**
 * Runtime barrier.
 */
void
gupcr_runtime_barrier (void)
{
#if LIBUPC_JOB_PMI2_API
  PMI2_KVS_Fence ();
#else
  PMI_Barrier ();
#endif
}

/** @} */
