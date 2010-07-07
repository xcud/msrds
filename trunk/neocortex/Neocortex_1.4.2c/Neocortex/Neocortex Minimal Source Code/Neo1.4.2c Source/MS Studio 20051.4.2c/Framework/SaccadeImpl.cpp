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

#include "ISaccade.h"
#include "SaccadeImpl.h"

void RasterSaccade::Initialize( BmpArray * pImage, int pImageSide, int pCoveredImageSide)
{
   cCoveredImageSide=pCoveredImageSide;
   cImageSide = pImageSide;
   *cOrigPixArray = *pImage;
   *cShiftedPixArray = *pImage;
};

BmpArray * RasterSaccade::Run( int pRunFor, int & pSaccadesCompleted, int pSaccadesRequested )
{ 
   // RasterSaccade is used during learning as of version 1.3.3
   unsigned int minRow, minCol, height, width;
   BmpArray *lTemp;
   lTemp = cOrigPixArray;
   cOrigPixArray = cShiftedPixArray;
   cShiftedPixArray = lTemp;
   FindImageBoundaries(*cOrigPixArray, minRow, minCol, height, width);

   //int lLimit = 0;
   //if ( pSaccadesRequested == -1 )
   //{
   //   // do all
   //   lLimit = ( cCoveredImageSide - height + 1 ) * ( cCoveredImageSide - width + 1 );
   //}
   //else
   //{
   //   lLimit = pSaccadesRequested;
   //}
   //generate all possible shifts of the image
   pSaccadesCompleted = 0;
   unsigned i, j; //i, j - upper left corner of image rectangle
   for(i = 0; i <= cCoveredImageSide - height; i++) //move only within covered area
   {
      for(j = 0; j <= cCoveredImageSide - width; j++)
      {

         //all possible shifts, including beyond image boundaries
         //for(i = 1-height; i < (int)CoveredImageSide; i++) //move only within covered area
         //  for(j = 1-width; j < (int)CoveredImageSide; j++){

         //if something was drawn in the image area
         if(ShiftImage(cOrigPixArray, minRow, minCol, height, width, i, j, cShiftedPixArray))
         { 
            emit VisionService(cShiftedPixArray, pRunFor);
         }
         pSaccadesCompleted++;
         if ( pSaccadesCompleted == pSaccadesRequested )  // this will never be true if requesting -1 saccades
         {
            break;
         }

         // Save some shifting;
         lTemp = cOrigPixArray;
         cOrigPixArray = cShiftedPixArray;
         cShiftedPixArray = lTemp; // don't actually need the data!
         // But now we have to get the boundaries again.
         FindImageBoundaries(*cOrigPixArray, minRow, minCol, height, width);
      }
      if ( pSaccadesCompleted == pSaccadesRequested ) // this will never be true if requesting -1 saccades
      {
         break;
      }
   }
   return cShiftedPixArray;
};

void RasterSaccade::FindImageBoundaries(BmpArray &pixArray, unsigned &minRow, unsigned &minCol,
                                                unsigned &height, unsigned &width)
{
   unsigned r, c, maxRow = 0, maxCol = 0;
   minRow = minCol = cImageSide-1;

   for(r = 0; r < cImageSide; r++)
   {
      for(c = 0; c < cImageSide; c++)
         if((pixArray)[r][c] == LogicalBlack) 
         {
            if(r < minRow)
               minRow = r;
            if(r > maxRow)
               maxRow = r;
            if(c < minCol)
               minCol = c;
            if(c > maxCol)
               maxCol = c;
         }
   }
   height = maxRow - minRow + 1;
   width = maxCol - minCol + 1;
}

//works correctly with shifting beyond image boundaries
//returns if something was actually drawn in the image
bool RasterSaccade::ShiftImage(  BmpArray *pOrigPixArray, unsigned minRow, unsigned minCol,
                                       unsigned height, unsigned width,
                                       int shiftedRow, int shiftedCol, BmpArray *pShiftedPixArray)
{

   bool ret = false;
   int r, c;
   for(r = 0; r < cImageSide; r++)
      for(c = 0; c < cImageSide; c++)
         if(r >= shiftedRow && r <= shiftedRow+height-1 &&
            c >= shiftedCol && c <= shiftedCol+width-1)
         {
               (*pShiftedPixArray)[r][c] = (*pOrigPixArray)[minRow+r-shiftedRow][minCol+c-shiftedCol];
               ret |= ((*pShiftedPixArray)[r][c] == LogicalBlack);
         }
         else
            (*pShiftedPixArray)[r][c] = LogicalWhite; //blank
   return ret;
}
// ----------------------------------------------------------------------------------------

void PingPongSaccade::Initialize( BmpArray * pImage, int pImageSide, int pCoveredImageSide)
{
   cCoveredImageSide=pCoveredImageSide;
   cImageSide = pImageSide;
   *cOrigPixArray = *pImage;
   *cShiftedPixArray = *pImage;
};

BmpArray * PingPongSaccade::Run( int pRunFor, int & pSaccadesCompleted, int pSaccadesRequested )
{ 
   // Make calling convention same as for RasterSaccade
   int lSaccadesRequested = pSaccadesRequested;
   if ( pSaccadesRequested == -1 )
   {
      lSaccadesRequested = 0;
   }
   unsigned int minRow, minCol, height, width;
   BmpArray *lTemp;
   lTemp = cOrigPixArray;
   cOrigPixArray = cShiftedPixArray;
   cShiftedPixArray = lTemp;
   pSaccadesCompleted = 0;
   FindImageBoundaries(*cOrigPixArray, minRow, minCol, height, width);
   
   // Always emit at least one image (for 0 saccades)
   emit VisionService(cShiftedPixArray, pRunFor);

   //generate all possible shifts of the image
   unsigned i, j; //i, j - upper left corner of image rectangle
   unsigned row = minRow, col = minCol;

   int deltaRow = -1, deltaCol = -1;

   if ( row+height == cCoveredImageSide )
   {
      deltaRow = 1;
   }
   else
   {
      deltaRow = -1;
   }

   if ( col+width == cCoveredImageSide )
   {
      deltaCol = 1;
   }
   else
   {
      deltaCol = -1;
   }

   for(i = 0; i < lSaccadesRequested; i++)
   {
      if ( height != cImageSide )
      {
         if(row == 0 || row+height == cCoveredImageSide)//bounce off boundaries  
            deltaRow = -deltaRow;                       //assume height, width < CoveredImageSide
         row += deltaRow;
      }
      if ( width != cImageSide )
      {
         if(col == 0 || col+width == cCoveredImageSide) // 
            deltaCol = -deltaCol;
         col += deltaCol;
      }
      ShiftImage(cOrigPixArray, minRow, minCol, height, width,
         row, col, cShiftedPixArray);
      emit VisionService(cShiftedPixArray, pRunFor);
      pSaccadesCompleted++;
      // Save some shifting;
      lTemp = cOrigPixArray;
      cOrigPixArray = cShiftedPixArray;
      cShiftedPixArray = lTemp; // don't actually need the data!
      // But now we have to get the boundaries again.
      FindImageBoundaries(*cOrigPixArray, minRow, minCol, height, width);
   }
   return cShiftedPixArray;
};

void PingPongSaccade::FindImageBoundaries(BmpArray &pixArray, unsigned &minRow, unsigned &minCol,
                                        unsigned &height, unsigned &width)
{
   unsigned r, c, maxRow = 0, maxCol = 0;
   minRow = minCol = cImageSide-1;

   for(r = 0; r < cImageSide; r++)
   {
      for(c = 0; c < cImageSide; c++)
         if((pixArray)[r][c] == LogicalBlack) 
         {
            if(r < minRow)
               minRow = r;
            if(r > maxRow)
               maxRow = r;
            if(c < minCol)
               minCol = c;
            if(c > maxCol)
               maxCol = c;
         }
   }
   height = maxRow - minRow + 1;
   width = maxCol - minCol + 1;
}

//works correctly with shifting beyond image boundaries
//returns if something was actually drawn in the image
bool PingPongSaccade::ShiftImage(  BmpArray *pOrigPixArray, unsigned minRow, unsigned minCol,
                               unsigned height, unsigned width,
                               int shiftedRow, int shiftedCol, BmpArray *pShiftedPixArray)
{

   bool ret = false;
   int r, c;
   for(r = 0; r < cImageSide; r++)
      for(c = 0; c < cImageSide; c++)
         if(r >= shiftedRow && r <= shiftedRow+height-1 &&
            c >= shiftedCol && c <= shiftedCol+width-1)
         {
            (*pShiftedPixArray)[r][c] = (*pOrigPixArray)[minRow+r-shiftedRow][minCol+c-shiftedCol];
            ret |= ((*pShiftedPixArray)[r][c] == LogicalBlack);
         }
         else
            (*pShiftedPixArray)[r][c] = LogicalWhite; //blank
   return ret;
}
