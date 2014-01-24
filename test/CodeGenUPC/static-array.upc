// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -fupc-threads 4 -o - | FileCheck %s -check-prefix=CHECK-ST
// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s -check-prefix=CHECK-DT

shared int c[THREADS];

int f() {
  static shared int a[THREADS], b[THREADS*4];
  // CHECK-ST: @f.a = internal global [1 x i32] zeroinitializer, section "upc_shared", align 16
  // CHECK-ST: @f.b = internal global [4 x i32] zeroinitializer, section "upc_shared", align 16

  // CHECK-DT: @f.a = internal global [1 x i32] zeroinitializer, section "upc_shared", align 4
  // CHECK-DT: @f.b = internal global [4 x i32] zeroinitializer, section "upc_shared", align 4

  // dynamic initialization of pointer-to-shared
  // (details omitted for clarity)
  static shared int *ptr = a + 2;
  // CHECK-ST: define internal void @__upc_global_var_init()
  // CHECK-ST: store %__upc_shared_pointer_type {{.*}}, %__upc_shared_pointer_type* @f.ptr, align 8

  // CHECK-DT: define internal void @__upc_global_var_init()
  // CHECK-DT:   %{{[0-9]+}} = load i32* @THREADS
  // CHECK-DT:  store %__upc_shared_pointer_type %{{[0-9]+}}, %__upc_shared_pointer_type* @f.ptr, align 8

  static shared int *pval = &a[3];
  // CHECK-ST: define internal void @__upc_global_var_init1()
  // CHECK-ST: store %__upc_shared_pointer_type {{.*}}, %__upc_shared_pointer_type* @f.pval, align 8

  // CHECK-DT:  define internal void @__upc_global_var_init1()
  // CHECK-DT:    %{{[0-9]+}} = load i32* @THREADS
  // CHECK-DT:    store %__upc_shared_pointer_type %{{[0-9]+}}, %__upc_shared_pointer_type* @f.pval, align 8

  return b[1] * c[0];
}



int main() {
  int i = f();
  return 0;
}
