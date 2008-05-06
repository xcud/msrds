//---------------------------------------------------------------------------

#ifndef DynLinAlgH
#define DynLinAlgH
#include "dynarrays.h"

//---------------------------------------------------------------------------

/*  scalar tridiagonal solver (Thomas algorithm)

        | d(1)  a(1)   0             | |u(1)|     |c(1)|
        | b(2)  d(2)  a(2)           | |u(2)|     |c(2)|
        |  0     -     -    -        | | .  |  =  | .  |
        |              -    -    -   | | .  |     | .  |
        |  0               b(n)  d(n)| |u(n)|     |c(n)|
INPUT:
       a - sup-diag,  a is NOT destroyed
       d - diagonal,  b is NOT destroyed
       b - sub-diag
       c(i) - r.h.s.
OUTPUT:
      solution vector, u(i) is returned to the calling program in the c(i) array
*/
void strid(FArr1D& b, FArr1D& d, FArr1D& a, FArr1D& c);


/* Tri-diagonal 2D matrix

   | m11 m12  0   -   |   | d(1) a(1)   0              |
   | m21 m22 m23  -   |   | b(2) d(2)  a(2)            |
   |  0  m32 m33  -   | = |  0    -     -     -        |
   |  -   -   -   -   |   |             -     -     -  |
   |  0   0   0   -   |   |  0               b(n)  d(n)|
INPUT:
       a - sup-diag
       d - diagonal
       b - sub-diag
OUTPUT:
       m - tri-dialonal matrix */
void TridTo2D(FArr1D& b, FArr1D& d, FArr1D& a, FArr2D& m);


/*  Gauss substitution with diagonal pivots

        | M(1,1) M(1,2) M(1,3) .  M(1,N)| |x(1)|     |b(1)|
        | M(2,1) M(2,2) M(2,3) .  M(2,N)| |x(2)|     |b(2)|
        | M(3,1) M(3,2) M(3,3) .  M(3,N)| |x(3)|     |b(3)|
        |   .      .      .    .    .   | | .  |  =  | .  |
        | M(N,1) M(N,2) M(N,3) .  M(N,N)| |x(N)|     |b(N)|
INPUT:
   M is N x (N+K) array  K > 0
   N x N contains the matrix itself
   the last K columns contain b - r.h.s.
OUTPUT:
   solution vector(s), x is(are) returned the last column(s) of M */
bool GaussD(FArr2D& M);


/*  Jacobi iterative method

        | M(1,1) M(1,2) M(1,3) .  M(1,N)| |x(1)|     |b(1)|
        | M(2,1) M(2,2) M(2,3) .  M(2,N)| |x(2)|     |b(2)|
        | M(3,1) M(3,2) M(3,3) .  M(3,N)| |x(3)|     |b(3)|
        |   .      .      .    .    .   | | .  |  =  | .  |
        | M(N,1) M(N,2) M(N,3) .  M(N,N)| |x(N)|     |b(N)|

INPUT:
   M is N x N array of coefficients
   b is the vector of r.h.s.
   x - initial approximation
   eps - accuracy
   itermax - iteration limit
OUTPUT:
   solution vector x */
bool JacobiIter(FArr2D& M, FArr1D& x, FArr1D& b, real eps, int itermax = 999);

/* Gaussian Elimination with partial pivoting

        | M(1,1) M(1,2) M(1,3) .  M(1,N)| |x(1)|     |b(1)|
        | M(2,1) M(2,2) M(2,3) .  M(2,N)| |x(2)|     |b(2)|
        | M(3,1) M(3,2) M(3,3) .  M(3,N)| |x(3)|     |b(3)|
        |   .      .      .    .    .   | | .  |  =  | .  |
        | M(N,1) M(N,2) M(N,3) .  M(N,N)| |x(N)|     |b(N)|

INPUT:
   M is N x (N+K) array  K > 0
   N x N contains the matrix itself
   the last K columns contain b - r.h.s.
OUTPUT:
   solution vector(s), x is(are) returned the last column(s) of M */
bool GaussP(FArr2D& M);

/*
  Conjugate Gradients solution of A*x = b

        | A(1,1) A(1,2) A(1,3) .  A(1,N)| |x(1)|     |b(1)|
        | A(2,1) A(2,2) A(2,3) .  A(2,N)| |x(2)|     |b(2)|
        | A(3,1) A(3,2) A(3,3) .  A(3,N)| |x(3)|     |b(3)|
        |   .      .      .    .    .   | | .  |  =  | .  |
        | A(N,1) A(N,2) A(N,3) .  A(N,N)| |x(N)|     |b(N)|
INPUT:
   A is N x N array
   x - initial guess for solution vector
   b - r.h.s.
OUTPUT:
   x - solution vector
Temp Arrays
   d, r, y         */
/* bool ConjGrad(FArr2D& A, FArr1D& b, FArr1D& x, FArr1D& y, FArr1D& d, FArr1D& r,
              real eps, int itermax = 999);  */
bool ConjGrad(FArr2D& A, FArr1D& b, FArr1D& x, real eps, int itermax = 999);


/*
  Conjugate Gradients solution of C*x = b

        | C(1,1) C(1,2) .  C(1,N)| |x(1)|     |b(1)|
        | C(2,1) C(2,2) .  C(2,N)| |x(2)|     |b(2)|
        | C(3,1) C(3,2) .  C(3,N)| | .  |  =  |b(3)|
        |   .      .    .    .   | |x(N)|     | .  |
        | C(M,1) C(M,2) .  C(M,N)|            |b(M)|

INPUT:
   C is M x N array    M >= N
   x(N) - initial guess for solution vector
   b(M) - r.h.s.
OUTPUT:
   x - solution vector
Temp Arrays
   g(N), d(N), r(M), y(M)      */
/* bool ConjGradLS(FArr2D& C, FArr1D& b, FArr1D& x, FArr1D& y, FArr1D& d, FArr1D& r,
                FArr1D& g, real eps, int itermax = 999);  */
bool ConjGradLS(FArr2D& C, FArr1D& b, FArr1D& x, real eps, int itermax = 999);

int index321(int i1, int i2, int i3, int l1, int l2, int l3, int n1, int n2, int n3);

void i3i2i1(int i, int& i1, int& i2, int& i3, int l1, int l2, int l3, int n1, int n2, int n3);

int index123(int i1, int i2, int i3, int l1, int l2, int l3, int n1, int n2, int n3);

void i1i2i3(int i, int& i1, int& i2, int& i3, int l1, int l2, int l3, int n1, int n2, int n3);


#endif
