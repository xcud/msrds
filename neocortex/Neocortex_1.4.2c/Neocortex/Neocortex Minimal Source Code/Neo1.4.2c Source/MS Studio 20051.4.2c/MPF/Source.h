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

#ifndef SourceH
#define SourceH

#include <vector>

using namespace std;

class ContextSource;

//structure that can produce a pattern in a form of unsigned matrix (x by y)
//has feed-forward outputs
class PatternSource{
  protected:
    ContextSource *Parent;
    unsigned OutputsX, OutputsY;
    unsigned SequenceLength; //number of steps that form a sequence (time)
    unsigned PatternNumber; //in the current sequence: from 0 to SequenceLength
    virtual vector<double> GetLambdaOutput(unsigned x, unsigned y) = 0;
  public:
    PatternSource(unsigned x, unsigned y, unsigned sl);
    virtual ~PatternSource(){};
    void SetParent(ContextSource *s) {Parent = s;}
    ContextSource * GetParent(){ return Parent; }

    virtual unsigned GetNameOutput(unsigned x, unsigned y) = 0;
    virtual void GetPattern(unsigned x, unsigned y,
                            unsigned sideCompr, unsigned *result);

    virtual void GetLambda(unsigned x, unsigned y,
                           unsigned sideCompr, vector<vector<double> > &result);

    virtual void SetPiInput(unsigned x, unsigned y, vector<double> &pi) = 0;
    virtual void SetPi(unsigned x, unsigned y,
                       unsigned sideCompr, vector<vector<double> > &pi);

};

class ContextSource{
  protected:
    GuiUtils &cGuiUtils;
    PatternSource *Child;
  public:
    ContextSource(GuiUtils &i);
    virtual ~ContextSource(){};
    void SetChild(PatternSource *s) {Child = s;}
    virtual void FeedForward(unsigned learningRegion, bool feedbackStage) = 0;
    virtual void Contextual() = 0;
    virtual int GetSequence(unsigned x, unsigned y) = 0;
    virtual unsigned GetMemCount() = 0;
};


#endif
