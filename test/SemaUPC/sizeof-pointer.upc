// RUN: %clang_cc1 %s -fsyntax-only -verify
// RUN: %clang_cc1 %s -fsyntax-only -fupc-pts=struct -verify
// expected-no-diagnostics

#define CAT_I(x, y) x ## y
#define CAT(x, y) CAT_I(x, y)
#define TEST(expr) char CAT(test, __LINE__)[(expr)? 1 : -1]

#if __SIZEOF_POINTER__ == 4
#TEST(sizeof(shared void *) == 8);
##else
##if __UPC_PTS_PACKED_REP__
#TEST(sizeof(shared void *) == 8);
##else
#TEST(sizeof(shared void *) == 16);
##endif
##endif
