// RUN: %clang_cc1 -fsyntax-only -verify %s

shared [UPC_MAX_BLOCK_SIZE] int okay1[THREADS];
shared [(unsigned long long)UPC_MAX_BLOCK_SIZE + 1] int err1[THREADS]; // expected-error-re{{block size {{[[:digit:]]+}} exceeds UPC_MAX_BLOCK_SIZE}}

shared [*] int okay2[THREADS*(unsigned long long)UPC_MAX_BLOCK_SIZE];
shared [*] int err2[THREADS*((unsigned long long)UPC_MAX_BLOCK_SIZE + 1)]; // expected-error-re{{block size {{[[:digit:]]+}} exceeds UPC_MAX_BLOCK_SIZE}}
