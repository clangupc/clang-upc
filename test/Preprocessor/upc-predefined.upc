// RUN: %clang_cc1 %s -fupc-threads 2 -E -dM -o - | FileCheck %s --check-prefix=CHECK-STATIC
// CHECK-STATIC: #define THREADS 2
// CHECK-STATIC: #define UPC_MAX_BLOCK_SIZE 1048575
// CHECK-STATIC-NOT: #define __UPC_DYNAMIC_THREADS__
// CHECK-STATIC: #define __UPC_STATIC_THREADS__ 1
// CHECK-STATIC: #define __UPC_VERSION__ 201311L
// CHECK-STATIC: #define __UPC__ 1
//
// RUN: %clang_cc1 %s -E -dM -o - | FileCheck %s --check-prefix=CHECK-DYNAMIC
// CHECK-DYNAMIC-NOT: #define THREADS
// CHECK-DYNAMIC: #define UPC_MAX_BLOCK_SIZE 1048575
// CHECK-DYNAMIC: #define __UPC_DYNAMIC_THREADS__ 1
// CHECK-DYNAMIC-NOT: #define __UPC_STATIC_THREADS__
// CHECK-DYNAMIC: #define __UPC_VERSION__ 201311L
// CHECK-DYNAMIC: #define __UPC__ 1
