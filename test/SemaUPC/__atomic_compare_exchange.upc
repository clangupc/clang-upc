// RUN: %clang_cc1 %s -fsyntax-only -verify

void inc(shared int * ptr, int * expected, shared int * val) {
  __atomic_compare_exchange(ptr, expected, val, 0, 0, 0); // expected-error{{address argument to atomic builtin must be a pointer}}
}
