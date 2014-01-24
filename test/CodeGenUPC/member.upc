// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

struct S {
  unsigned i1;
  unsigned i2;
  unsigned long i3;
};

unsigned read_member(shared struct S* ptr) { return ptr->i2; }
// CHECK: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 30
// CHECK-NEXT: %{{[0-9]+}} = add i64 %{{[0-9]+}}, 4
// CHECK: call i32 @__getsi2(i64 %{{[0-9]+}})

void write_member(shared struct S* ptr, unsigned val) { ptr->i2 = val; }
// CHECK: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 30
// CHECK-NEXT: %{{[0-9]+}} = add i64 %{{[0-9]+}}, 4
// CHECK: call void @__putsi2(i64 %{{[0-9]+}}, i32 %{{[0-9]+}})
