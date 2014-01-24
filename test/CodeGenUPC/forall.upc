// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

void f(shared int * ptr, int);

void test_upcforall_int(shared int * ptr, int n) {
  upc_forall(int i = 0; i < n; ++i; i) {
    f(ptr, i);
  }
}
// CHECK: test_upcforall_int
// CHECK: %{{[0-9]+}} = load i32* @__upc_forall_depth
// CHECK-NEXT: %upc_forall.inc_depth = add nuw i32 %{{[0-9]+}}, 1
// CHECK-NEXT: store i32 %upc_forall.inc_depth, i32* @__upc_forall_depth

// CHECK: upc_forall.cond:
// CHECK-NEXT:  %{{[0-9]+}} = load i32* %i, align 4                                                   
// CHECK-NEXT:  %{{[0-9]+}} = load i32* %n.addr, align 4
// CHECK-NEXT:  %cmp = icmp slt i32 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-NEXT:  br i1 %cmp, label %upc_forall.filter, label %upcforall.cond.cleanup

// CHECK: upcforall.cond.cleanup:
// CHECK-NEXT: store i32 2, i32* %cleanup.dest.slot
// CHECK-NEXT: store i32 %{{[0-9]+}}, i32* @__upc_forall_depth

// CHECK: upc_forall.filter:
// CHECK-NEXT: %{{[0-9]+}} = load i32* %i, align 4
// CHECK-NEXT: %{{[0-9]+}} = load i32* @THREADS
// CHECK-NEXT: %{{[0-9]+}} = srem i32 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = add i32 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = icmp slt i32 %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = select i1 %{{[0-9]+}}, i32 %{{[0-9]+}}, i32 %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = load i32* @MYTHREAD
// CHECK-NEXT: %{{[0-9]+}} = icmp eq i32 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = icmp ugt i32 %{{[0-9]+}}, 0
// CHECK-NEXT: %{{[0-9]+}} = or i1 %{{[0-9]+}}, %{{[0-9]+}}
// CHECK-NEXT: br i1 %{{[0-9]+}}, label %upc_forall.body, label %upc_forall.inc
