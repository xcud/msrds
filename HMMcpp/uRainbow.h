//---------------------------------------------------------------------------
// created:  20041102 by N.V.Shokhirev
// modified: 20041117 by N.V.Shokhirev
//---------------------------------------------------------------------------

#ifndef uRainbowH
#define uRainbowH
#include "MatUtils.h"
//---------------------------------------------------------------------------
enum RGB { clR = 255, clG = 65280, clB = 16711680 };
enum RGB2 { RG, RB, GR, GB, BR, BG };

int rainbow(double t);
int rainbow1(double t);

int PositiveNegative(RGB2 pair, double t);
int Transition(RGB2 pair, double t);

#endif
 