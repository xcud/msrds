/*
@abstract(code for the tutorial on hidden Markov models )
@author(Nikolai Shokhirev <nikolai@shokhirev.com> <nikolai@u.arizona.edu>
        http://www.shokhirev.com/nikolai.html
        http://www.u.arizona.edu/~nikolai/
        http://www.chem.arizona.edu/~shokhirn/nikolai.html )
@created(2006.02.02)
©Nikolai V. Shokhirev, 2003-2006
 2006.02.24 - added PosteriorDecodingIdxs }
*/
//---------------------------------------------------------------------------

#ifndef HMMH
#define HMMH
#include "MatUtils.h"
#include "dynarrays.h"
//---------------------------------------------------------------------------
class HMM
{
private:
  int N;
  int M;
  int Tmax;
  FArr2D A;
  FArr2D B;
  FArr1D P0;
  FArr2D beta;
  FArr2D alpha;

  FArr2D* pA;
  FArr2D* pB;
  FArr1D* pP0;
  FArr2D* pbeta;
  FArr2D* palpha;

public:
  HMM();
  HMM(FArr2D& TransProb, FArr2D& ObsProb, FArr1D& InitProb);
  ~HMM();
  real GetProbabilityF(IArr1D& ObsIdxs);
  real GetProbabilityB(IArr1D& ObsIdxs);
  IArr1D ViterbiStateIdxs(IArr1D& ObsIdxs);
  IArr1D PosteriorDecodingIdxs(IArr1D& ObsIdxs);
};
#endif
