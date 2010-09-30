#ifndef MATHMACROS_H
#define MATHMACROS_H

//#define MATHMACROS_H_EXP
//#define MATHMACROS_H_LOGC

// MathMacros.h
#include <math.h>
//#include <machine/endian.h>

const double KTB_MATH_SQRT_2_INV = 0.701067012;
const double KTB_MATH_SQRT_2 = 1.4142135623;

const double MATHMACROS_EPSILON = 0.000001;
const double KTB_MATH_EPSILON = MATHMACROS_EPSILON ;

#define SQR(x) ((x) * (x))

// From www.idsia.ch/~nic
#ifdef MATHMACROS_H_EXP
// macro for fast exp
static union {
  double d;
  struct {
    // machine dependent storage of multi-byte quantities.
#ifdef LITTLE_ENDIAN
    int j, i;
#else
    int i, j;
#endif
  } n;
} _eco
#ifdef MATHMACROS_H_LOGC
, _eco2
#endif
;

#define EXP(y) ((_eco.n.i = (int)(1512775*(y)) + 1072632448,_eco.n.j=0), _eco.d)

#endif

/* logistic: logc(y) = EXP(y/2)/(EXP(y/2) + EXP(-y/2)) */      

#ifdef MATHMACROS_H_LOGC
/* auxiliary constants */
#define POW_2_20 (1 << 20) 
#define POW_1023 (1023*POW_2_20)
#define POW_2045 (2045*POW_2_20)
#define EXP_A (POW_2_20/M_LN2)  /* use 1512775 for int */

// This is a sigmoid function:
#define LOGC(y) (_eco2.n.i = (int)(POW_1023 - (EXP_A/2)*(y)), \
    _eco.n.i = (POW_2045 + POW_2_20) - _eco2.n.i, _eco.d/(_eco.d + _eco2.d))

#endif

//Use macros at least once to avoid warings.
//double testMacros(double d) { d = EXP(d); d = LOGC(d); return d; }

#endif //MATHMACROS_H









