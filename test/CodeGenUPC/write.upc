// RUN: %clang_cc1 %s -emit-llvm -triple x86_64-pc-linux -o - | FileCheck %s

typedef struct S_ { char data[20]; } S;
#pragma pack(push, 1)
typedef struct Unaligned_ { char ch; int i; } Unaligned;
#pragma pack(pop)

#pragma upc relaxed

void testc(shared char * ptr, char val) { *ptr = val; }
// CHECK: testc
// CHECK call void @__putqi2(i64 %{{[0-9]+}}, i8 zeroext %{{[0-9]+}})

void testsc(shared signed char * ptr, signed char val) { *ptr = val; }
// CHECK: testsc
// CHECK call void @__putqi2(i64 %{{[0-9]+}}, i8 zeroext %{{[0-9]+}})

void testuc(shared unsigned char * ptr, unsigned char val) { *ptr = val; }
// CHECK: testuc
// CHECK call void @__putqi2(i64 %{{[0-9]+}}, i8 zeroext %{{[0-9]+}})

void tests(shared short * ptr, short val) { *ptr = val; }
// CHECK: tests
// CHECK call void @__puthi2(i64 %{{[0-9]+}}, i16 zeroext %{{[0-9]+}})

void testus(shared unsigned short * ptr, unsigned short val) { *ptr = val; }
// CHECK: testus
// CHECK call void @__puthi2(i64 %{{[0-9]+}}, i16 zeroext %{{[0-9]+}})

void testi(shared int * ptr, int val) { *ptr = val; }
// CHECK: testi
// CHECK call void @__putsi2(i64 %{{[0-9]+}}, i32 %{{[0-9]+}})

void testui(shared unsigned int * ptr, unsigned int val) { *ptr = val; }
// CHECK: testui
// CHECK call void @__puthi2(i64 %{{[0-9]+}}, i32 %{{[0-9]+}})

void testl(shared long * ptr, long val) { *ptr = val; }
// CHECK: testl
// CHECK call void @__putdi2(i64 %9, i64 %6)

void testul(shared unsigned long * ptr, unsigned long val) { *ptr = val; }
// CHECK: testul
// CHECK call void @__putdi2(i64 %{{[0-9]+}}, i64 zeroext %{{[0-9]+}})

void testll(shared long long * ptr, long long val) { *ptr = val; }
// CHECK: testll
// CHECK call void @__putdi2(i64 %{{[0-9]+}}, i64 %{{[0-9]+}})

void testull(shared unsigned long long * ptr, unsigned long long val) { *ptr = val; }
// CHECK: testull
// CHECK call void @__putdi2(i64 %{{[0-9]+}}, i64 zeroext %{{[0-9]+}})

void test128(shared __int128_t * ptr, __int128_t val) { *ptr = val; }
// CHECK: test128
// CHECK call void @__putti2(i64 %{{[0-9]+}}, { i64, i64 } %{{[0-9]+}})

void testu128(shared __uint128_t * ptr, __uint128_t val) { *ptr = val; }
// CHECK: testu128
// CHECK call void @__putti2(i64 %{{[0-9]+}}, { i64, i64 } %{{[0-9]+}})

void testf(shared float * ptr, float val) { *ptr = val; }
// CHECK: testf
// CHECK call void @__putsf2(i64 %{{[0-9]+}}, float %{{[0-9]+}})

void testd(shared double * ptr, double val) { *ptr = val; }
// CHECK: testd
// CHECK call void @__putdf2(i64 %{{[0-9]+}}, double %{{[0-9]+}})

void testld(shared long double * ptr, long double val) { *ptr = val; }
// CHECK: testld
// CHECK call void @__puttf2(i64 %{{[0-9]+}}, double %{{[0-9]+}})

void testp(int * shared * ptr, int * val) { *ptr = val; }
// CHECK: testp
// CHECK: call void @__putdi2(i64 %{{[0-9]+}}, i64 %{{[0-9]+}})

void testS(shared S * ptr, S * in) { *ptr = *in; }
// CHECK: testS
// CHECK: call void @__putblk3(i64 %{{[0-9]+}}, i8* %{{[0-9]+}}, i64 20)

void testUnaligned(shared Unaligned * ptr, int val) { ptr->i = val; }
// CHECK: testUnaligned
// CHECK: call void @__putblk3(i64 %{{[0-9]+}}, i8* %{{[0-9]+}}, i64 4)

void testComplex(shared _Complex double * ptr, _Complex double val) { *ptr = val; }
// CHECK: testComplex
// CHECK: call void @__putblk3(i64 %{{[0-9]+}}, i8* %{{[0-9]+}}, i64 16)

void testReal(shared _Complex double * ptr, double val) { __real__ *ptr = val; }
// CHECK: testReal
// CHECK: call void @__putdf2(i64 %{{[0-9]+}}, double %{{[0-9]+}})

void testImag(shared _Complex double * ptr, double val) { __imag__ *ptr = val; }
// CHECK: testImag
// CHECK: call void @__putdf2(i64 %{{[0-9]+}}, double %{{[0-9]+}})

#pragma upc strict

void testcs(shared char * ptr, char val) { *ptr = val; }
// CHECK: testcs
// CHECK call void @__putsqi2(i64 %{{[0-9]+}}, i8 zeroext %{{[0-9]+}})

void testscs(shared signed char * ptr, signed char val) { *ptr = val; }
// CHECK: testscs
// CHECK call void @__putsqi2(i64 %{{[0-9]+}}, i8 zeroext %{{[0-9]+}})

void testucs(shared unsigned char * ptr, unsigned char val) { *ptr = val; }
// CHECK: testucs
// CHECK call void @__putsqi2(i64 %{{[0-9]+}}, i8 zeroext %{{[0-9]+}})

void testss(shared short * ptr, short val) { *ptr = val; }
// CHECK: testss
// CHECK call void @__putshi2(i64 %{{[0-9]+}}, i16 zeroext %{{[0-9]+}})

void testuss(shared unsigned short * ptr, unsigned short val) { *ptr = val; }
// CHECK: testuss
// CHECK call void @__putshi2(i64 %{{[0-9]+}}, i16 zeroext %{{[0-9]+}})

void testis(shared int * ptr, int val) { *ptr = val; }
// CHECK: testis
// CHECK call void @__putssi2(i64 %{{[0-9]+}}, i32 %{{[0-9]+}})

void testuis(shared unsigned int * ptr, unsigned int val) { *ptr = val; }
// CHECK: testuis
// CHECK call void @__putshi2(i64 %{{[0-9]+}}, i32 %{{[0-9]+}})

void testls(shared long * ptr, long val) { *ptr = val; }
// CHECK: testls
// CHECK call void @__putsdi2(i64 %{{[0-9]+}}, i64 %{{[0-9]+}})

void testuls(shared unsigned long * ptr, unsigned long val) { *ptr = val; }
// CHECK: testuls
// CHECK call void @__putsdi2(i64 %{{[0-9]+}}, i64 zeroext %{{[0-9]+}})

void testlls(shared long long * ptr, long long val) { *ptr = val; }
// CHECK: testlls
// CHECK call void @__putsdi2(i64 %{{[0-9]+}}, i64 %{{[0-9]+}})

void testulls(shared unsigned long long * ptr, unsigned long long val) { *ptr = val; }
// CHECK: testulls
// CHECK call void @__putsdi2(i64 %{{[0-9]+}}, i64 zeroext %{{[0-9]+}})

void test128s(shared __int128_t * ptr, __int128_t val) { *ptr = val; }
// CHECK: test128s
// CHECK call void @__putsti2(i64 %{{[0-9]+}}, { i64, i64 } %{{[0-9]+}})

void testu128s(shared __uint128_t * ptr, __uint128_t val) { *ptr = val; }
// CHECK: testu128s
// CHECK call void @__putsti2(i64 %{{[0-9]+}}, { i64, i64 } %{{[0-9]+}})

void testfs(shared float * ptr, float val) { *ptr = val; }
// CHECK: testfs
// CHECK call void @__putssf2(i64 %{{[0-9]+}}, float %{{[0-9]+}})

void testds(shared double * ptr, double val) { *ptr = val; }
// CHECK: testds
// CHECK call void @__putsdf2(i64 %{{[0-9]+}}, double %{{[0-9]+}})

void testlds(shared long double * ptr, long double val) { *ptr = val; }
// CHECK: testlds
// CHECK call void @__putstf2(i64 %{{[0-9]+}}, double %{{[0-9]+}})

void testps(int * shared * ptr, int * val) { *ptr = val; }
// CHECK: testps
// CHECK: call void @__putsdi2(i64 %{{[0-9]+}}, i64 %{{[0-9]+}})

void testSs(shared S * ptr, S * in) { *ptr = *in; }
// CHECK: testSs
// CHECK: call void @__putsblk3(i64 %{{[0-9]+}}, i8* %{{[0-9]+}}, i64 20)

void testUnaligneds(shared Unaligned * ptr, int val) { ptr->i = val; }
// CHECK: testUnaligneds
// CHECK: call void @__putsblk3(i64 %{{[0-9]+}}, i8* %{{[0-9]+}}, i64 4)

void testComplexs(shared _Complex double * ptr, _Complex double val) { *ptr = val; }
// CHECK: testComplexs
// CHECK: call void @__putsblk3(i64 %{{[0-9]+}}, i8* %{{[0-9]+}}, i64 16)

void testReals(shared _Complex double * ptr, double val) { __real__ *ptr = val; }
// CHECK: testReals
// CHECK: call void @__putsdf2(i64 %{{[0-9]+}}, double %{{[0-9]+}})

void testImags(shared _Complex double * ptr, double val) { __imag__ *ptr = val; }
// CHECK: testImags
// CHECK: call void @__putsdf2(i64 %{{[0-9]+}}, double %{{[0-9]+}})
