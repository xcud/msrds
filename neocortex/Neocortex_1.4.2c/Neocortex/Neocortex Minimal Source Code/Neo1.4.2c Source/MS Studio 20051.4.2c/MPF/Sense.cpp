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
#include <QMap>
#include "SparseArray.h"
#include "SubRegion.h"
#include "../Framework/NeoParameters.h"
#include "../Precompile/ui_Neo.h"
#include "../Framework/GuiUtils.h"
#include "Source.h"
#include "Sequence.h"
#include "NeoRegion.h"

#include "Sense.h"

Sense::Sense(unsigned x, unsigned y, unsigned ovlap) : PatternSource(x, y, 1){
  Overlap = ovlap;
}

//return the pattern for Sub-region (x, y) in the calling region
//sideCompr is calling region's side compression
//the result 1-D array must be allocated before
//eye must take overlap into account
void Sense::GetPattern(unsigned x, unsigned y,
                       unsigned sideCompr, unsigned *result){
 unsigned resultIndex = 0;
  unsigned pixelIncrement = sideCompr - Overlap;
  for(unsigned i = x * pixelIncrement; i < x * pixelIncrement + sideCompr; i++)
    for(unsigned j = y * pixelIncrement; j < y * pixelIncrement + sideCompr; j++)
      result[resultIndex++] = GetNameOutput(i, j);
}


/////////////////// BitmapVision //////////////////////////////////////

BitmapVision::BitmapVision(unsigned x, unsigned y, unsigned ovlap)
  : Sense(x, y, ovlap) {
  pixArray = NULL;
  Parent = NULL;
}

void BitmapVision::SetBitmap(BmpArray &pPixArray){
  this->pixArray = &pPixArray;

  //cUtils.Pause();
}

//assume Bitmap is assigned
void BitmapVision::FeedForward(unsigned learningRegion, bool feedbackStage){ 
  Parent->FeedForward(learningRegion, feedbackStage);
}

//feed forward the value of the pixel
unsigned BitmapVision::GetNameOutput(unsigned x, unsigned y){
  return (*pixArray)[x][y];
}


