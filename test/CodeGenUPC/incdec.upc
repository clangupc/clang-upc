// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

void test_inc_Complex(shared _Complex int * ptr) { ++*ptr; }
// CHECK: test_inc_Complex
// CHECK: call void @__getblk3(i8* %{{[0-9]+}}, i64 %{{[0-9]+}}, i64 8)
// CHECK: %{{[0-9]+}} = extractvalue { i32, i32 } %{{[0-9]+}}, 0
// CHECK: %{{[0-9]+}} = extractvalue { i32, i32 } %{{[0-9]+}}, 1
// CHECK: %inc = add i32 %{{[0-9]+}}, 1
// CHECK: %{{[0-9]+}} = insertvalue { i32, i32 } undef, i32 %inc, 0
// CHECK: %{{[0-9]+}} = insertvalue { i32, i32 } %{{[0-9]+}}, i32 %{{[0-9]+}}, 1
// CHECK: call void @__putblk3(i64 %{{[0-9]+}}, i8* %{{[0-9]+}}, i64 8)

void test_inc_int(shared int * ptr) { ++*ptr; }
// CHECK: test_inc_int
// CHECK: %call = call i32 @__getsi2(i64 %{{[0-9]+}})
// CHECK: %inc = add nsw i32 %call, 1
// CHECK: call void @__putsi2(i64 %{{[0-9]+}}, i32 %inc)
