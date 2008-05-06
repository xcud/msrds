//---------------------------------------------------------------------------
// N.V.Shokhirev
// created: 20040720
// modified: 20040721

#ifndef dynarraysH
#define dynarraysH
//---------------------------------------------------------------------------
// N.V.Shokhirev
// created: 20040720
// modified: 20040721
#include <assert.h>  // Defines the assert function.

//---------------------------------------------------------------------------

typedef double real;

class FArr1D;

class FArr2D;

// Lim1D =============================================================

class Lim1D
{
public:
  // Default Constructor: Creates a 1 - vector.
  Lim1D ();

  // 1-based Constructor: Creates an aD1 - vector.
  Lim1D(int aD1);

  // General Constructor: Creates an aD1 - vector.
  Lim1D(int aL1, int aH1);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  Lim1D(const Lim1D& lim) { this->copy(lim); } // Call private copy function.

  // "getter" functions. Return dimension.
  int D1() const { return d1; }
  // "getter" functions. Return low.
  int L1() const { return l1; }
  // "getter" functions. Return high.
  int H1() const { return h1; }
  // "setter" functions for Low
  void SetL1(int aL1) { l1 = aL1; h1 = l1 + d1 -1; }

  bool eq(Lim1D& lim);
  
protected:
  // Copies values from one Lim1D object to another.
  void copy(const Lim1D& lim);

  int d1;  // Dimension
  int l1;  // low boundary
  int h1;  // high boundary
  
//private:

}; // Class Lim1D

// Lim2D =============================================================

class Lim2D: public Lim1D
{
public:

  // Default Constructor: Creates a 1 by 1 matrix.
  Lim2D ();

  // 1-based Constructor: Creates an aD1 by aD1 matrix.
  Lim2D(int aD1, int aD2);

  // General Constructor. Creates an aD1 by aD1 matrix.
  Lim2D(int aL1, int aH1, int aL2, int aH2);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  Lim2D(const Lim2D& lim) { this->copy(lim); } // Call private copy function.

  // "getter" function: Return dimension.
  int D2() const { return d2; }
  //  "getter" function: Return low.
  int L2() const { return l2; }
  //  "getter" function: Return high.
  int H2() const { return h2; }
  //  "setter" function for Low
  void SetL2(int aL2) { l2 = aL2; h2 = l2 + d2 -1; }

protected:
  // Copies values from one Lim2D object to another.
  void copy(const Lim2D& lim);

  int d2;  // Number of columns
  int l2;  // low boundary
  int h2;  // high boundary
  
//private:

}; // Class Lim2D

// Lim2D =============================================================

class Lim3D: public Lim2D
{
public:

  // Default Constructor: Creates a 1 by 1 matrix.
  Lim3D ();

  // 1-based Constructor: Creates an aD1 by aD1 matrix.
  Lim3D(int aD1, int aD2, int aD3);

  // General Constructor. Creates an aD1 by aD1 matrix.
  Lim3D(int aL1, int aH1, int aL2, int aH2, int aL3, int aH3);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  Lim3D(const Lim3D& lim) { this->copy(lim); } // Call private copy function.

  // "getter" function: Return dimension.
  int D3() const { return d3; }
  //  "getter" function: Return low.
  int L3() const { return l3; }
  //  "getter" function: Return high.
  int H3() const { return h3; }
  //  "setter" function for Low
  void SetL3(int aL3) { l3 = aL3; h3 = l3 + d3 -1; }

protected:
  // Copies values from one Lim2D object to another.
  void copy(const Lim3D& lim);

  int d3;  // Number of columns
  int l3;  // low boundary
  int h3;  // high boundary
  
//private:

}; // Class Lim3D

// FArr1D =============================================================

class FArr1D : public Lim1D
{

public:

  // Default Constructor: Creates a 1 - vector; sets value to zero.
  FArr1D ();

  // 1-based Constructor: Creates an aD1 - vector; sets values to zero.
  FArr1D(int aD1);

  // General Constructor: Creates an aD1 - vector; sets values to zero.
  FArr1D(int aL1, int aH1);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  FArr1D(const FArr1D& vec) {
    this->copy(vec);   // Call private copy function.
  }

  // Destructor: Called when a FArr1D object goes out of scope.
  ~FArr1D() {
    delete [] data;   // Release allocated memory
  }

  // Assignment operator function.
  // Overloads the equal sign operator to work with FArr1D objects.
  FArr1D& operator=(const FArr1D& vec) {
  if( this == &vec ) return *this;  // If two sides equal, do nothing.
  delete [] data;                  // Delete data on left hand side
  this->copy(vec);                  // Copy right hand side to l.h.s.
  return *this;
}
/*
  FArr1D& operator*=(const double);
  FArr1D& operator+=(const double);
  FArr1D& operator+=(const FArr1D&);
  FArr1D& operator-=(const FArr1D&);
  FArr1D& operator- ();
*/
  friend FArr1D operator+(const FArr1D&, const FArr1D&);
  friend FArr1D operator-(const FArr1D&, const FArr1D&);
  friend double operator*(const FArr1D&, const FArr1D&);
  friend FArr1D operator*(const FArr1D&, const double);
  friend FArr1D operator*(const double x, const FArr1D& a2){ return a2 * x; }

  // Parenthesis operator function: Allows access to values of FArr1D via (i1).
  // Example: a(1) = 2*b(2);
  double& operator() (int i1) {
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking
    return data[ (i1-l1) ];  // Access appropriate value
  }

  // Parenthesis operator function (const version).
  const double& operator() (int i1) const{
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking
    return data[ (i1-l1) ];  // Access appropriate value
  }

  // Fills all elements of a vector to a given value.
  void fill(double value);

  void swap1(int i1, int j1);

  double minval(int& i);

  double maxval(int& i);

  double maxabs(int& i);

  void minmax(double& amin, double& amax, bool reset);

//*********************************************************************
private:
  int dd;  // Data Dimension
  double* data;     // Pointer used to allocate memory for data.

  // Private copy function.
  // Copies values from one FArr1D object to another.
  void copy(const FArr1D& vec);

}; // Class FArr1D

// FArr2D =============================================================
//    l2    h2
// l1| 1| 2| 3|
//   | 4| 5| 6|
//   | 7| 8| 9|
// h1|10|11|12|
class FArr2D : public Lim2D
{

public:

  // Default Constructor: Creates a 1 by 1 matrix; sets value to zero.
  FArr2D ();

  // 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  FArr2D(int aD1, int aD2);

  // General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  FArr2D(int aL1, int aH1, int aL2, int aH2);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  FArr2D(const FArr2D& mat) {
    this->copy(mat);   // Call private copy function.
  }

  // Destructor: Called when a FArr2D object goes out of scope.
  ~FArr2D() {
    delete [] data;   // Release allocated memory
  }

  // Assignment operator function.
  // Overloads the equal sign operator to work with FArr2D objects.
  FArr2D& operator=(const FArr2D& mat) {
  if( this == &mat ) return *this;  // If two sides equal, do nothing.
  delete [] data;                  // Delete data on left hand side
  this->copy(mat);                  // Copy right hand side to l.h.s.
  return *this;
}

  friend FArr2D operator+(const FArr2D&, const FArr2D&);
  friend FArr2D operator-(const FArr2D&, const FArr2D&);
  friend FArr2D operator*(const FArr2D&, const double);
  friend FArr2D operator*(const double x, const FArr2D& a2){ return a2 * x; }
  friend FArr1D operator*(const FArr2D&, const FArr1D&);
  friend FArr1D operator*(const FArr1D&, const FArr2D&);
  friend FArr2D operator*(const FArr2D&, const FArr2D&);

  // Parenthesis operator function: Allows access to values of FArr2D via (i,j) pair.
  // Example: a(1,1) = 2*b(2,3);
  double& operator() (int i1, int i2) {
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    return data[ d2*(i1-l1) + (i2-l2) ];  // Access appropriate value
  }

  // Parenthesis operator function (const version).
  const double& operator() (int i1, int i2) const{
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    return data[ d2*(i1-l1) + (i2-l2) ];  // Access appropriate value
  }

  // Fills all elements of a matrix to a given value.
  void fill(double value);
  // swap rows
  void swap1(int i1, int j1);
  // swap columns
  void swap2(int i2, int j2);

//*********************************************************************
private:

  int dd;  // Data Dimension

  double* data;     // Pointer used to allocate memory for data.

  // Private copy function.
  // Copies values from one FArr2D object to another.
  void copy(const FArr2D& mat);

}; // Class FArr2D

// IArr1D =============================================================

class IArr1D : public Lim1D
{

public:

  // Default Constructor: Creates a 1 - vector; sets value to zero.
  IArr1D ();

  // 1-based Constructor: Creates an aD1 - vector; sets values to zero.
  IArr1D(int aD1);

  // General Constructor: Creates an aD1 - vector; sets values to zero.
  IArr1D(int aL1, int aH1);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  IArr1D(const IArr1D& vec) {
    this->copy(vec);   // Call private copy function.
  }

  // Destructor: Called when a IArr1D object goes out of scope.
  ~IArr1D() {
    delete [] data;   // Release allocated memory
  }

  // Assignment operator function.
  // Overloads the equal sign operator to work with IArr1D objects.
  IArr1D& operator=(const IArr1D& vec) {
  if( this == &vec ) return *this;  // If two sides equal, do nothing.
  delete [] data;                  // Delete data on left hand side
  this->copy(vec);                  // Copy right hand side to l.h.s.
  return *this;
}

//  friend IArr1D operator+(const IArr1D&, const IArr1D&);
//  friend IArr1D operator-(const IArr1D&, const IArr1D&);
//  friend IArr1D operator*(const IArr1D&, const int);
//  friend IArr1D operator*(const int x, const IArr1D& a2){ return a2 * x; }

  // Parenthesis operator function: Allows access to values of IArr1D via (i1).
  // Example: a(1) = 2*b(2);
  int& operator() (int i1) {
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking
    return data[ (i1-l1) ];  // Access appropriate value
  }

  // Parenthesis operator function (const version).
  const int& operator() (int i1) const{
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking
    return data[ (i1-l1) ];  // Access appropriate value
  }

  // Fills all elements of a vector to a given value.
  void fill(int value);

  void swap1(int i1, int j1);

//*********************************************************************
private:
  int dd;  // Data Dimension
  int* data;     // Pointer used to allocate memory for data.

  // Private copy function.
  // Copies values from one IArr1D object to another.
  void copy(const IArr1D& vec);

}; // Class IArr1D

// IArr2D =============================================================
//    l2    h2
// l1| 1| 2| 3|
//   | 4| 5| 6|
//   | 7| 8| 9|
// h1|10|11|12|
class IArr2D : public Lim2D
{

public:

  // Default Constructor: Creates a 1 by 1 matrix; sets value to zero.
  IArr2D ();

  // 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  IArr2D(int aD1, int aD2);

  // General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  IArr2D(int aL1, int aH1, int aL2, int aH2);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  IArr2D(const IArr2D& mat) {
    this->copy(mat);   // Call private copy function.
  }

  // Destructor: Called when a IArr2D object goes out of scope.
  ~IArr2D() {
    delete [] data;   // Release allocated memory
  }

  // Assignment operator function.
  // Overloads the equal sign operator to work with IArr2D objects.
  IArr2D& operator=(const IArr2D& mat) {
  if( this == &mat ) return *this;  // If two sides equal, do nothing.
  delete [] data;                  // Delete data on left hand side
  this->copy(mat);                  // Copy right hand side to l.h.s.
  return *this;
}


  // Parenthesis operator function: Allows access to values of IArr2D via (i,j) pair.
  // Example: a(1,1) = 2*b(2,3);
  int& operator() (int i1, int i2) {
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    return data[ d2*(i1-l1) + (i2-l2) ];  // Access appropriate value
  }

  // Parenthesis operator function (const version).
  const int& operator() (int i1, int i2) const{
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    return data[ d2*(i1-l1) + (i2-l2) ];  // Access appropriate value
  }

  // Fills all elements of a matrix to a given value.
  void fill(int value);

//*********************************************************************
private:

  int dd;  // Data Dimension

  int* data;     // Pointer used to allocate memory for data.

  // Private copy function.
  // Copies values from one IArr2D object to another.
  void copy(const IArr2D& mat);

}; // Class IArr2D


// FArr3D =============================================================
/*  _________
   |\        \ l1         i1=11          i1=11+1
   | \        \
   |  \ _______\ h1        l3    h3      l3    h3
   |   |  |  |  | l2    l2| 1| 2| 3|   l2|13|14|15|
    \  |  |  |  |         | 4| 5| 6|     |16|17|18|
     \ |  |  |  |         | 7| 8| 9|     |19|20|21|
      \|  |  |  | h2    h2|10|11|12|   h2|22|23|24|
        l3    h3
*/
class FArr3D : public Lim3D
{

public:

  // Default Constructor: Creates a 1 x 1 x 1 matrix; sets value to zero.
  FArr3D ();

  // 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  FArr3D(int aD1, int aD2, int aD3);

  // General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  FArr3D(int aL1, int aH1, int aL2, int aH2, int aL3, int aH3);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  FArr3D(const FArr3D& mat) {
    this->copy(mat);   // Call private copy function.
  }

  // Destructor: Called when a FArr2D object goes out of scope.
  ~FArr3D() {
    delete [] data;   // Release allocated memory
  }

  // Assignment operator function.
  // Overloads the equal sign operator to work with FArr2D objects.
  FArr3D& operator=(const FArr3D& mat) {
  if( this == &mat ) return *this;  // If two sides equal, do nothing.
  delete [] data;                  // Delete data on left hand side
  this->copy(mat);                  // Copy right hand side to l.h.s.
  return *this;
}

  // Parenthesis operator function: Allows access to values of FArr2D via (i,j) pair.
  // Example: a(1,1) = 2*b(2,3);
  double& operator() (int i1, int i2, int i3) {
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    assert(i3 >= l3 && i3 <= h3);          // Bounds checking
    return data[ d3*d2*(i1-l1) + d3*(i2-l2) + (i3-l3) ];  // Access appropriate value
  }

  // Parenthesis operator function (const version).
  const double& operator() (int i1, int i2, int i3) const{
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    assert(i3 >= l3 && i3 <= h3);          // Bounds checking
    return data[ d3*d2*(i1-l1) + d3*(i2-l2) + (i3-l3) ];  // Access appropriate value
  }

  // Fills all elements of a matrix to a given value.
  void fill(double value);

  void GetFArr2D(int i1, FArr2D& m);

  void SetFArr2D(int i1, FArr2D& m);

//*********************************************************************
private:

  int dd;  // Data Dimension

  double* data;     // Pointer used to allocate memory for data.

  // Private copy function.
  // Copies values from one FArr2D object to another.
  void copy(const FArr3D& mat);

}; // Class FArr3D


// Sparse2D =============================================================
//    l2    h2
// l1| 1| 2| 3|
//   | 4| 5| 6|
//   | 7| 8| 9|
// h1|10|11|12|
class Sparse2D : public Lim2D
{
public:

  // Default Constructor: Creates a 1 by 1 matrix; sets value to zero.
  Sparse2D ();

  // 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  Sparse2D(int aD1, int aD2);

  // General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  Sparse2D(int aL1, int aH1, int aL2, int aH2);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  Sparse2D(const Sparse2D& mat) {
    this->copy(mat);   // Call private copy function.
  }

  // Destructor: Called when a Sparse2D object goes out of scope.
  ~Sparse2D() {
    delete [] data;   // Release allocated memory
  }

  // Assignment operator function.
  // Overloads the equal sign operator to work with Sparse2D objects.
  Sparse2D& operator=(const Sparse2D& mat) {
  if( this == &mat ) return *this;  // If two sides equal, do nothing.
  delete [] data;                  // Delete data on left hand side
  this->copy(mat);                  // Copy right hand side to l.h.s.
  return *this;
}

  // Parenthesis operator function: Allows access to values of Sparse2D via (i,j) pair.
  // Example: a(1,1) = 2*b(2,3);
  double& operator() (int i1, int i2) {
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    return data[ d2*(i1-l1) + (i2-l2) ];  // Access appropriate value
  }

  // Parenthesis operator function (const version).
  const double& operator() (int i1, int i2) const{
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    return data[ d2*(i1-l1) + (i2-l2) ];  // Access appropriate value
  }

  // Fills all elements of a matrix to a given value.
  void fill(double value);

//*********************************************************************
private:

  int dd;  // Data Dimension

  double* data;     // Pointer used to allocate memory for data.

  // Private copy function.
  // Copies values from one Sparse2D object to another.
  void copy(const Sparse2D& mat);

}; // Class Sparse2D


// Sparse3D =============================================================
/*  _________
   |\        \ l1         i1=11          i1=11+1
   | \        \
   |  \ _______\ h1        l3    h3      l3    h3
   |   |  |  |  | l2    l2| 1| 2| 3|   l2|13|14|15|
    \  |  |  |  |         | 4| 5| 6|     |16|17|18|
     \ |  |  |  |         | 7| 8| 9|     |19|20|21|
      \|  |  |  | h2    h2|10|11|12|   h2|22|23|24|
        l3    h3
*/
class Sparse3D : public Lim3D
{

public:

  // Default Constructor: Creates a 1 x 1 x 1 matrix; sets value to zero.
  Sparse3D ();

  // 1-based Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  Sparse3D(int aD1, int aD2, int aD3);

  // General Constructor: Creates an aD1 by aD1 matrix; sets values to zero.
  Sparse3D(int aL1, int aH1, int aL2, int aH2, int aL3, int aH3);

  // Copy Constructor: Used when a copy of an object is produced
  // (e.g., passing to a function by value)
  Sparse3D(const Sparse3D& mat) {
    this->copy(mat);   // Call private copy function.
  }

  // Destructor: Called when a FArr2D object goes out of scope.
  ~Sparse3D() {
    delete [] data;   // Release allocated memory
  }

  // Assignment operator function.
  // Overloads the equal sign operator to work with FArr2D objects.
  Sparse3D& operator=(const Sparse3D& mat) {
  if( this == &mat ) return *this;  // If two sides equal, do nothing.
  delete [] data;                  // Delete data on left hand side
  this->copy(mat);                  // Copy right hand side to l.h.s.
  return *this;
}

  // Parenthesis operator function: Allows access to values of FArr2D via (i,j) pair.
  // Example: a(1,1) = 2*b(2,3);
  double& operator() (int i1, int i2, int i3) {
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    assert(i3 >= l3 && i3 <= h3);          // Bounds checking
    return data[ d3*d2*(i1-l1) + d3*(i2-l2) + (i3-l3) ];  // Access appropriate value
  }

  // Parenthesis operator function (const version).
  const double& operator() (int i1, int i2, int i3) const{
    assert(i1 >= l1 && i1 <= h1);          // Bounds checking for rows
    assert(i2 >= l2 && i2 <= h2);          // Bounds checking for columns
    assert(i3 >= l3 && i3 <= h3);          // Bounds checking
    return data[ d3*d2*(i1-l1) + d3*(i2-l2) + (i3-l3) ];  // Access appropriate value
  }

  // Fills all elements of a matrix to a given value.
  void fill(double value);

//*********************************************************************
private:

  int dd;  // Data Dimension

  double* data;     // Pointer used to allocate memory for data.

  // Private copy function.
  // Copies values from one FArr2D object to another.
  void copy(const Sparse3D& mat);

}; // Class Sparse3D

#endif
