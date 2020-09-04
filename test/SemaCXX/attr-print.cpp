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

// FIXME: Print this with correct format.
// CHECK: void foo1() __attribute__((noinline)) __attribute__((pure));
void foo1() __attribute__((noinline, pure));

// CHECK: typedef int Small1 __attribute__((mode(byte)));
typedef int Small1 __attribute__((mode(byte)));

// CHECK: int small __attribute__((mode(byte)));
int small __attribute__((mode(byte)));

// CHECK: int v __attribute__((visibility("hidden")));
int v __attribute__((visibility("hidden")));

// CHECK: __attribute__((malloc))char *PR24565()
char *PR24565() __attribute__((__malloc__));

// CHECK: class __attribute__((consumable("unknown"))) AttrTester1
class __attribute__((consumable(unknown))) AttrTester1 {
  // CHECK: __attribute__((callable_when("unconsumed", "consumed")))void callableWhen();
  void callableWhen()  __attribute__((callable_when("unconsumed", "consumed")));
};

// CHECK: class __single_inheritance SingleInheritance;
class __single_inheritance SingleInheritance;

// CHECK: class __multiple_inheritance MultipleInheritance;
class __multiple_inheritance MultipleInheritance;

// CHECK: class __virtual_inheritance VirtualInheritance;
class __virtual_inheritance VirtualInheritance;
