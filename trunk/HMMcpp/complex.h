//---------------------------------------------------------------------------

#ifndef complexH
#define complexH
#include "MatUtils.h"

//---------------------------------------------------------------------------

class Complex {

public:
  double Re, Im;

  Complex::Complex(const Complex& c); //copy-constructor

  Complex( double r = 0.0, double i = 0.0 );     //constructor

  const Complex& operator=(const Complex& c);
  const Complex& operator=(const double r);

  Complex operator* (const Complex& c);
  Complex operator/(const Complex& c);
  Complex operator-(const Complex& c);
  Complex operator+(const Complex& c);

//scalar math
  Complex operator*(double r);
  Complex operator/(double r);
  Complex operator-(double r);
  Complex operator+(double r);

//scalar math where scalars come first
  friend Complex operator*(double r, const Complex& c);
  friend Complex operator/(double r, const Complex& c);
  friend Complex operator-(double r, const Complex& c);
  friend Complex operator+(double r, const Complex& c);

//private:

};

real mod2(const Complex& c);

real mod(const Complex& c);

bool Equal(Complex c1, Complex c2, double eps = eps0);

bool Equal(Complex c1, double r, double i, double eps = eps0);

#endif
