//---------------------------------------------------------------------------

// created:  20040808  by N.V.Shokhirev
// modified: 200408930  by N.V.Shokhirev
//---------------------------------------------------------------------------
#ifndef MatUtilsH
#define MatUtilsH

#include <math.h>
#define eps0 1.0e-9

//---------------------------------------------------------------------------
// typedef double real;  <- in dynarrays.h

typedef double real;

// Macheps + 1.0 > 1.0
//const double Macheps = 1.0842021724855E-19;
const double Macheps = 2.22044604925031E-16;

// SafeMacheps = 1024*Macheps + 1.0 > 1.0
//const double SafeMacheps = 1.110223024625152E-16;
const double SafeMacheps = 2.27373675443231744-13;

// SafeMinDouble > 0.0
const double MinReal = 4.94065645841247E-324;

const double SqrtMinReal = 2.222758749485E-162;

// SafeMinDouble = 1024*MinDouble
const double SafeMinReal = 5.05923221341436928e-321;

// VerySafeMinDouble = 1048576*MinDouble
const double VerySafeMinReal = 5.18065378653631e-318;

// SafeMinDouble = 1024*MinDouble
// const double SafeMinDouble = 5.05923221341436928e-321;

// Pascal x**2
double sqr(double x);

//double abs(double x);

// FORTRAN signum
double sign(double x1, double x2);

double max1(double x1, double x2);

double min1(double x1, double x2);

int max0(int x1, int x2);

int min0(int x1, int x2);

double round1(double x);

int round0(double x);

int iSign (int x);

double sign0(double x);

double Sign1 (double  X );

// FORTRAN signum
double Sign2(double a, double b);

// safe sqrt: sqroot = sqrt(x + y)
double sqroot1(double x, double y);

// safe sqrt: sqroot2 = sqrt(x*x+y*y); functionally the same as pythag
double sqroot2(double x, double y);

// finds dsqrt(a**2+b**2) without overflow or destructive underflow;
// EISPACK algorithm ; functionally the same as sqroot2
double pythag(double a, double b);

double max3(double x1, double x2, double x3);

double min3(double x1, double x2, double x3);

//  if (abs(x2) > SafeMinDouble) return x1/x2; else return 0.0;
double SafeDiv(double x1, double x2);

double Zero(double x, double eps = 1.0e-9);

bool Equal(double x1, double x2, double eps);



#endif
