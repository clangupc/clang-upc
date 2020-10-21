#===- __init__.py - Clang Python Bindings --------------------*- python -*--===#
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===------------------------------------------------------------------------===#

r"""
Clang Library Bindings
======================

This package provides access to the Clang compiler and libraries.

The available modules are:

  cindex

    Bindings for the Clang indexing library.
"""


# Python 3 uses unicode for strings. The bindings, in particular the interaction
# with ctypes, need modifying to handle conversions between unicode and
# c-strings.
import sys 
if sys.version_info[0] != 2: 
    raise Exception("Only Python 2 is supported.")

__all__ = ['cindex']

