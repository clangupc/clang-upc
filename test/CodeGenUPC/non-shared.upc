// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

// Make sure that a UPC array type behaves like a
// variables array when it isn't declared shared.

typedef int array_type[10][THREADS][7];

array_type * test_non_shared_add(array_type * ptr, int x) { return ptr + x; }
// CHECK: test_non_shared_add
// CHECK: store [7 x i32]* %ptr, [7 x i32]** %ptr.addr, align 8
// CHECK-NEXT: store i32 %x, i32* %x.addr, align 4
// CHECK-NEXT: %{{[0-9]+}} = load [7 x i32]** %ptr.addr, align 8
// CHECK-NEXT: %{{[0-9]+}} = load i32* %x.addr, align 4
// CHECK-NEXT: %idx.ext = sext i32 %{{[0-9]+}} to i64
// CHECK-NEXT: %{{[0-9]+}} = load i32* @THREADS
// CHECK-NEXT: %{{[0-9]+}} = zext i32 %{{[0-9]+}} to i64
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 1, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 10, %{{[0-9]+}}
// CHECK-NEXT: %vla.index = mul nsw i64 %idx.ext, %{{[0-9]+}}
// CHECK-NEXT: %add.ptr = getelementptr inbounds [7 x i32]* %{{[0-9]+}}, i64 %vla.index

array_type * test_non_shared_subscript(array_type * ptr, int x) { return &ptr[x]; }
// CHECK: test_non_shared_subscript
// CHECK: store [7 x i32]* %ptr, [7 x i32]** %ptr.addr, align 8
// CHECK-NEXT: store i32 %x, i32* %x.addr, align 4
// CHECK-NEXT: %{{[0-9]+}} = load i32* %x.addr, align 4
// CHECK-NEXT: %idxprom = sext i32 %{{[0-9]+}} to i64
// CHECK-NEXT: %{{[0-9]+}} = load [7 x i32]** %ptr.addr, align 8
// CHECK-NEXT: %{{[0-9]+}} = load i32* @THREADS
// CHECK-NEXT: %{{[0-9]+}} = zext i32 %{{[0-9]+}} to i64
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 1, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 10, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = mul nsw i64 %idxprom, %{{[0-9]+}}
// CHECK-NEXT: %arrayidx = getelementptr inbounds [7 x i32]* %{{[0-9]+}}, i64 %{{[0-9]+}}

int test_non_shared_minus(array_type * ptr1, array_type * ptr2) { return ptr1 - ptr2; }
// CHECK: test_non_shared_minus
// CHECK: store [7 x i32]* %ptr1, [7 x i32]** %ptr1.addr, align 8
// CHECK-NEXT: store [7 x i32]* %ptr2, [7 x i32]** %ptr2.addr, align 8
// CHECK-NEXT: %{{[0-9]+}} = load [7 x i32]** %ptr1.addr, align 8
// CHECK-NEXT: %{{[0-9]+}} = load [7 x i32]** %ptr2.addr, align 8
// CHECK-NEXT: %sub.ptr.lhs.cast = ptrtoint [7 x i32]* %{{[0-9]+}} to i64
// CHECK-NEXT: %sub.ptr.rhs.cast = ptrtoint [7 x i32]* %{{[0-9]+}} to i64
// CHECK-NEXT: %sub.ptr.sub = sub i64 %sub.ptr.lhs.cast, %sub.ptr.rhs.cast
// CHECK-NEXT: %{{[0-9]+}} = load i32* @THREADS
// CHECK-NEXT: %{{[0-9]+}} = zext i32 %{{[0-9]+}} to i64
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 1, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 10, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 28, %{{[0-9]+}}
// CHECK-NEXT: %sub.ptr.div = sdiv exact i64 %sub.ptr.sub, %{{[0-9]+}}
// CHECK-NEXT: %conv = trunc i64 %sub.ptr.div to i32

void test_non_shared_increment(array_type * * ptr) { ++*ptr; }
// CHECK: test_non_shared_increment
// CHECK: store [7 x i32]** %ptr, [7 x i32]*** %ptr.addr, align 8
// CHECK-NEXT: %{{[0-9]+}} = load [7 x i32]*** %ptr.addr, align 8
// CHECK-NEXT: %{{[0-9]+}} = load [7 x i32]** %{{[0-9]+}}, align 8
// CHECK-NEXT: %{{[0-9]+}} = load i32* @THREADS
// CHECK-NEXT: %{{[0-9]+}} = zext i32 %{{[0-9]+}} to i64
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 1, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 10, %{{[0-9]+}}
// CHECK-NEXT: %vla.inc = getelementptr inbounds [7 x i32]* %{{[0-9]+}}, i64 %{{[0-9]+}}
// CHECK-NEXT: store [7 x i32]* %vla.inc, [7 x i32]** %{{[0-9]+}}, align 8

void test_non_shared_decl() {
    array_type arr;
    test_non_shared_add(&arr, 2);
}
// CHECK: test_non_shared_decl
// CHECK: %{{[0-9]+}} = load i32* @THREADS
// CHECK-NEXT: %{{[0-9]+}} = zext i32 %{{[0-9]+}} to i64
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 1, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 10, %{{[0-9]+}}
// CHECK-NEXT: %vla = alloca [7 x i32], i64 %{{[0-9]+}}, align 4
// CHECK-NEXT: %call = call [7 x i32]* @test_non_shared_add([7 x i32]* %vla, i32 2)

int test_sizeof() { return sizeof(array_type); }
// CHECK: test_sizeof
// CHECK: %{{[0-9]+}} = load i32* @THREADS
// CHECK-NEXT: %{{[0-9]+}} = zext i32 %{{[0-9]+}} to i64
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 1, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 10, %{{[0-9]+}}
// CHECK-NEXT: %{{[0-9]+}} = mul nuw i64 28, %{{[0-9]+}}
// CHECK-NEXT: %conv = trunc i64 %{{[0-9]+}} to i32
