// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

shared [2] int * testzerophase(shared [3] int * ptr) {
  return (shared [2] int *)ptr;
}
// CHECK: testzerophase
// CHECK: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 30
// CHECK-NEXT: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 20
// CHECK-NEXT: %{{[0-9]+}} = and i64 %{{[0-9]+}}, 1023
// CHECK-NEXT: %{{[0-9]+}} = shl i64 %{{[0-9]+}}, 20
// CHECK-NEXT: %{{[0-9]+}} = shl i64 %{{[0-9]+}}, 30
// CHECK-NEXT: %{{[0-9]+}} = or i64 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = insertvalue %__upc_shared_pointer_type undef, i64 %{{[0-9]+}}, 0

shared int *testnull(void) { return 0; }
// CHECK: testnull
// CHECK: store %__upc_shared_pointer_type zeroinitializer, %__upc_shared_pointer_type* %retval
