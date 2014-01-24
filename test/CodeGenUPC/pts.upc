// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s -check-prefix=CHECK-PF
// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - -fupc-pts-vaddr-order=last | FileCheck %s -check-prefix=CHECK-PL
// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - -fupc-pts=struct | FileCheck %s -check-prefix=CHECK-SF
// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - -fupc-pts=struct -fupc-pts-vaddr-order=last | FileCheck %s -check-prefix=CHECK-SL

// Use +, since it has to pick apart the representation
// and put it back together.
shared [3] int * test(shared [3] int * ptr, int i) { return ptr + i; }

// CHECK-PF: and i64 %{{[0-9]+}}, 1048575
// CHECK-PF: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 20
// CHECK-PF: %{{[0-9]+}} = and i64 %{{[0-9]+}}, 1023
// CHECK-PF: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 30
// CHECK-PF: %{{[0-9]+}} = udiv i64 %{{[0-9]+}}, 3
// CHECK-PF: %{{[0-9]+}} = urem i64 %{{[0-9]+}}, 3
// CHECK-PF: %{{[0-9]+}} = add i64 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-PF: %{{[0-9]+}} = shl i64 %{{[0-9]+}}, 20
// CHECK-PF: %{{[0-9]+}} = or i64 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-PF: %{{[0-9]+}} = shl i64 %{{[0-9]+}}, 30
// CHECK-PF: %{{[0-9]+}} = or i64 %{{[0-9]+}}, %{{[0-9]+}}

// CHECK-PL: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 44
// CHECK-PL: %{{[0-9]+}} = lshr i64 %{{[0-9]+}}, 34
// CHECK-PL: %{{[0-9]+}} = and i64 %{{[0-9]+}}, 1023
// CHECK-PL: %{{[0-9]+}} = and i64 %{{[0-9]+}}, 17179869183
// CHECK-PL: %{{[0-9]+}} = udiv i64 %{{[0-9]+}}, 3
// CHECK-PL: %{{[0-9]+}} = urem i64 %{{[0-9]+}}, 3
// CHECK-PL: %{{[0-9]+}} = add i64 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-PL: %{{[0-9]+}} = shl i64 %{{[0-9]+}}, 34
// CHECK-PL: %{{[0-9]+}} = or i64 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-PL: %{{[0-9]+}} = shl i64 %{{[0-9]+}}, 44
// CHECK-PL: %{{[0-9]+}} = or i64 %{{[0-9]+}}, %{{[0-9]+}}

// CHECK-SF: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 2
// CHECK-SF: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 1
// CHECK-SF: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 0
// CHECK-SF: %{{[0-9]+}} = udiv i64 %{{[0-9]+}}, 3
// CHECK-SF: %{{[0-9]+}} = urem i64 %{{[0-9]+}}, 3
// CHECK-SF: %{{[0-9]+}} = add i64 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-SF: %{{[0-9]+}} = trunc i64 %{{[0-9]+}} to i32
// CHECK-SF: %{{[0-9]+}} = trunc i64 %{{[0-9]+}} to i32
// CHECK-SF: %{{[0-9]+}} = insertvalue %__upc_shared_pointer_type undef, i64 %{{[0-9]+}}, 0
// CHECK-SF: %{{[0-9]+}} = insertvalue %__upc_shared_pointer_type %{{[0-9]+}}, i32 %{{[0-9]+}}, 1
// CHECK-SF: %{{[0-9]+}} = insertvalue %__upc_shared_pointer_type %{{[0-9]+}}, i32 %{{[0-9]+}}, 2

// CHECK-SL: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 0
// CHECK-SL: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 1
// CHECK-SL: %{{[0-9]+}} = extractvalue %__upc_shared_pointer_type %{{[0-9]+}}, 2
// CHECK-SL: %{{[0-9]+}} = udiv i64 %{{[0-9]+}}, 3
// CHECK-SL: %{{[0-9]+}} = urem i64 %{{[0-9]+}}, 3
// CHECK-SL: %{{[0-9]+}} = add i64 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-SL: %{{[0-9]+}} = trunc i64 %{{[0-9]+}} to i32
// CHECK-SL: %{{[0-9]+}} = trunc i64 %{{[0-9]+}} to i32
// CHECK-SL: %{{[0-9]+}} = insertvalue %__upc_shared_pointer_type undef, i32 %{{[0-9]+}}, 0
// CHECK-SL: %{{[0-9]+}} = insertvalue %__upc_shared_pointer_type %{{[0-9]+}}, i32 %{{[0-9]+}}, 1
// CHECK-SL: %{{[0-9]+}} = insertvalue %__upc_shared_pointer_type %{{[0-9]+}}, i64 %{{[0-9]+}}, 2
