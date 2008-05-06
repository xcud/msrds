//---------------------------------------------------------------------------
// created: 20040809  by N.V.Shokhirev
// modified: 20050809 by N.V.Shokhirev
//---------------------------------------------------------------------------
#pragma hdrstop

#include "dynarrutils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

bool SameLim(Lim1D& a1, Lim1D& a2)
{ return ( a1.L1()==a2.L1() && a1.H1()==a2.H1() ); };

bool SameLim(Lim2D& a1, Lim2D& a2)
{ return ( a1.L1()==a2.L1() && a1.H1()==a2.H1() &&
           a1.L2()==a2.L2() && a1.H2()==a2.H2() ); };

bool IsSquare(Lim2D& a)
{ return ( a.L1()==a.L2() && a.H1()==a.H2() ); };

double Diff(FArr1D& a1, FArr1D& a2)
{
  double d, t = 0.0;
  assert( SameLim(a1, a2) );
  for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ )
  {
    d = fabs(a2(i1)-a1(i1));
    if (d > t) t = d;
  };
  return t;
}

double Diff(FArr2D& a1, FArr2D& a2)
{
  double d, t = 0.0;
  assert( SameLim(a1, a2) );
  for(int i2 = a1.L2(); i2 <= a1.H2(); i2++ )
    for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ )
    {
      d = fabs(a2(i1,i2)-a1(i1,i2));
      if (d > t) t = d;
    };
  return t;
}

double minval(FArr1D& a, int& i)
{
  i = a.H1();
  double t = a(i);
  for(int i1 = a.L1(); i1 < a.H1(); i1++ )
    if (a(i1) < t) { t = a(i1); i = i1; };
  return t;
}

double maxval(FArr1D& a, int& i)
{
  i = a.H1();
  double t = a(i);
  for(int i1 = a.L1(); i1 < a.H1(); i1++ )
    if (a(i1) > t) { t = a(i1); i = i1; };
  return t;
}

double maxabs(FArr1D& a, int& i)
{
  i = a.L1();
  double t = 0.0;
  for(int i1 = a.L1(); i1 <= a.H1(); i1++ )
    if (fabs(a(i1)) > t) { t = a(i1); i = i1; };
  return t;
}

void minmax(FArr1D& a, double& amin, double& amax, bool reset)
{
  if (reset)
  {
    amin = a(a.H1());
    amax = amin;
  };

  for(int i1 = a.L1(); i1 < a.H1(); i1++ )
  {
    if (a(i1) > amax) amax = a(i1);
    if (a(i1) < amin) amin = a(i1);
  };
}

//bool Equal(double x1, double x2, double eps) { return (fabs(x1-x2) < eps); };

bool Equal(IArr1D& a1, IArr1D& a2)
{
  bool d = true;
  assert( SameLim(a1, a2) );
  for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ )
    if (a2(i1) != a1(i1)) d = false;
  return d;
}

bool Equal(FArr1D& a1, FArr1D& a2, double eps) { return (Diff(a1, a2) < eps); }

bool Equal(FArr2D& a1, FArr2D& a2, double eps){ return (Diff(a1, a2) < eps); }

void RandArr(IArr1D& a, int v1, int v2)
{
  for(int i1 = a.L1(); i1 <= a.H1(); i1++ )
    a(i1) = v1 + (v2 - v1)*( (int) rand()/RAND_MAX );
}

void RandArr(FArr1D& a, double v1, double v2)
{
  for(int i1 = a.L1(); i1 <= a.H1(); i1++ )
    a(i1) = v1 + (v2 - v1)*( (double) rand()/RAND_MAX );
}

void RandArr(FArr2D& a, double v1, double v2, int sym)
{
  if (sym == 0)
  {
    for(int i2 = a.L2(); i2 <= a.H2(); i2++ )
      for(int i1 = a.L1(); i1 <= a.H1(); i1++ )
        a(i1,i2) = v1 + (v2 - v1)*( (double) rand()/RAND_MAX );
  }
  else
  {
    assert( IsSquare(a) );
    if (sym>0)
    {
      for(int i2 = a.L1(); i2 < a.H1(); i2++ )
      {
        for(int i1 = i2+1; i1 <= a.H1(); i1++ )
        {
          a(i1,i2) = v1 + (v2 - v1)*( (double) rand()/RAND_MAX );
          a(i2,i1) = a(i1,i2);
        };
        a(i2,i2) = v1 + (v2 - v1)*( (double) rand()/RAND_MAX );
      };
    }
    else
    {
      for(int i2 = a.L1(); i2 < a.H1(); i2++ )
      {
        for(int i1 = i2+1; i1 <= a.H1(); i1++ )
        {
          a(i1,i2) = v1 + (v2 - v1)*( (double) rand()/RAND_MAX );
          a(i2,i1) = -a(i1,i2);
        };
        a(i2,i2) = 0.0;
      };
    };
  };
}

void ShiftDiag(FArr2D& a, double v)
{
  assert( IsSquare(a) );
  for(int i1 = a.L1(); i1 <= a.H1(); i1++ )
    a(i1,i1) = a(i1,i1) + v;
}

void MultArr(FArr1D& a, double v)
{ for(int i1 = a.L1(); i1 <= a.H1(); i1++ ) a(i1) = a(i1) * v; };

void MultArr(FArr2D& a, double v)
{
  for(int i2 = a.L2(); i2 <= a.H2(); i2++ )
    for(int i1 = a.L1(); i1 <= a.H1(); i1++ )
      a(i1,i2) = a(i1,i2) * v;
}

double Norm2(FArr1D& a1)
{
  double t = 0.0;
  for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ ) t += sqr(a1(i1));
  return t;
}

double Dot(FArr1D& a1, FArr1D& a2)
{
  double t = 0.0;
  assert( SameLim(a1, a2) );
  for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ )
      t += a2(i1) * a1(i1);
  return t;
}

FArr1D Combine(double v1, FArr1D& a1, double v2, FArr1D& a2)
{
  assert( SameLim(a1, a2) );
  FArr1D temp(a1.L1(),a1.H1());
  for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ )
    temp(i1) = v1*a1(i1) +v2*a2(i1);
  return temp;
}

FArr2D Combine(double v1, FArr2D& a1, double v2, FArr2D& a2)
{
  assert( SameLim(a1, a2) );
  FArr2D temp(a1.L1(),a1.H1(), a1.L2(),a1.H2());
  for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ )
    for(int i2 = a1.L2(); i2 <= a1.H2(); i2++ )
      temp(i1,i2) = v1*a1(i1,i2) +v2*a2(i1,i2);
  return temp;
}

FArr2D MT(FArr2D& a)
{
  FArr2D temp(a.L2(),a.H2(), a.L1(),a.H1());
  for(int i1 = a.L1(); i1 <= a.H1(); i1++ )
    for(int i2 = a.L2(); i2 <= a.H2(); i2++ )
      temp(i2,i1) = a(i1,i2);
  return temp;
}

FArr1D MxV(FArr2D& a1, FArr1D& a2)
{
  double t = 0.0;
  assert( a1.L2() == a2.L1() && a1.H2() == a2.H1() );
  FArr1D temp(a1.L1(),a1.H1());
  for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ )
  {
    t = 0.0;
    for(int i2 = a1.L2(); i2 <= a1.H2(); i2++ )
      t += a1(i1,i2)*a2(i2);
    temp(i1) = t;
  };
  return temp;
}

FArr1D VxM(FArr1D& a1, FArr2D& a2)
{
  double t = 0.0;
  assert( a1.L1() == a2.L1() && a1.H1() == a2.H1() );
  FArr1D temp(a2.L2(),a2.H2());
  for(int i2 = a2.L2(); i2 <= a2.H2(); i2++ )
  {
    t = 0.0;
    for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ )
      t += a1(i1)*a2(i1,i2);
    temp(i2) = t;
  };
  return temp;
}

FArr2D MxM(FArr2D& a1, FArr2D& a2)
{
  double t = 0.0;
  assert( a1.L2() == a2.L1() && a1.H2() == a2.H1() );
  FArr2D temp(a1.L1(),a1.H1(), a2.L2(),a2.H2());
  for(int i1 = a1.L1(); i1 <= a1.H1(); i1++ )
    for(int i2 = a2.L2(); i2 <= a2.H2(); i2++ )
    {
      t = 0.0;
      for(int k = a1.L2(); k <= a1.H2(); k++ )
      t += a1(i1,k)*a2(k,i2);
    temp(i1,i2) = t;
    };
  return temp;
}

/*
 Fourier transform T0[j] corresponds to non-centered frequency          <br>                  <br>
         Q[j] = j*2*Pi/(dT*N)   j = 0, 1, 2, .. , N-1                   <br>
 Centered frequency:     F[j] = j*2*Pi/(dT*N)   Mmin <= j <= Mmax       <br>
 where Mmin = -((N-1) div 2), Mmax = (M div 2)                          <br>
 The transform corresponding to the centered frequency                  <br>
              C[m] = T0[ m ]  0 <= m <= Mmax                            <br>
              C[m] = T0[N-m]  Mmin <= m < 0                             <br>
 */
void CenterToTrans(FArr1D& T, FArr1D& C)
{
  int N = T.D1();
//  int Mmax = N / 2;
  int Mmin = -((N-1)/ 2);
  assert(C.D1()==N);
  T.SetL1(0);
  C.SetL1(Mmin);
  for (int m = C.L1(); m <= C.H1(); m++)
    if(m<0)
      { C(m) = T(N-m); }
    else
      { C(m) = T(m); };
}

void TransToCenter(FArr1D& T, FArr1D& C)
{
  int N = C.D1();
//  int Mmax = N / 2;
  int Mmin = -((N-1)/ 2);
  assert(T.D1()==N);
  T.SetL1(0);
  C.SetL1(Mmin);
  for (int m = C.L1(); m <= C.H1(); m++)
    if(m<0)
      { T(N-m) = C(m); }
    else
      { T(m)   = C(m); };
}

int cycle(int i, int L, int H, int D)
{
  if(i < L) i += D; else if (i > H) i -= D;
  return i;
}

double extend(FArr1D& a, int i, bool cyclic)
{
  if (cyclic) // a(i+D) = a(i-D) = a(i)
  {
    if (i < a.L1()) i += a.D1(); else if (i > a.H1()) i -= a.D1();
    return a(i);
  }
  else // 0 outside limits
  {
    if (i <= a.H1() && a.L1() <= i)
      return a(i);
    else
      return 0.0;
  };
}

double extend0(FArr1D& a, int i, bool cyclic)
{

  if (cyclic) // a[0..N] a(0) = a(N);  a[0] is not used
  {
    int N = a.D1()-1;
    if (i <= a.L1()) i += N; else if (i > a.H1()) i -= N;
    return a(i);
  }
  else // 0 outside limits
  {
    if (i <= a.H1() && a.L1() <= i)
      return a(i);
    else
      return 0.0;
  };
}

//  5-point smoothing
// p1 = p0 - OK
void Smooth5(FArr1D& p0, FArr1D& p1)
{
  int l = p0.L1();
  int h = p0.H1();
  double mm = 0.0;
  double m  = p0(l);
  double z  = p0(l+1);
  double p  = p0(l+2);
  double pp = p0(l+3);

  p1(l)   = (31.0*m +9.0*z -3.0*p-5.0*pp+3.0*p0(l+4))/35.0;
  p1(l+1) = ( 9.0*m+13.0*z+12.0*p+6.0*pp-5.0*p0(l+4))/35.0;

  for (int i = l+2; i <= h-2; i++)
  {
    mm = m;  m = z;  z = p;  p = pp;  pp = p0(i+2);
    p1(i)   = (-3.0*mm+12.0*m+17.0*z+12.0*p-3.0*pp)/35.0;
  };

  p1(h-1) = (-5.0*mm+6.0*m+12.0*z+13.0*p +9.0*pp)/35.0;
  p1(h)   = ( 3.0*mm-5.0*m -3.0*z +9.0*p+31.0*pp)/35.0;
} // Smooth5 <--------------------------------------------------------------

