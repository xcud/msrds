/*
Neocortex - Memory-Prediction Framework
Copyright (C) 2007 Saulius J. Garalevicius
Modified by David Green (QT based GUI, event-model data sources, User interface enhancements)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef RegionH
#define RegionH

#include <vector>

using namespace std;

//class SubRegion;

class NeoRegion : public PatternSource, public ContextSource{
  private:
    unsigned ThisRegionLevel;           //level of region in hierarchy (0-lowest)
    unsigned ThisRegionSideCompression; //how many times the side of forward input is compressed
    unsigned SubRegionInputCount;//number of inputs to a Sub-region (space) 
    SubRegion ***cSubRegions;
    LearnedSequence *Mem; //[NeoParameters::MaxMemSize]; //learned vectors for this region
    unsigned MemCount;       //number of filled spots in memory array

    void GetLevel0Lambda(unsigned x, unsigned y, vector<vector<double> > &lambda);
    GuiUtils * cGuiUtils;
    int ThisMaxMemSize;
    unsigned cLowUsageFailureCount;

  public:
    NeoRegion(unsigned x, unsigned y, unsigned sl, unsigned sideCompr,
      unsigned regionLevel, GuiUtils &i);
    virtual ~NeoRegion();

    void InitLowUsageFailureCount(){ cLowUsageFailureCount=0; };
    unsigned GetLowUsageFailureCount(){ return cLowUsageFailureCount; };
    virtual void FeedForward(unsigned learningRegion, bool feedbackStage);
    virtual void Contextual();
    void ForgetRareMemories( bool pForgettingOn );
    void ForgetMemory( unsigned i );
    void InitForInference();
    void BeginRecognition();
    void Recognize();

    virtual vector<double> GetLambdaOutput(unsigned x, unsigned y);
    virtual void SetPiInput(unsigned x, unsigned y, vector<double> &pi);


    virtual unsigned GetNameOutput(unsigned x, unsigned y);
    virtual int GetSequence(unsigned x, unsigned y);

    LearnedSequence &Memory(unsigned index);
    virtual unsigned GetMemCount() {return MemCount;}
    unsigned GetLevel() {return ThisRegionLevel;}
    int AddSequence(Sequence &s);
};



#endif
