// RUN: %clang_cc1 %s -ast-print -fms-extensions | FileCheck %s

// CHECK: int x __attribute__((aligned(4)));
int x __attribute__((aligned(4)));

// FIXME: Print this at a valid location for a __declspec attr.
// CHECK: int y __declspec(align(4));
__declspec(align(4)) int y;

// CHECK: __attribute__((const))void foo();
void foo() __attribute__((const));

// CHECK: __attribute__((__const))void bar();
void bar() __attribute__((__const));

// FIXME: Print this with correct format and order.
// CHECK: __attribute__((pure)) __attribute__((noinline))void foo1();
void foo1() __attribute__((noinline, pure));

// CHECK: typedef int Small1 __attribute__((mode(byte)));
typedef int Small1 __attribute__((mode(byte)));

// CHECK: int small __attribute__((mode(byte)));
int small __attribute__((mode(byte)));
