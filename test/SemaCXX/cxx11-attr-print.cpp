// RUN: %clang_cc1 -std=c++11 -ast-print -fms-extensions %s | FileCheck %s
//
// CHECK: int x __attribute__((aligned(4)));
int x __attribute__((aligned(4)));

// FIXME: Print this at a valid location for a __declspec attr.
// CHECK: int y __declspec(align(4));
__declspec(align(4)) int y;

// CHECK: int z {{\[}}[gnu::aligned(4)]];
int z [[gnu::aligned(4)]];

// CHECK: __attribute__((deprecated("warning")));
int a __attribute__((deprecated("warning")));

// CHECK: int b {{\[}}[gnu::deprecated("warning")]];
int b [[gnu::deprecated("warning")]];

// CHECK: int cxx11_alignas alignas(4);
alignas(4) int cxx11_alignas;

// CHECK: int c11_alignas _Alignas(alignof(int));
_Alignas(int) int c11_alignas;

// CHECK: __attribute__((const))void foo();
void foo() __attribute__((const));

// CHECK: __attribute__((__const))void bar();
void bar() __attribute__((__const));

// CHECK: __attribute__((warn_unused_result))int f1();
int f1() __attribute__((warn_unused_result));

// CHECK: {{\[}}[clang::warn_unused_result]]int f2();
int f2 [[clang::warn_unused_result]] ();

// CHECK: {{\[}}[gnu::warn_unused_result]]int f3();
int f3 [[gnu::warn_unused_result]] ();

// FIXME: ast-print need to print C++11
// attribute after function declare-id.
// CHECK: {{\[}}[noreturn]]void f4();
void f4 [[noreturn]] ();

// CHECK: {{\[}}[std::noreturn]]void f5();
void f5 [[std::noreturn]] ();

// CHECK: inline __attribute__((gnu_inline))void f6();
inline void f6() __attribute__((gnu_inline));

// CHECK: inline {{\[}}[gnu::gnu_inline]]void f7();
inline void f7 [[gnu::gnu_inline]] ();

// arguments printing
// CHECK: __attribute__((format(printf, 2, 3)))void f8(void *, const char *, ...);
void f8 (void *, const char *, ...) __attribute__ ((format (printf, 2, 3)));

// CHECK: int m __attribute__((aligned(4
// CHECK: int n alignas(4
// CHECK: static __attribute__((pure))int f() {
// CHECK: static {{\[}}[gnu::pure]]int g() {
template <typename T> struct S {
  __attribute__((aligned(4))) int m;
  alignas(4) int n;
  __attribute__((pure)) static int f() {
    return 0;
  }
  [[gnu::pure]] static int g() {
    return 1;
  }
};

// CHECK: int m __attribute__((aligned(4
// CHECK: int n alignas(4
// CHECK: static __attribute__((pure))int f() {
// CHECK: static {{\[}}[gnu::pure]]int g() {
template struct S<int>;

// CHECK: using Small2 {{\[}}[gnu::mode(byte)]] = int;
using Small2 [[gnu::mode(byte)]] = int;
