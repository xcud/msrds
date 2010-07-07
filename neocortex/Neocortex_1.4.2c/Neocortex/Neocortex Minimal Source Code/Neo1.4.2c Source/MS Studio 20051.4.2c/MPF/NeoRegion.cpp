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

//#include <fstream.h>
#include <sstream>
#include <math.h>
#ifdef _WINDOWS
#include <stdexcept>
#endif
#include <algorithm>
#include "../Framework/NeoParameters.h"
#include "../Precompile/ui_Neo.h"
#include "../Framework/GuiUtils.h"
#include "Sequence.h"
#include <QMap>
#include "SparseArray.h"
#include "SubRegion.h"
#include "Source.h"
#include "NeoRegion.h"

NeoRegion::NeoRegion(unsigned x, unsigned y, unsigned pSeqLen, unsigned sideCompr,
               unsigned regionLevel, GuiUtils &pGuiUtils)
  : PatternSource(x, y, pSeqLen), ContextSource(pGuiUtils) {
  
  cGuiUtils = &pGuiUtils;
  ThisRegionLevel = regionLevel;
  ThisRegionSideCompression = sideCompr;
  SubRegionInputCount = ThisRegionSideCompression*ThisRegionSideCompression; //number of inputs to a Sub-region
  MemCount = 0;
  ThisMaxMemSize = (pGuiUtils.GetNeoParameters())->GetRegionMemorySize( ThisRegionLevel );

  // Initialise memory - we could create it as we go along but in practice the memory always fills up anyway so better do it here
  // This leaves open the possibility of 'grafting' new memory on later.
  // Maybe in a real brain if the memory fills up there is a mechanism for growing more.
  // The premise here is that, in adults at least, we normally have fixed amount of tissue to work with.
  Mem = new LearnedSequence[ThisMaxMemSize] ;  

  cSubRegions = new SubRegion**[OutputsX];
  for(unsigned i = 0; i < OutputsX; i++){
    cSubRegions[i] = new SubRegion*[OutputsY];
    for(unsigned j = 0; j < OutputsY; j++)
      cSubRegions[i][j] = new SubRegion(SequenceLength, SubRegionInputCount, *this);
  }

}

NeoRegion::~NeoRegion(){
  for(unsigned x = 0; x < OutputsX; x++){
    for(unsigned y = 0; y < OutputsY; y++)
      delete cSubRegions[x][y];
    delete cSubRegions[x];
  }
  delete cSubRegions;
  delete [] Mem;
}

//take inputs, process them and remember the output
void NeoRegion::FeedForward(unsigned learningRegion, bool feedbackStage)
{
  unsigned lLowUsageThreshold = 0;
  unsigned *pattern = new unsigned[SubRegionInputCount];
  bool memorize = learningRegion == ThisRegionLevel && !feedbackStage;
  unsigned x, y;
  for(x = 0; x < OutputsX; x++)
  {
    for(y = 0; y < OutputsY; y++)
    {
      Child->GetPattern(x, y, ThisRegionSideCompression, pattern);

      // Version 1.4
      lLowUsageThreshold = cGuiUtils->GetNeoParameters()->GetRegionLowUsageThreshold(learningRegion);

      bool lRetcode = cSubRegions[x][y]->FeedForward(pattern, memorize, lLowUsageThreshold); //process and remember output
      // Version 1.4    - there is only one error condition that is reported
      //                - it happens if a low usage slot cannot be found.
      if ( !lRetcode )
      {
         cLowUsageFailureCount++;
      }
    }
  }
  delete pattern;

  if(++PatternNumber == SequenceLength)
  { //sequence processed and names are ready
    if(ThisRegionLevel < learningRegion)   //stop feed-forward chain at learning region
      Parent->FeedForward(learningRegion, feedbackStage);
    PatternNumber = 0;
  }
}

void NeoRegion::Contextual(){ //parent is the learning region
  unsigned x, y, parentContext;
  for(x = 0; x < OutputsX; x++)
    for(y = 0; y < OutputsY; y++){
      parentContext = Parent->GetSequence(x, y); //name output from parent
      cSubRegions[x][y]->Contextual(parentContext);
    }
}

void NeoRegion::ForgetRareMemories( bool pForgettingOn )
{
   unsigned lTotalObserved = 0;
   unsigned lOldMemCount = MemCount;

   // Just display total frequency and return
   if ( !pForgettingOn )
   {
      for(unsigned j = 0; j < MemCount; j++)
      { 
         unsigned lFrequency = Mem[j].GetFrequency();
         lTotalObserved += lFrequency;
      }
      std::ostringstream  lLogStream;
      lLogStream << "Region " << ThisRegionLevel << " total observations: " << lTotalObserved;
      lLogStream << ". Number of Memories: " << MemCount;
      cGuiUtils->Summary( std::string( lLogStream.str() ) );
      return;
   }


   // For forgetting by percentage
   std::vector<unsigned>lFrequencies;
   for(unsigned j = 0; j < MemCount; j++)
   { 
      unsigned lFrequency = Mem[j].GetFrequency();
      lTotalObserved += lFrequency;
      lFrequencies.push_back(lFrequency);
   }
   std::sort(lFrequencies.begin(), lFrequencies.end());
   float lPercentageThreshold = (float)lFrequencies[MemCount * cGuiUtils->GetNeoParameters()->GetRegionForgetThreshold(ThisRegionLevel) / 100 ];

   float lMemThreshold = 0.0;
   if ( cGuiUtils->GetNeoParameters()->GetDeletionByPercentage() )
   {
      lMemThreshold = lPercentageThreshold;
   }
   else
   {
      // Original calculation
      lMemThreshold = cGuiUtils->GetNeoParameters()->GetRegionForgetThreshold(ThisRegionLevel) * (float) lTotalObserved / (float) ( MemCount );
   }

   if ( lMemThreshold > 0 )
   {
      for(unsigned i = 0; i < MemCount; i++)
         if(Mem[i].GetFrequency() < lMemThreshold) //forget this sequence
         {
            Mem[i--].Assign(Mem[--MemCount]); //overwrite with last element
         }
   }

  std::ostringstream  lLogStream;
  lLogStream << "Region " << ThisRegionLevel << " total observations: " << lTotalObserved;
  if (lMemThreshold > 0 )
  {
     lLogStream << ". Memories before forgetting: " << lOldMemCount
                << ". Memories after forgetting : " << MemCount; //<< std::endl;
  }
  else
  {
     lLogStream << ". Number of Memories: " << lOldMemCount;
  }
  cGuiUtils->Summary( std::string( lLogStream.str() ) );
}

void NeoRegion::InitForInference(){
  for(unsigned x = 0; x < OutputsX; x++)
    for(unsigned y = 0; y < OutputsY; y++)
      cSubRegions[x][y]->InitForInference(Parent->GetMemCount());

  std::ostringstream  lLogStream;
  lLogStream << ThisRegionLevel << " level memories: " << MemCount; //<< std::endl; 
  cGuiUtils->Log( std::string( lLogStream.str() ) );
}

void NeoRegion::BeginRecognition(){
  for(unsigned x = 0; x < OutputsX; x++)
    for(unsigned y = 0; y < OutputsY; y++)
      cSubRegions[x][y]->BeginRecognition(Parent->GetMemCount());
}

//lambda = 1 x 139 from uniqueL1_vectors
//lambda is exponential of the hamming distance between the input and each unique vector
void NeoRegion::GetLevel0Lambda(unsigned x, unsigned y, vector<vector<double> > &lambda){
  unsigned i, j;
  int hammDist;
  unsigned *pattern = new unsigned[SubRegionInputCount];
  double newLambda;
  Child->GetPattern(x, y, ThisRegionSideCompression, pattern); //image patch from Eye
  lambda.resize(1);
  lambda[0].resize(0);

  for(i = 0; i < MemCount; i++){ //hamming distance
    hammDist = 0;
    for(j = 0; j < SubRegionInputCount; j++)
      hammDist += pattern[j] ^ Mem[i].GetElement(0, j);
    newLambda = exp(-((double)hammDist*(double)hammDist));
    lambda[0].push_back(newLambda);
  }
  delete pattern;
}

void NeoRegion::Recognize(){
  vector<vector<double> > lambda, piOut;

  for(unsigned x = 0; x < OutputsX; x++)
    for(unsigned y = 0; y < OutputsY; y++){
      //get lambda from each child of the region
      //(similarity to observed patterns from children)
      if(ThisRegionLevel == 0)
        GetLevel0Lambda(x, y, lambda); //lambda = 1 x 139 from uniqueL1_vectors
      else //ThisRegionLevel=1: 4x1037, ThisRegionLevel=2: 16x91
        Child->GetLambda(x, y, ThisRegionSideCompression, lambda);

      cSubRegions[x][y]->Recognize(lambda, piOut);

      Child->SetPi(x, y, ThisRegionSideCompression, piOut); //distribute PiOut to each child of the region
    }
}

vector<double> NeoRegion::GetLambdaOutput(unsigned x, unsigned y){
  return cSubRegions[x][y]->GetLambdaOutput();
}

void NeoRegion::SetPiInput(unsigned x, unsigned y, vector<double> &pi){
  cSubRegions[x][y]->SetPiInput(pi);
}

unsigned NeoRegion::GetNameOutput(unsigned x, unsigned y){
  return cSubRegions[x][y]->GetNameOutput();
}

//x, y are the coordinates of a Sub-region in lower region that is requesting the sequence
int NeoRegion::GetSequence(unsigned x, unsigned y){
  unsigned mySubRegionX = x / ThisRegionSideCompression; //coordinates of the column of current region
  unsigned mySubRegionY = y / ThisRegionSideCompression;
  return cSubRegions[mySubRegionX][mySubRegionY]->GetNameOutput();
}

LearnedSequence &NeoRegion::Memory(unsigned index){
   if(index < MemCount)
      return Mem[index];
   else {
      std::ostringstream  lLogStream;
      lLogStream << "Memory index: " << index << " is out of bounds."; //<< std::endl; 
      cGuiUtils->Log( std::string( lLogStream.str() ) );
      // DG 05/05/2008 This is not ideal but does output some information
      //               The fault is probably non-recoverable.
      return * ( new LearnedSequence() ); 
      //     throw std::exception("Invalid memory index.");
      // May need a pause and 'abort' in here, more info etc.
   }
}

void NeoRegion::ForgetMemory( unsigned i )
{
   Mem[i].Assign(Mem[--MemCount]); //overwrite element [i] with last element and free the last element
}

// Returns -1 if memory is full
int NeoRegion::AddSequence( Sequence &s )
{
   if(MemCount < ThisMaxMemSize)
   {
      // MemCount is one more than the current maximum index
      // But MemCount is the old value and we do have a new entry
      Mem[MemCount++].Assign(s); // set frequency to 1
      // Success in assigning new memory
      return MemCount-1;
   }
   else 
   {
      // Failed to assign
      return -1;
      //std::ostringstream  lLogStream;
      //lLogStream << "Cannot add sequence at: " << MemCount << " memory is full."; //<< std::endl; 
      //cGuiUtils->Log( std::string( lLogStream.str() ) );
   }
   //   throw std::exception("Cannot add sequence: memory full.");
}
