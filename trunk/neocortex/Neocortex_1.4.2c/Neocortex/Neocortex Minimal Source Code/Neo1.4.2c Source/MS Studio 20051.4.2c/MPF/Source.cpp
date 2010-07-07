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

//#include "../Framework/NeoParameters.h"
#include "../Precompile/ui_Neo.h"
#include "../Framework/GuiUtils.h"
#include "Source.h"


PatternSource::PatternSource(unsigned x, unsigned y, unsigned pSeqLen) {
  OutputsX = x;
  OutputsY = y;
  SequenceLength = pSeqLen;
  PatternNumber = 0;
  Parent = NULL;
}

//return the pattern for Sub-region (x, y) in the calling region
//sideCompr is calling region's side compression
//the result 1-D array must be allocated before
void PatternSource::GetPattern(unsigned x, unsigned y,
                               unsigned sideCompr, unsigned *result){
  unsigned resultIndex = 0;
  for(unsigned i = x * sideCompr; i < (x+1) * sideCompr; i++)
    for(unsigned j = y * sideCompr; j < (y+1) * sideCompr; j++)
      result[resultIndex++] = GetNameOutput(i, j);
}

void PatternSource::GetLambda(unsigned x, unsigned y,
                              unsigned sideCompr, vector<vector<double> > &result){
  result.resize(0);
  for(unsigned i = x * sideCompr; i < (x+1) * sideCompr; i++)
    for(unsigned j = y * sideCompr; j < (y+1) * sideCompr; j++)
      result.push_back(GetLambdaOutput(i, j));
}

void PatternSource::SetPi(unsigned x, unsigned y,
                          unsigned sideCompr, vector<vector<double> > &pi){
     int piIndex = 0;
     int lPiSize = pi.size();
     for(unsigned i = x * sideCompr; i < (x+1) * sideCompr; i++)
        for(unsigned j = y * sideCompr; j < (y+1) * sideCompr; j++)
           {
           // DG Check valid
           if ( piIndex < lPiSize )
              {
              SetPiInput(i, j, pi[piIndex++]);
              }
           }
   }

ContextSource::ContextSource(GuiUtils &pInOut) : cGuiUtils(pInOut) {
  Child = NULL;
}
