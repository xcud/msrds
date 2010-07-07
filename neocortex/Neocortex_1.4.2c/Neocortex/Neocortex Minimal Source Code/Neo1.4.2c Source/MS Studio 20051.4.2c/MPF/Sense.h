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

#ifndef SenseH
#define SenseH

class Sense : public PatternSource
{

private:
   unsigned Overlap; //number of overlapping pixels between adjacent parent sub-regions
public:
   Sense(unsigned x, unsigned y, unsigned ovlap);

   void GetPattern(unsigned x, unsigned y,
      unsigned sideCompr, unsigned *result);

   // =================================================================================================
   // DG - must implement these two functions because they are pure virtual functions in the superclass
   // ---------------------------------------------------------------

   // Never called
   vector<double> GetLambdaOutput(unsigned x, unsigned y)
   {
      std::vector<double> *lRet = NULL;
      return *lRet; 
   };

   // Never called
   void SetPiInput(unsigned x, unsigned y, vector<double> &pi) 
   {
      return;
   };
   // ==============================================================

};

class BitmapVision : public Sense{
private:
   BmpArray * pixArray;
public:
   BitmapVision(unsigned x, unsigned y, unsigned ovlap);
   void SetBitmap(BmpArray &pixArray);
   virtual unsigned GetNameOutput(unsigned x, unsigned y);

   virtual int GetSequence(unsigned x, unsigned y)
   {return GetNameOutput(x, y);} //dummy

   virtual void FeedForward(unsigned learningRegion, bool feedbackStage);

   // See declaration in PatternSource
};

#endif
