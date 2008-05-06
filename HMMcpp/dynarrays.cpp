//---------------------------------------------------------------------------
// N.V.Shokhirev
// created:  20041020
// modified: 20051020
//---------------------------------------------------------------------------

#include "dynarrays.h"
#include <math.h>

//---------------------------------------------------------------------------

FArr1D operator+(const FArr1D& a1, const FArr1D& a2)
{
  assert( a1.l1 == a2.l1 && a1.h1 == a2.h1 );  // Check lim match.
  FArr1D temp(a1.l1, a1.h1);
  for (int i = a1.l1; i <= a1.h1; i++) temp(i) = a1(i) + a2(i);
  return temp;
}

FArr1D operator-(const FArr1D& a1, const FArr1D& a2)
{
  assert( a1.l1 == a2.l1 && a1.h1 == a2.h1 );  // Check lim match.
  FArr1D temp(a1.l1, a1.h1);
  for (int i = a1.l1; i <= a1.h1; i++) temp(i) = a1(i) - a2(i);
  return temp;
}

double operator * (const FArr1D& a1, const FArr1D& a2)
{
  double temp = 0.0;
  assert( a1.l1 == a2.l1 && a1.h1 == a2.h1 );  // Check lim match.
  for (int i = a1.l1; i <= a1.h1; i++) temp += a1(i)*a2(i);
  return temp;
}

FArr1D operator*(const FArr1D& a1, const double x)
{
  FArr1D temp(a1.l1, a1.h1);
  for (int i = a1.l1; i <= a1.h1; i++) temp(i) = a1(i)*x;
  return temp;
}

FArr2D operator*(const FArr2D& a1, const FArr2D& a2)
{
  double sum;
  assert( a1.l2 == a2.l1 && a1.h2 == a2.h1);  // Check lim match.
  FArr2D t(a1.l1, a1.h1, a2.l2, a2.h2);
  for (int i1 = t.l1; i1 <= t.h1; i1++)
    for (int i2 = t.l2; i2 <= t.h2; i2++)
    {
      sum = 0.0;
      for (int k = a2.l1; k <= a2.h1; k++)
        sum += a1(i1,k) * a2(k,i2);
    t(i1,i2) = sum;
    };
  return t;
}

//---------------------------------------------------------------------------

FArr2D operator+(const FArr2D& a1, const FArr2D& a2)
{
  assert( a1.l1 == a2.l1 && a1.h1 == a2.h1 &&
          a1.l2 == a2.l2 && a1.h2 == a2.h2);  // Check lim match.
  FArr2D temp(a1.l1, a1.h1, a1.l2, a1.h2);
  for (int i1 = a1.l1; i1 <= a1.h1; i1++)
    for (int i2 = a1.l2; i2 <= a1.h2; i2++) temp(i1,i2) = a1(i1,i2) + a2(i1,i2);
  return temp;
}

FArr2D operator-(const FArr2D& a1, const FArr2D& a2)
{
  assert( a1.l1 == a2.l1 && a1.h1 == a2.h1 &&
          a1.l2 == a2.l2 && a1.h2 == a2.h2);  // Check lim match.
  FArr2D temp(a1.l1, a1.h1, a1.l2, a1.h2);
  for (int i1 = a1.l1; i1 <= a1.h1; i1++)
    for (int i2 = a1.l2; i2 <= a1.h2; i2++) temp(i1,i2) = a1(i1,i2) - a2(i1,i2);
  return temp;
}

FArr2D operator*(const FArr2D& a1, const double x)
{
  FArr2D temp(a1.l1, a1.h1, a1.l2, a1.h2);
  for (int i1 = a1.l1; i1 <= a1.h1; i1++)
    for (int i2 = a1.l2; i2 <= a1.h2; i2++) temp(i1,i2) = a1(i1,i2) * x;
  return temp;
}

FArr1D operator*(const FArr2D& a1, const FArr1D& a2)
{
  double sum = 0.0;
  assert( a1.l2 == a2.L1() && a1.h2 == a2.H1());  // Check lim match.
  FArr1D temp(a1.l1, a1.h1);
  for (int i1 = a1.l1; i1 <= a1.h1; i1++)
  {
    sum = 0.0;
    for (int i2 = a1.l2; i2 <= a1.h2; i2++) sum += a1(i1,i2) * a2(i2);
    temp(i1) = sum;
  }
  return temp;
}

//
FArr1D operator*(const FArr1D& a1, const FArr2D& a2)
{
  double sum = 0.0;
  assert( a2.l1 == a1.L1() && a2.h1 == a1.H1());  // Check lim match.
  FArr1D temp(a2.l2, a2.h2);
  for (int i2 = a2.l2; i2 <= a2.h2; i2++)
  {
    sum = 0.0;
    for (int i1 = a2.l1; i1 <= a2.h1; i1++) sum += a1(i1) * a2(i1, i2);
    temp(i2) = sum;
  }
  return temp;
}

// Lim1D =============================================================

// Default Constructor: Creates a 1 - vector;
Lim1D::Lim1D()
{
  d1 = 1;
  l1 = 1;
  h1 = 1;
}

// 1-based Constructor: Creates an aD1 - vector;
Lim1D::Lim1D(int aD1)
{
  assert( aD1 > 0 );    // Check that Dim1 > 0.
  d1 = aD1;
  SetL1(1);
}

// General Constructor: Creates an aD1 - vector;
Lim1D::Lim1D(int aL1, int aH1)
{
  assert( aH1 >= aL1 );    // Check High >= Low.
  d1 = aH1-aL1+1;
  SetL1(aL1);
}

// Protected copy function: Copies values from one Lim1D object to another.
void Lim1D::copy(const Lim1D& lim)
{
  d1 = lim.d1;
  l1 = lim.l1;
  h1 = lim.h1;
}

bool Lim1D::eq(Lim1D& lim)
{
  return ( l1 == lim.L1() &&  h1 == lim.H1() );  // Check lim match.
}

// Lim2D =============================================================

// Default Constructor: Creates a 1 by 1 matrix;
Lim2D::Lim2D()
{
  d2 = 1;
  l2 = 1;
  h2 = 1;
}

// 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
Lim2D::Lim2D(int aD1, int aD2) :Lim1D(aD1)
{
  assert(aD2 > 0);    // Check that Dim1 and Dim2 both > 0.
  d2 = aD2;
  SetL2(1);
}

// General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
Lim2D::Lim2D(int aL1, int aH1, int aL2, int aH2) : Lim1D(aL1, aH1)
{
  assert(aH2 >= aL2);    // Check High >= Low.
  d2 = aH2-aL2+1;
  SetL2(aL2);
}

// Protected copy function: Copies values from one Lim2D object to another.
void Lim2D::copy(const Lim2D& lim)
{
  Lim1D::copy(lim);
  d2 = lim.d2;
  l2 = lim.l2;
  h2 = lim.h2;
}

// Lim3D =============================================================

// Default Constructor: Creates a 1 x 1 x 1 matrix;
Lim3D::Lim3D()
{
  d3 = 1;
  l3 = 1;
  h3 = 1;
}

// 1-based Constructor: Creates an aD1 by aD1 matrix;
Lim3D::Lim3D(int aD1, int aD2, int aD3) :Lim2D(aD1, aD2)
{
  assert(aD3 > 0);    // Check that Dim1 and Dim2 both > 0.
  d3 = aD3;
  SetL3(1);
}

// General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
Lim3D::Lim3D(int aL1, int aH1, int aL2, int aH2, int aL3, int aH3)
     : Lim2D(aL1, aH1, aL2, aH2)
{
  assert(aH3 >= aL3);    // Check High >= Low.
  d3 = aH3-aL3+1;
  SetL3(aL3);
}

// Protected copy function: Copies values from one Lim2D object to another.
void Lim3D::copy(const Lim3D& lim)
{
  Lim2D::copy(lim);
  d3 = lim.d3;
  l3 = lim.l3;
  h3 = lim.h3;
}

// FArr1D =============================================================

// Default Constructor: Creates a 1 - vector; sets value to zero.
FArr1D::FArr1D()
{
  dd = d1;
  data = new double [dd];  // Allocate memory
  fill(0.0);               // Set value of data_[0] to 0.0
}

// 1-based Constructor: Creates an aD1 - vector; sets values to zero.
FArr1D::FArr1D(int aD1) : Lim1D(aD1)
{
  dd = d1;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// General Constructor: Creates an aD1 - vector; sets values to zero.
FArr1D::FArr1D(int aL1, int aH1) : Lim1D(aL1, aH1)
{
  dd = d1;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// Fills all elements of a matrix to a given value.
void FArr1D::fill(double value) {
  for(int i = 0; i < dd; i++ ) data[i] = value;
}

// Protected copy function: Copies values from one FArr1D object to another.
void FArr1D::copy(const FArr1D& vec)
{
  Lim1D::copy(vec);
  dd = vec.dd;
  data = new double [dd];
  for(int i = 0; i < dd; i++ ) data[i] = vec.data[i];
}

void FArr1D::swap1(int i1, int j1)
{
  double temp = data[ (i1-l1) ];
  data[ (i1-l1) ] = data[ (j1-l1) ];
  data[ (j1-l1) ] = temp;
};

double FArr1D::minval(int& i)
{
  i = h1;
  double q;
  double t = this->operator() (i);
  for(int i1 = l1; i1 < h1; i1++ )
  {
    q = this->operator() (i1);
    if (q < t) { t = q; i = i1; };
  };
  return t;
};

double FArr1D::maxval(int& i)
{
  double q;
  i = h1;
  double t = this->operator() (i);
  for(int i1 = l1; i1 < h1; i1++ )
  {
    q = this->operator() (i1);
    if (q > t) { t = q; i = i1; };
  };
  return t;
};

double FArr1D::maxabs(int& i)
{
  i = l1;
  double q, t = 0.0;
  for(int i1 = l1; i1 <= h1; i1++ )
  {
    q = fabs( this->operator() (i1) );
    if ( q > t) { t = q; i = i1; };
  };
  return t;
};

void FArr1D::minmax(double& amin, double& amax, bool reset)
{
  double q;
  if (reset)
  {
    amin = this->operator() (h1);
    amax = amin;
  };

  for(int i1 = l1; i1 < h1; i1++ )
  {
    q = this->operator() (i1);
    if (q > amax) amax = q;
    if (q < amin) amin = q;
  };
};


// FArr2D =============================================================

// Default Constructor: Creates a 1 by 1 matrix; sets value to zero.
FArr2D::FArr2D(){
  dd = 1;
  data = new double [dd];  // Allocate memory
  fill(0.0);                // Set value of data_[0] to 0.0
}

// 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
FArr2D::FArr2D(int aD1, int aD2) : Lim2D(aD1, aD2)
{
  dd = d1*d2;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
FArr2D::FArr2D(int aL1, int aH1, int aL2, int aH2) : Lim2D(aL1, aH1, aL2, aH2)
{
  dd = d1*d2;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// Fills all elements of a matrix to a given value.
void FArr2D::fill(double value) {
  for(int i = 0; i < dd; i++ ) data[i] = value;
}

// Private copy function: Copies values from one FArr2D object to another.
void FArr2D::copy(const FArr2D& mat) {
  Lim2D::copy(mat);
  dd = mat.dd;
  data = new double [dd];
  for(int i = 0; i < dd; i++ ) data[i] = mat.data[i];
}

void FArr2D::swap1(int i1, int j1)
{
  double t;
  int i = d2*(i1-l1);
  int j = d2*(j1-l1);
  for (int k = 0; k < d2; k++ )
  {
    t = data[i+k];
    data[i+k] = data[j+k];
    data[j+k] = t;
  };
};

void FArr2D::swap2(int i2, int j2)
{
  double t;
  int i = (i2-l1);
  int j = (j2-l1);
  for (int k = 0; k < d1; k++ )
  {
    t = data[i];
    data[i] = data[j];
    data[j] = t;
    i += d2;  j += d2;
  };
};

// IArr1D =============================================================

// Default Constructor: Creates a 1 - vector; sets value to zero.
IArr1D::IArr1D()
{
  dd = d1;
  data = new int [dd];  // Allocate memory
  fill(0.0);               // Set value of data_[0] to 0.0
}

// 1-based Constructor: Creates an aD1 - vector; sets values to zero.
IArr1D::IArr1D(int aD1) : Lim1D(aD1)
{
  dd = d1;
  data = new int [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// General Constructor: Creates an aD1 - vector; sets values to zero.
IArr1D::IArr1D(int aL1, int aH1) : Lim1D(aL1, aH1)
{
  dd = d1;
  data = new int [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// Fills all elements of a matrix to a given value.
void IArr1D::fill(int value) {
  for(int i = 0; i < dd; i++ ) data[i] = value;
}

// Protected copy function: Copies values from one IArr1D object to another.
void IArr1D::copy(const IArr1D& vec)
{
  Lim1D::copy(vec);
  dd = vec.dd;
  data = new int [dd];
  for(int i = 0; i < dd; i++ ) data[i] = vec.data[i];
}
void IArr1D::swap1(int i1, int j1)
{
  int temp = data[ (i1-l1) ];
  data[ (i1-l1) ] = data[ (j1-l1) ];
  data[ (j1-l1) ] = temp;
};

// IArr2D =============================================================

// Default Constructor: Creates a 1 by 1 matrix; sets value to zero.
IArr2D::IArr2D(){
  dd = 1;
  data = new int [dd];  // Allocate memory
  fill(0.0);                // Set value of data_[0] to 0.0
}

// 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
IArr2D::IArr2D(int aD1, int aD2) : Lim2D(aD1, aD2)
{
  dd = d1*d2;
  data = new int [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
IArr2D::IArr2D(int aL1, int aH1, int aL2, int aH2) : Lim2D(aL1, aH1, aL2, aH2)
{
  dd = d1*d2;
  data = new int [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// Fills all elements of a matrix to a given value.
void IArr2D::fill(int value) {
  for(int i = 0; i < dd; i++ ) data[i] = value;
}

// Private copy function: Copies values from one IArr2D object to another.
void IArr2D::copy(const IArr2D& mat) {
  Lim2D::copy(mat);
  dd = mat.dd;
  data = new int [dd];
  for(int i = 0; i < dd; i++ ) data[i] = mat.data[i];
}


// FArr3D =============================================================

// Default Constructor: Creates a 1 by 1 matrix; sets value to zero.
FArr3D::FArr3D(){
  dd = 1;
  data = new double [dd];  // Allocate memory
  fill(0.0);                // Set value of data_[0] to 0.0
}

// 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
FArr3D::FArr3D(int aD1, int aD2, int aD3) : Lim3D(aD1, aD2, aD3)
{
  dd = d1*d2*d3;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
FArr3D::FArr3D(int aL1, int aH1, int aL2, int aH2, int aL3, int aH3)
      : Lim3D(aL1, aH1, aL2, aH2, aL3, aH3)
{
  dd = d1*d2*d3;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// Fills all elements of a matrix to a given value.
void FArr3D::fill(double value) {
  for(int i = 0; i < dd; i++ ) data[i] = value;
}

// Private copy function: Copies values from one FArr2D object to another.
void FArr3D::copy(const FArr3D& mat) {
  Lim3D::copy(mat);
  dd = mat.dd;
  data = new double [dd];
  for(int i = 0; i < dd; i++ ) data[i] = mat.data[i];
}

void FArr3D::GetFArr2D(int i1, FArr2D& m)
{
  double t;
  int k = d3*d2*(i1-l1);
  for (int i2 = l2; i2 <= h2; i2++ )
    for (int i3 = l3; i3 <= h3; i3++ )
    {
  //    data[ d3*d2*(i1-l1) + d3*(i2-l2) + (i3-l3) ]
      m(i2,i3) = data[ k++ ];
    };
};

void FArr3D::SetFArr2D(int i1, FArr2D& m)
{
  double t;
  int k = d3*d2*(i1-l1);
  for (int i2 = l2; i2 <= h2; i2++ )
    for (int i3 = l3; i3 <= h3; i3++ )
    {
//      data[ d3*d2*(i1-l1) + d3*(i2-l2) + (i3-l3) ]
      data[ k++ ] = m(i2,i3);
    };
};


// Sparse2D =============================================================

// Default Constructor: Creates a 1 by 1 matrix; sets value to zero.
Sparse2D::Sparse2D(){
  dd = 1;
  data = new double [dd];  // Allocate memory
  fill(0.0);                // Set value of data_[0] to 0.0
}

// 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
Sparse2D::Sparse2D(int aD1, int aD2) : Lim2D(aD1, aD2)
{
  dd = d1*d2;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
Sparse2D::Sparse2D(int aL1, int aH1, int aL2, int aH2) : Lim2D(aL1, aH1, aL2, aH2)
{
  dd = d1*d2;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// Fills all elements of a matrix to a given value.
void Sparse2D::fill(double value) {
  for(int i = 0; i < dd; i++ ) data[i] = value;
}

// Private copy function: Copies values from one Sparse2D object to another.
void Sparse2D::copy(const Sparse2D& mat) {
  Lim2D::copy(mat);
  dd = mat.dd;
  data = new double [dd];
  for(int i = 0; i < dd; i++ ) data[i] = mat.data[i];
}


// Sparse3D =============================================================

// Default Constructor: Creates a 1 by 1 matrix; sets value to zero.
Sparse3D::Sparse3D(){
  dd = 1;
  data = new double [dd];  // Allocate memory
  fill(0.0);                // Set value of data_[0] to 0.0
}

// 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
Sparse3D::Sparse3D(int aD1, int aD2, int aD3) : Lim3D(aD1, aD2, aD3)
{
  dd = d1*d2*d3;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
Sparse3D::Sparse3D(int aL1, int aH1, int aL2, int aH2, int aL3, int aH3)
      : Lim3D(aL1, aH1, aL2, aH2, aL3, aH3)
{
  dd = d1*d2*d3;
  data = new double [dd];  // Allocate memory
  assert(data != 0);          // Check that memory was allocated
  fill(0.0);                  // Set values of data_[] to 0.0
}

// Fills all elements of a matrix to a given value.
void Sparse3D::fill(double value) {
  for(int i = 0; i < dd; i++ ) data[i] = value;
}

// Private copy function: Copies values from one Sparse2D object to another.
void Sparse3D::copy(const Sparse3D& mat) {
  Lim3D::copy(mat);
  dd = mat.dd;
  data = new double [dd];
  for(int i = 0; i < dd; i++ ) data[i] = mat.data[i];
}

// ===================================================================



