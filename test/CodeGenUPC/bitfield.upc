// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

struct S {
  unsigned i1 : 10;
  unsigned i2 : 20;
  unsigned long i3 : 34;
};

unsigned read_bitfield(shared struct S* ptr) { return ptr->i2; }
// CHECK: read_bitfield
// CHECK:      %{{[0-9]+}} = load i64* %{{coerce.dive2|[0-9]+}}
// CHECK-NEXT: %{{call|[0-9]+}} = call i64 @__getdi2(i64 %{{[0-9]+}})
// CHECK-NEXT: %{{bf.lshr|[0-9]+}} = lshr i64 %{{call|[0-9]+}}, 10
// CHECK-NEXT: %{{bf.clear|[0-9]+}} = and i64 %{{bf.lshr|[0-9]+}}, 1048575

void write_bitfield(shared struct S* ptr, unsigned val) { ptr->i2 = val; }
// CHECK: write_bitfield
// CHECK:      %{{[0-9]+}} = load i64* %{{coerce.dive2|[0-9]+}}
// CHECK-NEXT: %{{call|[0-9]+}} = call i64 @__getdi2(i64 %{{[0-9]+}})
// CHECK-NEXT: %{{bf.value|[0-9]+}} = and i64 %{{[0-9]+}}, 1048575
// CHECK-NEXT: %{{bf.shl|[0-9]+}} = shl i64 %{{bf.value|[0-9]+}}, 10
// CHECK-NEXT: %{{bf.clear|[0-9]+}} = and i64 %{{call|[0-9]+}}, -1073740801
// CHECK-NEXT: %{{bf.set|[0-9]+}} = or i64 %{{bf.clear|[0-9]+}}, %{{bf.shl|[0-9]+}}
// CHECK:      %{{[0-9]+}} = load i64* %{{coerce.dive4|[0-9]+}}
// CHECK-NEXT: call void @__putdi2(i64 %{{[0-9]+}}, i64 %{{bf.set|[0-9]+}}
