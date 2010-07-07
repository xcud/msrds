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

#include "../Framework/NeoParameters.h"
#include "../Precompile/ui_Neo.h"
#include "../Framework/GuiUtils.h"
#include "Source.h"
#include "Hippo.h"
#include <string>
#include <sstream>

Hippocampus::Hippocampus(GuiUtils &i, unsigned sideCompr) : ContextSource(i) {
  MemCount = 0;
  SequenceOutput = NeoParameters::OutputNone;
  SideCompression = sideCompr;

  MaxBel.resize(NeoParameters::PredictionCount, 0);
  MaxIndex.resize(NeoParameters::PredictionCount, 0);

  ThisMaxMemSize = 1000;
  Mem = new std::string[ThisMaxMemSize];
}

void Hippocampus::SetTextOutput(std::string name){
  unsigned i;
  for(i = 0; i < MemCount; i++)
    if(Mem[i] == name)
      break; //i is the sequence number found
  if(i == MemCount)
    Mem[MemCount++] = name; 

  SequenceOutput = i;
}

void Hippocampus::InitForInference(){
// DG Check valid
//   if ( MemCount == 0 ) {
//    return;
//}
  PiIn.resize(MemCount, 1. / MemCount);
  BelStar.resize(MemCount, 0); //MemCount will not change after learning
  CombinedBelief.resize(MemCount, 0);
  std::ostringstream  lLogStream;
  lLogStream << "Hippocampus memories: " << MemCount; //<< std::endl;
  cGuiUtils.Summary( std::string( lLogStream.str() ) );
}

void Hippocampus::BeginRecognition(){
  PiIn.assign(MemCount, 1. / MemCount); //reinitialize PiIn before each recognition
  CombinedBelief.assign(MemCount, 0);
}

void Hippocampus::Recognize(){
  vector<vector<double> > lambda;
  Child->GetLambda(0, 0, SideCompression, lambda);

  unsigned r, c;

  //multiply all lambdas into lambdaProd
  vector<double> lambdaProd(MemCount, 1);
  for(c = 0; c < MemCount; c++)
    for(r = 0; r < lambda.size(); r++)
      lambdaProd[c] *= lambda[r][c];

  BelStar.assign(MemCount, 0);
  double SumBelStar = 0;
  for(c = 0; c < MemCount; c++){
    BelStar[c] = lambdaProd[c] / MemCount;
    SumBelStar += BelStar[c];
  }

//  if(SumBelStar > 0) //else we get too little probabilities (not similar to anything)
//    for(c = 0; c < MemCount; c++)
//      CombinedBelief[c] += BelStar[c] / SumBelStar; //normalize (let SumBelStar = 1)

  // Optimisation by Greg Kochaniak
  if(SumBelStar > 0) //else we get too little probabilities (not similar to anything)
  {
     double RecipSumBelStar = 1.0 / SumBelStar;  //  Just one division here allows us to multply instead of dividing
     for(c = 0; c < MemCount; c++)
        CombinedBelief[c] += BelStar[c] * RecipSumBelStar; //normalize (let SumBelStar = 1)
  }

  vector<vector<double> > piOut;
  piOut.resize(lambda.size());       //lambda.size() = NumberOfChildren
  for(r = 0; r < piOut.size(); r++){ //piOut.size()  = NumberOfChildren
    piOut[r].resize(MemCount);
    for(c = 0; c < MemCount; c++)
      if(lambda[r][c] == 0)
        piOut[r][c] = 0;
      else
        piOut[r][c] = BelStar[c] / lambda[r][c];
  }
  Child->SetPi(0, 0, SideCompression, piOut); //distribute PiOut to each child Sub-region
}

//noOfResults that was used for recognition of moving image
void Hippocampus::CalculateResults(unsigned noOfResults){ //find and display 10 best results
   unsigned c;
   for(c = 0; c < MemCount; c++)
      CombinedBelief[c] /= noOfResults; //normalize so that sum(CombinedBelief) = 1

   MaxBel.assign(NeoParameters::PredictionCount, 0);
   MaxIndex.assign(NeoParameters::PredictionCount, 0);
   for(unsigned i = 0; i < NeoParameters::PredictionCount; i++){
      for(c = 0; c < MemCount; c++)
         if(CombinedBelief[c] > MaxBel[i]){
            MaxBel[i] = CombinedBelief[c];
            MaxIndex[i] = c;
         }
         CombinedBelief[MaxIndex[i]] = 0; //will find next maximum
   }
}

//noOfResults that was used for recognition of moving image
void Hippocampus::DisplayResults( bool pLog, bool pImages ) { //find and display 10 best results
   if ( pLog ){
      for(unsigned i = 0; i < NeoParameters::PredictionCount; i++){
         std::ostringstream  lLogStream;
         if ( MaxBel[i] > 0 )
         {
            lLogStream << (i+1) << ": " << Mem[MaxIndex[i]] << "    " << (MaxBel[i]); //<< std::endl;
            cGuiUtils.Log( std::string( lLogStream.str() ) );
         }
      }
   }
   //cGuiUtils.Plot(MaxBel); - no TChart in Turbo C++ 
   if ( pImages ){
      cGuiUtils.DisplayPredictionImages(MaxIndex, MaxBel);
   }
}

std::string Hippocampus::GetResultName(unsigned resultNumber){
  return Mem[MaxIndex[resultNumber]];
}

int Hippocampus::GetSequence(unsigned x, unsigned y){
  return SequenceOutput;
}

void Hippocampus::DisplayAllMemories(){
  cGuiUtils.Log("");
  cGuiUtils.Log("Memory of hippocampus:");
  for(unsigned i = 0; i < ThisMaxMemSize; i++)
    if ( !(Mem[i].length() == 0) ){
       std::ostringstream  lLogStream;
       lLogStream << i << "\t" << Mem[i]; //<< std::endl;
       cGuiUtils.Log( std::string( lLogStream.str() ) );
  }
}
