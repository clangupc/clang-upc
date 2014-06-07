##===- runtime/Makefile ------------------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##

CLANG_LEVEL := ..
include $(CLANG_LEVEL)/../../Makefile.config

ifndef NO_RUNTIME_LIBS

PARALLEL_DIRS  := compiler-rt libcxx

ifeq ($(ENABLE_CLANG_UPC_RUNTIME), 1)                                                 
  PARALLEL_DIRS += libupc
endif

endif

include $(CLANG_LEVEL)/Makefile

install::

