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
#ifdef _WINDOWS
#include<stdexcept>
#endif
#include "Sequence.h"
#include <sstream>

void Sequence::CreateData(){
  if(SequenceLength > 0 && InputCount > 0){
    Data = new unsigned*[SequenceLength];      //create 2-D array
    for(unsigned i = 0; i < SequenceLength; i++){
      Data[i] = new unsigned[InputCount];
      for ( int j=0; j<InputCount; j++ ){
         Data[i][j] = 0;
      }
    }
  }
}

void Sequence::DeleteData(){
  if(SequenceLength > 0 && InputCount > 0){
    for(unsigned i = 0; i < SequenceLength; i++)
      delete Data[i];
    delete Data;
  }
}


Sequence::Sequence(unsigned pSeqLen, unsigned ic){
  SequenceLength = pSeqLen;
  InputCount = ic;
  CreateData();    //create empty array in advance
  Length = 0;
}

Sequence::~Sequence(){
  DeleteData();
}

void Sequence::Init(unsigned pSeqLen, unsigned ic){
  DeleteData();
  SequenceLength = pSeqLen;
  InputCount = ic;
  CreateData();
}

//add the pattern to the end of the sequence
//pattern is an array of length InputCount
void Sequence::AddPattern(unsigned *pattern){
   if(Length < SequenceLength){              
      //copy pattern array
      for(unsigned i = 0; i < InputCount; i++)
         Data[Length][i] = pattern[i];
      Length++;
   }
   else{
      std::ostringstream  lLogStream;
      lLogStream << "Length: " << Length << " is out of bounds. Cannot add pattern."<< std::endl;
      // TO DO Can't log here at the moment: no IO available.  Need to pass in reference to constructor.
      //IO.Log( std::string( lLogStream.str() ) );
   }
   //  else
   //     throw std::exception("Sequence is already filled.");
}

void Sequence::Clear(){
  Length = 0;
}

//returns percentage of similarity (1 if sequences are identical)
//assuming sequence lengths and input counts match
//s may be incomplete
double Sequence::Compare(Sequence &s, unsigned length){
   if(SequenceLength != s.SequenceLength || InputCount != s.InputCount)  {
      std::ostringstream  lLogStream;
      lLogStream << "Cannot compare sequences of different lengths."<< std::endl;
      // TO DO Can't log here at the moment: no IO available.  Need to pass in reference to constructor.
      //IO.Log( std::string( lLogStream.str() ) );
   }
   //     throw std::exception("Cannot compare sequences of different lengths.");
   if(!Complete()){
      std::ostringstream  lLogStream;
      lLogStream << "Cannot compare with incomplete sequence."<< std::endl;
      // TO DO Can't log here at the moment: no IO available.  Need to pass in reference to constructor.
      //IO.Log( std::string( lLogStream.str() ) );
      return 0; // crashes if sequences unequal - DG fixed 05/05/2008
   }
   //     throw std::exception("Cannot compare with incomplete sequence.");
   unsigned matches = 0;
   for(unsigned i = 0; i < length; i++)
      for(unsigned j = 0; j < InputCount; j++)
         if(Data[i][j] == s.Data[i][j])
            matches++;
   return (double)matches / (length * InputCount); //precision of match
}

double Sequence::Compare(Sequence &s){
  return Compare(s, s.Length); //compare only filled part
}

void Sequence::Assign(Sequence &s){

   if(SequenceLength != s.SequenceLength || InputCount != s.InputCount)
    Init(s.SequenceLength, s.InputCount);
  Length = s.Length;
  for(unsigned i = 0; i < SequenceLength; i++)
    for(unsigned j = 0; j < InputCount; j++)
      Data[i][j] = s.Data[i][j];
}

unsigned Sequence::GetElement(unsigned patternIndex, unsigned elementIndex){
  return Data[patternIndex][elementIndex];
}

///////////////////////////////////////////////////////////////////////////////

LearnedSequence::LearnedSequence()
  : Sequence(0, 0){
  Frequency = 0; //empty sequence
}

LearnedSequence::LearnedSequence(unsigned sl, unsigned ic)
  : Sequence(sl, ic){
  Frequency = 0; //empty sequence
}

void LearnedSequence::Init(unsigned sl, unsigned ic){
  Sequence::Init(sl, ic);
  Frequency = 0; //empty sequence
}

void LearnedSequence::Assign(Sequence &s){
  Sequence::Assign(s);
  Frequency = 1; //not empty sequence - frequency must be > 0
}

void LearnedSequence::Assign(LearnedSequence &s){
  Sequence::Assign(s);
  Frequency = s.Frequency;
}


bool LearnedSequence::CanDiscard(){
  return Frequency <= 1;
}
