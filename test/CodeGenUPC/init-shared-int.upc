// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -fupc-threads 4 -o - | FileCheck %s

shared int q = 17;
// CHECK: define internal void @__upc_global_var_init()
// CHECK:   %coerce = alloca %__upc_shared_pointer_type, align 8
// CHECK:   store %__upc_shared_pointer_type { i64 shl (i64 sub (i64 ptrtoint
// (i32* @q to i64), i64 ptrtoint (i8* @__upc_shared_start to i64)), i64 30) }, %__upc_shared_pointer_type* %{{[0-9]+}}, align 8
// CHECK:   %coerce.dive = getelementptr %__upc_shared_pointer_type* %coerce, i32 0, i32 0
// CHECK:   %{{[0-9]+}} = load i64* %coerce.dive
// CHECK:   call void @__putsi2(i64 %{{[0-9]+}}, i32 17)


int f() {
  static shared int r = 23;
  return r;
}
// CHECK: define internal void @__upc_global_var_init1()
// CHECK:   %coerce = alloca %__upc_shared_pointer_type, align 8
// CHECK:   store %__upc_shared_pointer_type { i64 shl (i64 sub (i64 ptrtoint
// (i32* @f.r to i64), i64 ptrtoint (i8* @__upc_shared_start to i64)), i64 30) }, %__upc_shared_pointer_type* %{{[0-9]+}}, align 8
// CHECK:   %coerce.dive = getelementptr %__upc_shared_pointer_type* %coerce, i32 0, i32 0
// CHECK:   %{{[0-9]+}} = load i64* %coerce.dive
// CHECK:   call void @__putsi2(i64 %{{[0-9]+}}, i32 23)


int main() {
  return q + f();
}
