// RUN: %clang_cc1 -fsyntax-only -fupc-threads 1 -Wno-duplicate-decl-specifier -verify %s

// combine shared, shared [], shared[0], shared[1], shared[2], shared[3], shared[*]
// in every possible way directly, through typedefs and through pointers.

shared     shared     int inn;
shared     shared [ ] int ine;
shared     shared [0] int in0;
shared     shared [1] int in1;
shared     shared [2] int in2;
shared     shared [3] int in3;
shared     shared [*] int ins;

shared [ ] shared     int ien;
shared [ ] shared [ ] int iee;
shared [ ] shared [0] int ie0;
shared [ ] shared [1] int ie1; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [ ] shared [2] int ie2; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [ ] shared [3] int ie3; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [ ] shared [*] int ies; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

shared [0] shared     int i0n;
shared [0] shared [ ] int i0e;
shared [0] shared [0] int i00;
shared [0] shared [1] int i01; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [0] shared [2] int i02; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [0] shared [3] int i03; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [0] shared [*] int i0s; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

shared [1] shared     int i1n;
shared [1] shared [ ] int i1e; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [1] shared [0] int i10; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [1] shared [1] int i11;
shared [1] shared [2] int i12; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [1] shared [3] int i13; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [1] shared [*] int i1s;

shared [2] shared     int i2n;
shared [2] shared [ ] int i2e; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [2] shared [0] int i20; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [2] shared [1] int i21; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [2] shared [2] int i22;
shared [2] shared [3] int i23; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [2] shared [*] int i2s; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

shared [3] shared     int i3n;
shared [3] shared [ ] int i3e; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [3] shared [0] int i30; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [3] shared [1] int i31; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [3] shared [2] int i32; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [3] shared [3] int i33;
shared [3] shared [*] int i3s; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

shared [*] shared     int isn;
shared [*] shared [ ] int ise; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [*] shared [0] int is0; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [*] shared [1] int is1;
shared [*] shared [2] int is2; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [*] shared [3] int is3; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [*] shared [*] int iss;

typedef shared     int shared_n_int;
shared     shared_n_int tnn;
shared [ ] shared_n_int tne;
shared [0] shared_n_int tn0;
shared [1] shared_n_int tn1;
shared [2] shared_n_int tn2;
shared [3] shared_n_int tn3;
shared [*] shared_n_int tns;

typedef shared [ ] int shared_e_int;
shared     shared_e_int ten;
shared [ ] shared_e_int tee;
shared [0] shared_e_int te0;
shared [1] shared_e_int te1; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [2] shared_e_int te2; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [3] shared_e_int te3; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [*] shared_e_int tes; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

typedef shared [0] int shared_0_int;
shared     shared_0_int t0n;
shared [ ] shared_0_int t0e;
shared [0] shared_0_int t00;
shared [1] shared_0_int t01; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [2] shared_0_int t02; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [3] shared_0_int t03; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [*] shared_0_int t0s; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

typedef shared [1] int shared_1_int;
shared     shared_1_int t1n;
shared [ ] shared_1_int t1e; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [0] shared_1_int t10; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [1] shared_1_int t11;
shared [2] shared_1_int t12; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [3] shared_1_int t13; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [*] shared_1_int t1s;

typedef shared [2] int shared_2_int;
shared     shared_2_int t2n;
shared [ ] shared_2_int t2e; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [0] shared_2_int t20; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [1] shared_2_int t21; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [2] shared_2_int t22;
shared [3] shared_2_int t23; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [*] shared_2_int t2s; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

typedef shared [3] int shared_3_int;
shared     shared_3_int t3n;
shared [ ] shared_3_int t3e; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [0] shared_3_int t30; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [1] shared_3_int t31; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [2] shared_3_int t32; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
shared [3] shared_3_int t33;
shared [*] shared_3_int t3s; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

int * shared     shared     pnn;
int * shared     shared [ ] pne;
int * shared     shared [0] pn0;
int * shared     shared [1] pn1;
int * shared     shared [2] pn2;
int * shared     shared [3] pn3;
int * shared     shared [*] pns;

int * shared [ ] shared     pen;
int * shared [ ] shared [ ] pee;
int * shared [ ] shared [0] pe0;
int * shared [ ] shared [1] pe1; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [ ] shared [2] pe2; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [ ] shared [3] pe3; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [ ] shared [*] pes; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

int * shared [0] shared     p0n;
int * shared [0] shared [ ] p0e;
int * shared [0] shared [0] p00;
int * shared [0] shared [1] p01; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [0] shared [2] p02; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [0] shared [3] p03; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [0] shared [*] p0s; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

int * shared [1] shared     p1n;
int * shared [1] shared [ ] p1e; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [1] shared [0] p10; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [1] shared [1] p11;
int * shared [1] shared [2] p12; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [1] shared [3] p13; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [1] shared [*] p1s;

int * shared [2] shared     p2n;
int * shared [2] shared [ ] p2e; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [2] shared [0] p20; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [2] shared [1] p21; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [2] shared [2] p22;
int * shared [2] shared [3] p23; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [2] shared [*] p2s; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

int * shared [3] shared     p3n;
int * shared [3] shared [ ] p3e; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [3] shared [0] p30; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [3] shared [1] p31; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [3] shared [2] p32; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [3] shared [3] p33;
int * shared [3] shared [*] p3s; // expected-error{{cannot combine with previous 'shared' declaration specifier}}

int * shared [*] shared     psn;
int * shared [*] shared [ ] pse; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [*] shared [0] ps0; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [*] shared [1] ps1;
int * shared [*] shared [2] ps2; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [*] shared [3] ps3; // expected-error{{cannot combine with previous 'shared' declaration specifier}}
int * shared [*] shared [*] pss;
