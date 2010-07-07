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

#include <vector>
#include <map>
#include <sstream>
#include "SparseArray.h"
#include "../Framework/NeoParameters.h"
#include "../Precompile/ui_Neo.h"
#include "../Framework/GuiUtils.h"
#include "Sequence.h"
#include "SubRegion.h"
#include "Source.h"
#include "NeoRegion.h"

SubRegion::SubRegion(unsigned pSeqLen, unsigned ic, NeoRegion &r) : MyRegion(r){
  SequenceLength = pSeqLen;
  InputCount = ic;
  CurrentSequence = new Sequence(SequenceLength, InputCount);
  NameOutput = NeoParameters::OutputNone;
  cCPDMatrix = new std::map<AccessKey, double>; //, AccessKey::LessThan> ;
}

SubRegion::~SubRegion(){
  delete CurrentSequence;
  cCPDMatrix->clear();
  delete cCPDMatrix;
}


//finds a sequence in the array of learned sequences
// It always finds a match
void SubRegion::FindBestMatch(Sequence &s, unsigned &retIndex, double &retPrecision){
  double precision;
  retPrecision = 0;
  retIndex = 0;
  for(unsigned i = 0; i < MyRegion.GetMemCount(); i++){
    precision = MyRegion.Memory(i).Compare(s);
    if(precision > retPrecision){
      retPrecision = precision;
      retIndex = i;
      if(retPrecision == 1) //can't be better
        return;
    }
  }  
}

int SubRegion::FindLowUsage( int pMaxUsage )
{
   for(unsigned i = 0; i < MyRegion.GetMemCount(); i++){
      if ( MyRegion.Memory(i).GetFrequency() < (pMaxUsage+1) )
      {
         return i;
      }
   }  
   return -1;
};

//take input pattern, process it and remember the output
//pattern is array of length InputCount containing the input pattern
bool SubRegion::FeedForward( unsigned *pattern, bool memorize, unsigned pLowUsageThreshold )
{
   bool lRetCode = true;
   // CurrentSequence is the single sequence that is being worked on
   // Sequence length is always one.
   CurrentSequence->AddPattern(pattern);
   if(CurrentSequence->Complete())
   {
      unsigned int lLearnedIndex;
      double lMatchPrecision;
      BestMatch lBestMatch;
      FindBestMatch(*CurrentSequence, lLearnedIndex, lMatchPrecision);

      lBestMatch.Precision = lMatchPrecision;
      lBestMatch.Index = lLearnedIndex;

      if(memorize && lBestMatch.Precision < 1) // potentially treat as a new sequence
      {
         int lRetIndex = 0;
         lRetIndex = MyRegion.AddSequence(*CurrentSequence);

         if ( -1 ==  lRetIndex )
         {
            // New at version 1.4
            // Memory full so add it to the best place we can find
            // But only for a reasonably good match
            // 
            if ( lBestMatch.Precision > 0.95 )
            {
               MyRegion.Memory(lBestMatch.Index).IncFrequency();  
            }
            else
            {
               int lLowUsageIndex = FindLowUsage(pLowUsageThreshold);
               if ( lLowUsageIndex != -1 )
               {
                  // Replace the memory with the one at the top 
                  MyRegion.ForgetMemory( lLowUsageIndex );
                  // Add the new sequence to the top
                  lRetIndex = MyRegion.AddSequence(*CurrentSequence);
                  if ( -1 ==  lRetIndex )
                  {
                     lRetCode = false;
                     lBestMatch.Index = NeoParameters::OutputNone;
                  }
                  else
                  {
                     lBestMatch.Index = (unsigned int) lRetIndex; // this should be the top of memory
                  }

               }
               else
               {
                  // Sequence was not added.
                  lRetCode = false;
                  lBestMatch.Index = NeoParameters::OutputNone;
                  // Would be nice to log - but we do not have logging in this class right now.
                  //std::ostringstream  lLogStream;
                  //lLogStream << "Cannot find a low-usage slot for new sequence." << " Memory is full."; //<< std::endl; 
                  //cGuiUtils->Log( std::string( lLogStream.str() ) );
               }
            }
         }
         else
         {
            // Memory already had space for the new sequence (frequency is already set to 1)
            lBestMatch.Index = (unsigned int) lRetIndex;
         }
      }
      else
      {
         if ( memorize && lBestMatch.Precision == 1 )
         {
            // Match precision is 100%
            MyRegion.Memory(lBestMatch.Index).IncFrequency();  
         }
      }

      NameOutput = lBestMatch.Index; //index of the best match for the new sequence
      cMemIndex = lBestMatch.Index;   //used in Contextual
      CurrentSequence->Clear();
   }

   return lRetCode;
}

void SubRegion::Contextual(unsigned parentContext){ //parentContext is name output from parent


   //  NO NEED to pre-create entries - only the ones that are required will be created
   std::map<AccessKey, double> lIter; 
   AccessKey lKey(parentContext, cMemIndex);

   // Returns false if entry did not exist
   if ( (*cCPDMatrix)[lKey] == 0.0 )
   {
      // Initialize new entry (note that it has been created by the attempted access
      (*cCPDMatrix)[lKey] = 1.0;
   }
   else
   {
      (*cCPDMatrix)[lKey] += 1.0;
   }
   //   Use insert to speed things up
   // May be quicker but may crash
   //lSuccess = cCPDMatrix->insert(std::make_pair(lKey, 1.0)).second;
   //if ( !lSuccess ) // i.e. row exists
   //{
   //   (*cCPDMatrix)[lKey] += 1.0;
   //}
}

void SubRegion::InitForInference(unsigned pParentMemCount)
{
   //cCPDMatrix has pParentMemCount rows and MemCount Sub-regions

   double sum = 0;
   
   // Declare map iterator.  This will return all elements row-major order.
   // Initialise Current row to the first row of the matrix
   std::map<AccessKey, double>::iterator lIter = cCPDMatrix->begin(); //, AccessKey::LessThan>::iterator lIter = cCPDMatrix->begin();
   unsigned lCurrentRow = lIter->first.cPosition.first;

   std::vector<AccessKey> lvPrev;
   std::vector<AccessKey>::iterator lvPrevIter;

   unsigned lMaxCol = 0;
   unsigned lMaxRow = 0;

   // Normalise the results
   for ( lIter = cCPDMatrix->begin(); lIter != cCPDMatrix->end(); lIter++ )
   {
      // NB the lIter->first is the key for the map.
      //    The key itself is also made up of a value pair - row and column, respectively, within cPosition.
      // 
      if ( lIter->first.cPosition.first != lCurrentRow )
      { 
         // We use this next time through
         // Now we have to just normalise the current row and set the sum back to zero
         lCurrentRow = lIter->first.cPosition.first;
         if ( sum > 0 )
         {
            lvPrevIter = lvPrev.begin();
            for ( lvPrevIter = lvPrev.begin(); lvPrevIter != lvPrev.end(); lvPrevIter++ )
            {
               (*cCPDMatrix)[*lvPrevIter] /= sum;
            }
            sum = 0;
            lvPrev.clear();
         }
      }
      lvPrev.push_back(lIter->first); // save access key
      sum += lIter->second;
   }
   
   if ( lvPrev.size() > 0 )   // we have exited from the final row but this row has not been normalised
                              // So do it now
   {
      lvPrevIter = lvPrev.begin();
      for ( lvPrevIter = lvPrev.begin(); lvPrevIter != lvPrev.end(); lvPrevIter++ )
      {
         (*cCPDMatrix)[*lvPrevIter] /= sum;
      }
      sum=0;
      lvPrev.clear();
   }   

   // Increment to give the actual row count
   cPiIn.resize(lCurrentRow+1, 1);
   // It seems that the following is sometimes too small:
   // cPiIn.resize(pParentMemCount, 1);
}

void SubRegion::BeginRecognition(unsigned parentMemCount){
  cPiIn.assign(parentMemCount, 1); //reinitialize cPiIn before each recognition
}

//lambda is evidence from children, measures NumberOfChildren x MemCount
//for Level0: lambda[r][c] is probability that the input to the child r is
//the memorized pattern c (how close is the input to the memorized pattern)
//assume size(piOut) = number of children
void SubRegion::Recognize(vector<vector<double> > &lambda, vector<vector<double> > &piOut){
   unsigned r, c;

   AccessKey lKey(0,0);

   unsigned rowCount = MyRegion.GetParent()->GetMemCount(); //cPiIn.size(); ?
   unsigned colCount = MyRegion.GetMemCount();

   //multiply all lambdas into lambdaProd
   //lambdaProd[c] is combined probability from children that the input to ALL of them
   //(total input to this Sub-region) is the memorized pattern c
   //(AND operation - multiplication)

   vector<double> lambdaProd(colCount, 1.0);
   for(c = 0; c < colCount; c++)
      for(r = 0; r < lambda.size(); r++) //lambda.size() = NumberOfChildren
         lambdaProd[c] *= lambda[r][c];

   // DG.  I think if we try to add new learning and forget rare memories that have been referenced by higher level memories, 
   //      the program can crash due to these orphaned memories still being in cCPDMatrix.
   //      (This version of the program does not allow addition of learning).
   //      We could consider deleting the orphaned memories from cCPDMatrix at this point.
   //
   // Exptl - can recalculate required array size in case forgetting memories results in incorrect value
   //         Probably not a very good strategy but code may be useful anyway.
   // std::map<AccessKey, double>::iterator lIter0, lIterEnd0;
   // lIterEnd0 = cCPDMatrix->end();
   //for ( lIter0 = cCPDMatrix->begin(); lIter0 != lIterEnd0; lIter0++ )
   //{
   //   const std::pair<unsigned,unsigned> lPosition= lIter0->first.cPosition;
   //   r=lPosition.first;
   //   c=lPosition.second;
   //   if ( r > rowCount )
   //      rowCount = r;
   //   if ( c > colCount )
   //      colCount = c;
   //}

   vector<double> v(colCount, 0);

   // fxu is the probability that the current input (classified as c) is
   //            part of the higher-level pattern r
   // Optimisation by Greg Kochaniak - fxu does not have to be a matrix if we just have a single loop.
   double fxu, lOut;
   cBelStar.assign(colCount, 0);
   cLambdaOut.assign(rowCount, 0);

   std::map<AccessKey, double>::iterator lIter, lIterEnd;
   lIterEnd = cCPDMatrix->end();
   for ( lIter = cCPDMatrix->begin(); lIter != lIterEnd; lIter++ )
   {
      const std::pair<unsigned,unsigned> lPosition= lIter->first.cPosition;
      r=lPosition.first;
      c=lPosition.second;
      double lVal = lIter->second;
      lOut = lambdaProd[c] * lVal;
      fxu = lOut * cPiIn[r];
      if ( cBelStar[c] < fxu )
      {
         cBelStar[c] = fxu;
      }

      if (cLambdaOut[r] < lOut )
      {
         cLambdaOut[r] = lOut;
      }
   }


/*   The following code is no longer needed
   //cBelStar (belStarX) is the max for each column in fxu (max over u)
   // colCount is the number of memorised spatial pattern coincidences
   cBelStar.resize(colCount, 0);
   cBelStar.assign(colCount, 0);
   for(r = 0; r < rowCount; r++)
      for(c = 0; c < colCount; c++)
         if(cBelStar[c] < fxu[r][c])
            cBelStar[c] = fxu[r][c];

   //cLambdaOut is the max for each row in fxu divided by cPiIn - message to our parent
   cLambdaOut.resize(rowCount, 0);
   cLambdaOut.assign(rowCount, 0);
   for(r = 0; r < rowCount; r++)
      if(cPiIn[r] == 0)
         cLambdaOut[r] = 0;
      else{
         for(c = 0; c < colCount; c++) //max of each row
            if(cLambdaOut[r] < fxu[r][c])
               cLambdaOut[r] = fxu[r][c];
         cLambdaOut[r] /= cPiIn[r];
      }

      //lambda.size() = piOut.size() = InputCount(NumberOfChildren) (or 1 for lowest level)
      piOut.resize(lambda.size());
      for(r = 0; r < piOut.size(); r++){
         piOut[r].resize(colCount);
         for(c = 0; c < colCount; c++)
            if(lambda[r][c] == 0)
               piOut[r][c] = 0;
            else
               piOut[r][c] = cBelStar[c] / lambda[r][c];
      }
*/
}


vector<double> SubRegion::GetLambdaOutput(){
  return cLambdaOut;
}

void SubRegion::SetPiInput(vector<double> &pi){
  cPiIn = pi; //copies all elements; pi.size() = pParentMemCount
}

unsigned SubRegion::GetNameOutput(){
  return NameOutput;
}





