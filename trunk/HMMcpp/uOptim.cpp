//---------------------------------------------------------------------------


//#pragma hdrstop

#include "uOptim.h"
#include "MatUtils.h"

//---------------------------------------------------------------------------

//#pragma package(smart_init)

const double SqrtMacheps = 1.4901161193848e-8; // = sqrt(2.22044604925031e-16)

/*
  Routine from the book
      Forsythe, G. E., M. A. Malcolm and C. Moler,
      Computer Methods for Mathematical Computations,
      Prentice-Hall, Englewood Cliffs, NJ, 1977.

  An approximation x to the point where f attains a minimum
  on the interval (ax,bx) is determined.

input..

  ax  - left endpoint of initial interval
  bx  - right endpoint of initial interval
  f   - function subprogram which evaluates f(x) for any x
        in the interval  (ax,bx)
  tol - desired length of the interval of uncertainty of the final
        result ( .ge. 0.0)

output..

  fmin  abcissa approximating the point where  f  attains a minimum

    The method used is a combination of golden section search and successive
  parabolic interpolation. Convergence is never much slower than that for a
  fibonacci search. If f has a continuous second derivative which is positive
  at the minimum (which is not at ax or bx), then convergence is superlinear,
  and usually of the order of about 1.324....
    The function f is never evaluated at two points closer together than
  eps*abs(fmin) + (tol/3), where eps is approximately the square root of the
  relative machine precision. If f is a unimodal function and the computed
  values of f are always unimodal when separated by at least
  eps*abs(x) + (tol/3), then fmin approximates the abcissa of the global minimum
  of f on the interval ax, bx with an error less than 3*eps*abs(fmin) + tol.
  If f is not unimodal, then fmin may approximate a local, but perhaps
  non-global, minimum to the same accuracy.
    This function subprogram is a slightly modified version of the algol 60
  procedure localmin given in
    Richard Brent, Algorithms for  minimization without derivatives,
    Prentice - Hall, Inc. (1973).
*/



//
//double fmin(double ax, double bx, double (*f)(double), double tol)
//{
////  double ax,bx,f,tol;
//  double  a,b,c,d,e,eps,xm,p,q,r,tol1,tol2,u,v,w;
//  double  fu,fv,fw,fx,x;
////  double  abs,dsqrt,dsign;
//
////  c is the squared inverse of the golden ratio
//
//      c = 0.5*(3.0 - sqrt(5.0));
//
////  SqrtMacheps is the square root of the relative machine precision.
//
////  initialization
//      a = ax;
//      b = bx;
//      v = a + c*(b - a);
//      w = v;
//      x = v;
//      e = 0.0;
//      fx = f(x);
//      fv = fx;
//      fw = fx;
//
////  main loop starts here
//   {
//   20 xm = 0.5*(a + b);
//      tol1 = SqrtMacheps*abs(x) + tol/3.0;
//      tol2 = 2.0*tol1;
//
////  check stopping criterion
//      if (abs(x - xm)  <=  (tol2 - 0.5*(b - a))) goto 90;
//
//// is golden-section necessary
//      if (abs(e)  <=  tol1) goto 40;
//
////  fit parabola
//      r = (x - w)*(fx - fv);
//      q = (x - v)*(fx - fw);
//      p = (x - v)*q - (x - w)*r;
//      q = 2.00*(q - r);
//      if (q  >  0.0) p = -p;
//      q =  abs(q);
//      r = e;
//      e = d;
//
////  is parabola acceptable
//      if (abs(p)  >=  abs(0.5*q*r)) goto 40;
//      if (p  <=  q*(a - x)) goto 40;
//      if (p  >=  q*(b - x)) goto 40;
//
////  a parabolic interpolation step
//      d = p/q;
//      u = x + d;
//
////  f must not be evaluated too close to ax or bx
//      if ((u - a) < tol2) d = sign(tol1, xm - x);
//      if ((b - u) < tol2) d = sign(tol1, xm - x);
//      goto 50;
//
////  a golden-section step
//   40 if (x  >=  xm) e = a - x;
//      if (x < xm) e = b - x;
//      d = c*e;
//
////  f must not be evaluated too close to x
//   50 if (abs(d)  >=  tol1) u = x + d;
//      if (abs(d) < tol1) u = x + sign(tol1, d);
//      fu = f(u);
//
////  update  a, b, v, w, and x
//      if (fu  >  fx) goto 60;
//      if (u  >=  x) a = x;
//      if (u < x) b = x;
//      v = w;
//      fv = fw;
//      w = x;
//      fw = fx;
//      x = u;
//      fx = fu;
//      goto 20;
//
//   60 if (u < x) a = u;
//      if (u  >=  x) b = u;
//      if (fu  <=  fw) goto 70;
//      if (w  ==  x) goto 70;
//      if (fu  <=  fv) goto 80;
//      if (v  ==  x) goto 80;
//      if (v  ==  w) goto 80;
//      goto 20;
//   70 v = w;
//      fv = fw;
//      w = u;
//      fw = fu;
//      goto 20;
//   80 v = u;
//      fv = fu;
//      goto 20;
//  } //  end of main loop
//
//   90 fmin = x;
//      return;
//      };//
