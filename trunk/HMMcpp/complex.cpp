#include "complex.h"
#include "MatUtils.h"

//---------------------------------------------------------------------------

//#pragma package(smart_init)

Complex::Complex(const Complex& c) {Re = c.Re; Im = c.Im; }

Complex::Complex( double r, double i) { Re = r; Im = i; }

const Complex& Complex::operator=(const Complex& c)
{
  if(this != &c) {Re = c.Re; Im = c.Im;}
  return *this;
}

const Complex& Complex::operator=(const double r)
{
  Re = r; Im = 0.0;
  return *this;
}

/*  Can be used anywhere because of automatic conversion

  friend Complex operator +( const Complex &, const Complex & );

  Complex operator +(const Complex &c1, const Complex &c2)
  {
    double r = c1.Re + c2.Re, i = c1.Im + c2.Im;
    return Complex(r, i);
  } */

Complex Complex::operator+ (const Complex& c)
{
  return Complex(Re + c.Re, Im + c.Im);
}

Complex Complex::operator- (const Complex& c)
{
  return Complex(Re - c.Re, Im - Im*c.Im);
}

Complex Complex::operator* (const Complex& c)
{
  return Complex(Re*c.Re - Im*c.Im, Re*c.Im + Im*c.Re);
}

Complex Complex::operator/ (const Complex& c)
{
  double cc = mod2(c);
  return Complex((Re*c.Re + Im*c.Im)/cc, (Im*c.Re - Re*c.Im)/cc);
}

//scalar math

Complex Complex::operator+ (double r) { return Complex(Re + r, Im); }

Complex Complex::operator- (double r) { return Complex(Re - r, Im); }

Complex Complex::operator* (double r) { return Complex(Re*r, Im*r); }

Complex Complex::operator/ (double r) { return Complex(Re/r , Im/r); }

//scalar math where scalars come first

Complex operator+ (double r, const Complex& c) { return Complex(r+c.Re, c.Im); }

Complex operator- (double r, const Complex& c) { return Complex(r-c.Re, -c.Im); }

Complex operator* (double r, const Complex& c) { return Complex(r*c.Re, r*c.Im); }

Complex operator/ (double r, const Complex& c)
{
  double cc = mod2(c);
  return Complex(r*c.Re/cc, -r*c.Im/cc); 
}

real mod2(const Complex& c) { return (c.Re*c.Re + c.Im*c.Im); }

real mod(const Complex& c) { return sqrt(mod2(c)); }

bool Equal(Complex c1, Complex c2, double eps)
{
  return (fabs(c1.Re-c2.Re) < eps && fabs(c1.Im-c2.Im) < eps);
}

bool Equal(Complex c1, double r, double i, double eps)
{
  return (fabs(c1.Re-r) < eps && fabs(c1.Im-i) < eps);
}



