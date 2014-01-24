// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

#pragma upc relaxed

void test_plus_assign(shared int *ptr, int val) { *ptr += val; }
// CHECK: test_plus_assign
// CHECK: call i32 @__getsi2(i64 %{{[0-9]+}})
// CHECK: add nsw i32 %call, %{{[0-9]+}}
// CHECK: call void @__putsi2(i64 %{{[0-9]+}}, i32 %add)

#pragma upc strict

void test_plus_assigns(shared int *ptr, int val) { *ptr += val; }
// CHECK: test_plus_assigns
// CHECK: call i32 @__getssi2(i64 %{{[0-9]+}})
// CHECK: add nsw i32 %call, %{{[0-9]+}}
// CHECK: call void @__putssi2(i64 %{{[0-9]+}}, i32 %add)
