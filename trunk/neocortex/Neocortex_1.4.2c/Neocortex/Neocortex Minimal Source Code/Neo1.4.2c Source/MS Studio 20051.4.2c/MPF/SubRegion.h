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

#ifndef SubRegionH
#define SubRegionH

#include <vector>

using namespace std;

class NeoRegion;
class Sequence;

class SubRegion {
  private:
    unsigned SequenceLength; //number of steps that form a sequence (time)
    unsigned InputCount;     //number of inputs (space)
    NeoRegion &MyRegion;
    int NameOutput; //output value of the Sub-region; negative values are special situations
    unsigned cMemIndex; //for level1 indexes in Mem
    Sequence *CurrentSequence;
    //vector<vector<double> > cCPDMatrix;
    // This is a map based sparse matrix to replace <vector<vector>>
    std::map<AccessKey, double> * cCPDMatrix;

    std::vector<double> cBelStar, cLambdaOut, cPiIn;

    void FindBestMatch(Sequence &s, unsigned &retIndex, double &retPrecision);
    int SubRegion::FindLowUsage(int pMaxUsage);
    struct BestMatch
    {
       //Sequence * Sequence;
       double Precision;
       unsigned int Index;
    };
    //void AddLearnedSequence(Sequence &s, int &retIndex);
  public:
    SubRegion(unsigned sl, unsigned ic, NeoRegion &r);
    virtual ~SubRegion();
    bool FeedForward(unsigned *pattern, bool memorize, unsigned pLowUsageThreshold);
    void Contextual(unsigned parentContext);
    void InitForInference(unsigned parentMemCount);
    void BeginRecognition(unsigned parentMemCount);
    void Recognize(vector<vector<double> > &lambda, vector<vector<double> > &piOut);

    vector<double> GetLambdaOutput();
    void SetPiInput(vector<double> &pi);
    unsigned GetNameOutput();
};


#endif
