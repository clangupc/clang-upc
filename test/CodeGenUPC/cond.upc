// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

int test_if(shared int * ptr) {
  if (ptr)
    return 1;
  else
    return 0;
}
// CHECK: test_if
// CHECK: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = and i64 %{{[0-9]+}}, 1048575
// CHECK-NEXT: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 20
// CHECK-NEXT: %{{[0-9]+}} = and i64 %{{[0-9]+}}, 1023
// CHECK-NEXT: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 30
// CHECK-NEXT: %{{[0-9]+}} = icmp ne i64 %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = icmp ne i64 %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = icmp ne i64 %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = or i1 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = or i1 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-NEXT: br i1 %{{[0-9]+}}, label %if.then, label %if.else
