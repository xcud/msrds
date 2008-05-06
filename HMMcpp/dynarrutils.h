//---------------------------------------------------------------------------
// N.V.Shokhirev
// created: 20040809
// modified: 20040809
//---------------------------------------------------------------------------
#ifndef dynarrutilsH
#define dynarrutilsH
#include <math.h>
#include <stdlib.h>
#include "dynarrays.h"
#include "MatUtils.h"
//---------------------------------------------------------------------------

bool SameLim(Lim1D& a1, Lim1D& a2);

bool SameLim(Lim2D& a1, Lim2D& a2);

bool IsSquare(Lim2D& a);

double Diff(FArr1D& a1, FArr1D& a2);

double Diff(FArr2D& a1, FArr2D& a2);

double minval(FArr1D& a, int& i);

double maxval(FArr1D& a, int& i);

double maxabs(FArr1D& a, int& i);

void minmax(FArr1D& a, double& amin, double& amax, bool reset);

bool Equal(double x1, double x2, double eps = eps0);

bool Equal(IArr1D& a1, IArr1D& a2);

bool Equal(FArr1D& a1, FArr1D& a2, double eps = eps0);

bool Equal(FArr2D& a1, FArr2D& a2, double eps = eps0);

void RandArr(IArr1D& a, int v1, int v2);

void RandArr(FArr1D& a, double v1, double v2);

void RandArr(FArr2D& a, double v1, double v2, int sym = 0);

void ShiftDiag(FArr2D& a, double v);

void MultArr(FArr1D& a, double v);

void MultArr(FArr2D& a, double v);

double Norm2(FArr1D& a1);

double Dot(FArr1D& a1, FArr1D& a2);

FArr1D Combine(double v1, FArr1D& a1, double v2, FArr1D& a2);

FArr2D Combine(double v1, FArr2D& a1, double v2, FArr2D& a2);

// Transpose
FArr2D MT(FArr2D& a);

FArr1D MxV(FArr2D& a1, FArr1D& a2);

FArr1D VxM(FArr1D& a1, FArr2D& a2);

FArr2D MxM(FArr2D& a1, FArr2D& a2);

/*
 Fourier transform T0[j] corresponds to non-centered frequency          <br>                  <br>
         Q[j] = j*2*Pi/(dT*N)   j = 0, 1, 2, .. , N-1                   <br>
 Centered frequency:     F[j] = j*2*Pi/(dT*N)   Mmin <= j <= Mmax       <br>
 where Mmin = -((N-1) div 2), Mmax = (M div 2)                          <br>
 The transform corresponding to the centered frequency                  <br>
              C[m] = T0[ m ]  0 <= m <= Mmax                            <br>
              C[m] = T0[N-m]  Mmin <= m < 0                             <br>
 */
void CenterToTrans(FArr1D& T, FArr1D& C);

void TransToCenter(FArr1D& T, FArr1D& C);

int cycle(int i, int L, int H, int D);

double extend(FArr1D& a, int i, bool cyclic);

double extend0(FArr1D& a, int i, bool cyclic);

void Smooth5(FArr1D& p0, FArr1D& p1);


#endif
