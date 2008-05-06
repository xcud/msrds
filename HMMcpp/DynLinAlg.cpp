//---------------------------------------------------------------------------
// N.V.Shokhirev
// created: 20040809
// modified: 20050809
//---------------------------------------------------------------------------
#pragma hdrstop

#include <math.h>
#include "DynLinAlg.h"
#include "MatUtils.h"
#include "dynarrutils.h"

//---------------------------------------------------------------------------

//#pragma package(smart_init)

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
void strid(FArr1D& b, FArr1D& d, FArr1D& a, FArr1D& c)
{
  int h = d.H1();
  int l = d.L1();
  double r;
//  Establish upper triangular matrix
  for (int j = l+1; j <= h; j++)
  {
    r    = b(j) / d(j-1);
    d(j) = d(j) - r*a(j-1);
    c(j) = c(j) - r*c(j-1);
  };

//  Back substitution
  c(h) = c(h) / d(h);
  for (int j = h-1; j >= l; j--)
  {
    c(j) = ( c(j) - a(j) * c(j+1) ) / d(j);
  };
} // strid <-----------------------------------------------------------------


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
       m - tri-dialonal matrix    */
void TridTo2D(FArr1D& b, FArr1D& d, FArr1D& a, FArr2D& m)
{
  int h = d.H1();
  int l = d.L1();
  assert( a.L1() == l && a.H1() == h &&  b.L1() == l && b.H1() == h );
  assert( m.L1() == l && m.H1() == h &&  m.L2() == l && m.H2() == h );
  m.fill(0.0);
  m(l, l ) = d(l);
  for (int j = l+1; j <= h; j++)
  {
    m(j,j-1) = b(j);
    m(j, j ) = d(j);
    m(j-1,j) = a(j-1);
  };
} // TridTo2D <--------------------------------------------------------------


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
bool GaussD(FArr2D& M)
{
  int h1 = M.H1();
  int l1 = M.L1();
  int h2 = M.H2();
  double t;

  assert( l1 == M.L2() );

  for (int i = l1; i <= h1; i++)
  {
    t = M(i,i);
    if(abs(t) < SafeMinReal) return false;
    for (int j = i; j <= h2; j++) M(i,j) = M(i,j)/t;

    for (int k = l1; k <= h1; k++)
      if (k != i)
      {
        t = M(k,i);
        if(abs(t)>SafeMinReal)
          for (int j = i; j <= h2; j++) M(k,j) = M(k,j)-M(i,j)*t;
      };
  };
  return true;
} // GaussD <----------------------------------------------------------------


/*  Jacobi iterative method for M * x = b

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
bool JacobiIter(FArr2D& M, FArr1D& x, FArr1D& b, real eps, int itermax)
{
  int h = M.H1();
  int l = M.L1();
  int iter = 0;
  double delta, sum;
  FArr1D y(l,h);

  assert( (l == M.L2()) && (h == M.H2()) );
  do
  {
    iter++;
    delta = 0.0;
    for (int i = l; i <= h; i++)
    {
      sum = 0.0;
      y = x;
      for (int j = l; j <= h; j++)
        if (j != i) sum += M(i,j)*x(j);
      x(i) = (b(i)-sum)/M(i,i);
      delta = max1(delta, abs(x(i)-y(i)));
    };  
  } while ( (delta > eps) && (iter < itermax) );
  return (iter < itermax);
} // JacobiIter <------------------------------------------------------------


/*  Gauss elimination with partial pivoting

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
bool GaussP(FArr2D& M)
{
  int h1 = M.H1();
  int l1 = M.L1();
  int h2 = M.H2();
  int ii;
  double t;

  assert( l1 == M.L2() );

  for (int i = l1; i <= h1; i++)
  {
    // search for the largest element
    t = 0.0;  ii = i;
    for (int j = i; j <= h1; j++) if(abs(M(j,i)) > t){ t = abs(M(j,i)); ii = j;}

    if(t < SafeMinReal) return false;
    if(ii > i) M.swap1(ii,i);

    t = M(i,i);
    for (int j = i; j <= h2; j++) M(i,j) = M(i,j)/t;

    for (int k = l1; k <= h1; k++)
      if (k != i)
      {
        t = M(k,i);
        if(abs(t)>SafeMinReal)
          for (int j = i; j <= h2; j++) M(k,j) = M(k,j)-M(i,j)*t;
      };
  };
  return true;
} // GaussP <----------------------------------------------------------------

/*  Conjugate Gradients solution of A*x = b

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
   x - solution vector          */
bool ConjGrad(FArr2D& A, FArr1D& b, FArr1D& x, real eps, int itermax)
{
  double Pold, Pnew, beta, gamma;
  int h1 = A.H1();
  int l1 = A.L1();
  int iter = 0;
  int k = sqrt((float)A.D2());
  //  Temp Arrays d, r, y
  FArr1D y(l1,h1);
  FArr1D d(l1,h1);
  FArr1D r(l1,h1);

  assert( l1 == A.L2() && h1 == A.H2() );

  r = b - A*x;
  Pold = Norm2(r);  //  Pold = r*r;
  d = r;

  while ((Pold > eps) && (iter <= itermax))
  {
    y = A*d;
    beta = Pold/(d*y);
    x = x + beta*d;
    if (iter % k == 0) r = b - A*x;
    else r = r - beta*y;
    Pnew = Norm2(r);  // Pnew = r*r;
    gamma = Pnew/Pold;
    d = r + gamma*d;
    Pold = Pnew;
    iter++;
  };

  return (iter < itermax);
} // ConjGrad <--------------------------------------------------------------


/*  Conjugate Gradients solution of C*x = b

        | C(1,1) C(1,2) .  C(1,N)| |x(1)|     |b(1)|
        | C(2,1) C(2,2) .  C(2,N)| |x(2)|     |b(2)|
        | C(3,1) C(3,2) .  C(3,N)| | .  |  =  |b(3)|
        |   .      .    .    .   | |x(N)|     | .  |
        | C(M,1) C(M,2) .  C(M,N)|            |b(M)|
INPUT:
   C(M,N) array    M >= N
   x(N) - initial guess for solution vector
   b(M) - r.h.s.
OUTPUT:
   x(N) - solution vector     */
bool ConjGradLS(FArr2D& C, FArr1D& b, FArr1D& x, real eps, int itermax)
{
  double Pold, Pnew, beta, gamma;
  int h1 = C.H1();
  int l1 = C.L1();
  int h2 = C.H2();
  int l2 = C.L2();
  int iter = 0;
  int k = sqrt((float)C.D2());
  // Temp Arrays  g(N), d(N), r(M), y(M)
  FArr1D y(l1,h1);
  FArr1D d(l2,h2);
  FArr1D r(l1,h1);
  FArr1D g(l2,h2);

  r = b - C*x;
  g = r*C;
  Pold = Norm2(g);  //  Pold = g*g;
  d = g;

  while ((Pold > eps) && (iter <= itermax))
  {
    y = C*d;
    beta = Pold/Norm2(y);  //  beta = Pold/(y*y);
    x = x + beta*d;
    if (iter % k == 0) r = b - C*x;
    else r = r - beta*y;
    g = r*C;
    Pnew = Norm2(g);  //  Pnew = g*g;
    gamma = Pnew/Pold;
    d = g + gamma*d;
    Pold = Pnew;
    iter++;
  };
  return (iter < itermax);
} // ConjGradLS <------------------------------------------------------------


int index321(int i1, int i2, int i3, int l1, int l2, int l3, int n1, int n2, int n3)
{
  return (i1-l1 + n1*(i2-l2 + n2*(i3-l3)));
}

void i3i2i1(int i, int& i1, int& i2, int& i3, int l1, int l2, int l3, int n1, int n2, int n3)
{
  int n1n2 = n1*n2;
  i3 = i/n1n2+l3;  i %= n1n2;
  i2 = i/n1+l2;
  i1 = i % n1 + l1;
}

int index123(int i1, int i2, int i3, int l1, int l2, int l3, int n1, int n2, int n3)
{
  return (i3-l3 + n3*(i2-l2 + n2*(i1-l1)));
}

void i1i2i3(int i, int& i1, int& i2, int& i3, int l1, int l2, int l3, int n1, int n2, int n3)
{
  int n3n2 = n3*n2;
  i1 = i/n3n2+l1;  i %= n3n2;
  i2 = i/n3+l2;
  i3 = i % n3 + l3;
}


