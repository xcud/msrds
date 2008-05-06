//---------------------------------------------------------------------------
// created:  20040808  by N.V.Shokhirev
// modified: 20050809  by N.V.Shokhirev
//---------------------------------------------------------------------------
//#include <vcl.h>
#pragma hdrstop

#include "MatUtils.h"

//---------------------------------------------------------------------------

//#pragma package(smart_init)

double sqr(double x) { return (x*x); }

//double abs(double x)
//{
//	return ( (x >= 0.0) ? x : -x); 
//}

double abs1(double x)
{ return ( (x >= 0.0) ? x : -x); }

// FORTRAN signum
double sign(double x1, double x2)
{ return ( (x2 > 0.0) ? abs(x1) : -abs(x1) ); }

double max1(double x1, double x2)
{ return ( (x1 > x2) ? x1 : x2); }

double min1(double x1, double x2)
{ return ( (x1 < x2) ? x1 : x2); }

int max0(int x1, int x2)
{ return ( (x1 > x2) ? x1 : x2); }

int min0(int x1, int x2)
{ return ( (x1 < x2) ? x1 : x2); }

double round1(double x)
{ return floor(x+0.5);};

int round0(double x)
{ return int(round1(x)); }

int iSign (int x)
{
  if (x > 0) { return 1; }
  else if (x == 0)  { return 0; }
  else { return -1; };
}

double sign0(double x)
{
  if ( x > 0.0 ) { return 1.0; }
  else if ( x < 0.0 ) { return -1.0; }
  else { return 0.0; };
}

double Sign1 (double X)
{
  if ( X < 0.0 ) { return -1.0; }
  else { return 1.0; };
}

// FORTRAN signum
double Sign2(double a, double b)
{
  if ( b < 0.0 ) { return -abs(a); }
  else { return abs(a); };
}

// safe sqrt: sqroot = sqrt(x + y)
double sqroot1(double x, double y)
{
  if ((x + y) <= 0) { return 0.0; }
  else if (x > y) { return sqrt(x)*sqrt(1.0 + y/x); }
  else { return sqrt(y)*sqrt(1.0 + x/y); };
}

// safe sqrt: sqroot2 = sqrt(x*x+y*y); functionally the same as pythag
double sqroot2(double x, double y)
{
  if (abs(x) > abs(y)) { return abs(x)*sqrt(1.0+sqr(y/x)); }
  else if (abs(y) == 0.0) { return 0.0; }
  else { return abs(y)*sqrt(1.0+sqr(x/y)); };
}

// finds dsqrt(a**2+b**2) without overflow or destructive underflow;
// EISPACK algorithm; functionally the same as sqroot2
double pythag(double a, double b)
{
  double p, r;
  p = max1(abs(a),abs(b));
  if (p == 0.0) { return 0.0; };
  r = min1(abs(a),abs(b));
  if (r < SqrtMinReal*p) { return p; }
  else { return p*sqrt(1.0 + sqr(r/p)); };
}

double max3(double x1, double x2, double x3){ return max1(max1(x1,x3),x2); }

double min3(double x1, double x2, double x3){ return min1(min1(x1,x3),x2); }

double SafeDiv(double x1, double x2)
{
  if (abs(x2) > SafeMinReal) return x1/x2; else return 0.0;
}

double Zero(double x, double eps)
{
  if ( abs(x)> eps) return x; else return 0.0;
}

bool Equal(double x1, double x2, double eps) { return (fabs(x1-x2) < eps); };


