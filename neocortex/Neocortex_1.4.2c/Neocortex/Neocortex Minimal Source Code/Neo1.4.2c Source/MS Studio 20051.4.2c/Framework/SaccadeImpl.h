/*
Neocortex - Memory-Prediction Framework
Copyright (C) 2007 Saulius J. Garalevicius
Modified by David Green (QT based GUI, event-model data sources, User interface enhancements) (QT based GUI and data sources)

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

#ifndef SaccadeImpl
#define SaccadeImpl

class RasterSaccade : public ISaccade
{
public:
   RasterSaccade(int pImageSide) : LogicalBlack(0), LogicalWhite(1)
   {
      cOrigPixArray = new std::vector< std::vector<unsigned> >(pImageSide, std::vector<unsigned>(pImageSide,0));
      cShiftedPixArray = new std::vector< std::vector<unsigned> >(pImageSide, std::vector<unsigned>(pImageSide,0));
   };
   virtual ~RasterSaccade(void)
   {
      delete cOrigPixArray;
      cOrigPixArray=NULL;
      delete cShiftedPixArray;
      cShiftedPixArray=NULL;
   };
   void Initialize( BmpArray * pImage, int pImageSide, int pCoveredImageSide );
   BmpArray * Run(int pRunFor, int & pSaccadesCompleted, int pSaccadesRequested );

private:
   const unsigned LogicalBlack;
   const unsigned LogicalWhite;
   BmpArray * cShiftedPixArray;
   BmpArray * cOrigPixArray;
   int cImageSide;
   int cCoveredImageSide;
   void FindImageBoundaries(BmpArray &pixArray, unsigned &minRow, unsigned &minCol,
      unsigned &height, unsigned &width);

   bool ShiftImage(  BmpArray *pOrigPixArray, unsigned minRow, unsigned minCol,
      unsigned height, unsigned width,
      int shiftedRow, int shiftedCol, BmpArray *pShiftedPixArray);

};

class PingPongSaccade : public ISaccade
{
public:
   PingPongSaccade(int pImageSide) : LogicalBlack(0), LogicalWhite(1)
   {
      cOrigPixArray = new std::vector< std::vector<unsigned> >(pImageSide, std::vector<unsigned>(pImageSide,0));
      cShiftedPixArray = new std::vector< std::vector<unsigned> >(pImageSide, std::vector<unsigned>(pImageSide,0));
   };
   virtual ~PingPongSaccade(void)
   {
      delete cOrigPixArray;
      cOrigPixArray=NULL;
      delete cShiftedPixArray;
      cShiftedPixArray=NULL;
   };
   void Initialize( BmpArray * pImage, int pImageSide, int pCoveredImageSide );
   BmpArray * Run(int pRunFor, int & pSaccadesCompleted, int pSaccadesRequested );

private:
   const unsigned LogicalBlack;
   const unsigned LogicalWhite;
   BmpArray * cShiftedPixArray;
   BmpArray * cOrigPixArray;
   int cImageSide;
   int cCoveredImageSide;
   void FindImageBoundaries(BmpArray &pixArray, unsigned &minRow, unsigned &minCol,
      unsigned &height, unsigned &width);

   bool ShiftImage(  BmpArray *pOrigPixArray, unsigned minRow, unsigned minCol,
      unsigned height, unsigned width,
      int shiftedRow, int shiftedCol, BmpArray *pShiftedPixArray);

};
#endif