// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

struct S {
  unsigned i1 : 10;
  unsigned i2 : 20;
  unsigned long i3 : 34;
};

unsigned read_bitfield(shared struct S* ptr) { return ptr->i2; }
// CHECK: read_bitfield
// CHECK:      %{{[0-9]+}} = load i64* %coerce.dive2
// CHECK-NEXT: %call = call i64 @__getdi2(i64 %{{[0-9]+}})
// CHECK-NEXT: %bf.lshr = lshr i64 %call, 10
// CHECK-NEXT: %bf.clear = and i64 %bf.lshr, 1048575

void write_bitfield(shared struct S* ptr, unsigned val) { ptr->i2 = val; }
// CHECK: write_bitfield
// CHECK:      %{{[0-9]+}} = load i64* %coerce.dive2
// CHECK-NEXT: %call = call i64 @__getdi2(i64 %{{[0-9]+}})
// CHECK-NEXT: bf.value = and i64 %{{[0-9]+}}, 1048575
// CHECK-NEXT: %bf.shl = shl i64 %bf.value, 10
// CHECK-NEXT: %bf.clear = and i64 %call, -1073740801
// CHECK-NEXT: %bf.set = or i64 %bf.clear, %bf.shl
// CHECK:      %{{[0-9]+}} = load i64* %coerce.dive4
// CHECK-NEXT: call void @__putdi2(i64 %{{[0-9]+}}, i64 %bf.set)
