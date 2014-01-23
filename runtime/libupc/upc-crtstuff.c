/*===-- upc-crtstuff.c - UPC Runtime Support Library ----------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intrepid Technology, Inc.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTREPID.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/* Target machine header files require this define.  */
#define IN_LIBGCC2

#undef USED_FOR_TARGET

#include "config.h"
#include "upc-crt-config.h"

/* Only define section start/end if no link script is used.   */

#ifdef CRT_BEGIN

/* Shared begin is always defined in order to allocate space
   at the beginning of the section.  */
#ifdef UPC_SHARED_SECTION_BEGIN
/* Establish a symbol at the beginning of the data section.  */
UPC_SHARED_SECTION_BEGIN
#endif /* UPC_SHARED_SECTION_BEGIN */

#ifndef HAVE_UPC_LINK_SCRIPT
#ifdef UPC_PGM_INFO_SECTION_BEGIN
/* Establish a symbol at the beginning of the program info data section.  */
UPC_PGM_INFO_SECTION_BEGIN
#endif /* UPC_PGM_INFO_SECTION_BEGIN */
#ifdef UPC_INIT_ARRAY_SECTION_BEGIN
/* Establish a symbol at the beginning of the initialization array section.  */
UPC_INIT_ARRAY_SECTION_BEGIN
#endif /* UPC_INIT_ARRAY_SECTION_BEGIN */
#endif /* !HAVE_UPC_LINK_SCRIPT */

#elif defined(CRT_END)		/* ! CRT_BEGIN */

#ifndef HAVE_UPC_LINK_SCRIPT
#ifdef UPC_SHARED_SECTION_END
/* Establish a symbol at the end of the shared data section.  */
UPC_SHARED_SECTION_END
#endif /* UPC_SHARED_SECTION_END */
#ifdef UPC_PGM_INFO_SECTION_END
/* Establish a symbol at the end of the program info data section.  */
UPC_PGM_INFO_SECTION_END
#endif /* UPC_PGM_INFO_SECTION_END */
#ifdef UPC_INIT_ARRAY_SECTION_END
/* Establish a symbol at the end of the initialization array section.  */
UPC_INIT_ARRAY_SECTION_END
#endif /* UPC_INIT_ARRAY_SECTION_END */
#endif /* !HAVE_UPC_LINK_SCRIPT */
#else /* ! CRT_BEGIN && ! CRT_END */
#error "One of CRT_BEGIN or CRT_END must be defined."
#endif
