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

#ifndef HippoH
#define HippoH

class Hippocampus : public ContextSource{
  private:
    int ThisMaxMemSize;
    std::string * Mem; //[NeoParameters::MaxMemSize];
    unsigned MemCount;        //number of filled spots in memory array
    unsigned SideCompression; //number of children = SideCompression*SideCompression
    int SequenceOutput;
    std::string TextInput, TextOutput;
    vector<double> BelStar, PiIn, CombinedBelief, MaxBel;
    vector<unsigned> MaxIndex;
  public:
    Hippocampus(GuiUtils &i, unsigned sideCompr);
    ~Hippocampus(){ delete [] Mem; };
    void SetTextOutput(std::string name);

    void FeedForward(unsigned learningRegion, bool feedbackStage) {}
    void Contextual()  {}

    void InitForInference();
    void BeginRecognition();
    void Recognize();
    void DisplayResults(bool pLog, bool pImages);
    void CalculateResults(unsigned saccadeCount);
    std::string GetResultName(unsigned resultNumber);    

    virtual int GetSequence(unsigned x, unsigned y);
    virtual unsigned GetMemCount() {return MemCount;}

    void DisplayAllMemories();
};

#endif
