// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

void testnotify_arg(int i) {
  upc_notify i;
}
// CHECK: testnotify_arg
// CHECK: %{{[0-9]+}} = load i32* %i.addr, align 4
// CHECK-NEXT: call void @__upc_notify(i32 %{{[0-9]+}})

void testnotify_noarg() {
  upc_notify;
}
// CHECK: testnotify_noarg
// CHECK: call void @__upc_notify(i32 -2147483648)

void testwait_arg(int i) {
  upc_wait i;
}
// CHECK: testwait_arg
// CHECK: %{{[0-9]+}} = load i32* %i.addr, align 4
// CHECK-NEXT: call void @__upc_wait(i32 %{{[0-9]+}})

void testwait_noarg() {
  upc_wait;
}
// CHECK: testwait_noarg
// CHECK: call void @__upc_wait(i32 -2147483648)

void testbarrier_arg(int i) {
  upc_barrier i;
}
// CHECK: testbarrier_arg
// CHECK: %{{[0-9]+}} = load i32* %i.addr, align 4
// CHECK-NEXT: call void @__upc_barrier(i32 %{{[0-9]+}})

void testbarrier_noarg() {
  upc_barrier;
}
// CHECK: testbarrier_noarg
// CHECK: call void @__upc_barrier(i32 -2147483648)

void testfence() {
  upc_fence;
}
// CHECK: testfence
// CHECK: call void @__upc_fence()
