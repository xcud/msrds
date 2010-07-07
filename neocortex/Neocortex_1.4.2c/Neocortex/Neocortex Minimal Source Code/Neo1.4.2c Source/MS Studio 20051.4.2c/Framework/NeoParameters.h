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

#ifndef NEOPARAMETERS_H
#define NEOPARAMETERS_H

#include <vector>
#include <cmath>
#include <QMessageBox>
//#include <QCloseEvent>
#include "../Precompile/ui_Parameters.h"

class NeoParameters : public QObject
{

   Q_OBJECT
signals:
   // Implements the IVision interface.
   // delivers parameter information to the client by callback thought IParameters
   // void ParameterService(  );
   public slots:
      void ParamsRevert();
      void ParamsOK();
      //void SetForgetThreshold_0(double);
      void SetControlsForMemoryStrategy(bool);
      void SetDefaultsForRegionCount(bool); // dummy parameter to satisfy signal/slot mechanism
      void SetDefaultsForRegionCount(int); // To force settings
      void UpdateDataFromSpinners();
      void UpdateSpinnersFromData();
      void SetParametersDialogExec( int& pChanged, bool pLearningValid);
public:
   // unsigned Zoom = 8;  //zoom for on-screen bitmap
   enum eSaccadePattern { eLeftRight, eRightLeft, eUpDown, eDownUp, eRightDown, eLeftDown, eRightUp, eRotate, ePingPongPattern, eRasterPattern };
   enum eRunFor{ eLearning, eRecognition };

   static const unsigned ImageSide  = 32; //in pixels

   //static const unsigned MaxMemSize = 30000; //for region

   static const unsigned PredictionCount = 10; //how many best results to display

   //region outputs
   static const int OutputNone = -1;
   static const int OutputOutOfMemory  = -2;

   //
   NeoParameters();

private:
   //sequence is forgotten if its frequency < average frequency * cForgetThreshold
   //larger threshold - more is forgotten; zero threshold - nothing is forgotten
   std::vector<float> cRegionForgetThreshold;
   std::vector<unsigned> cRegionMemorySize;
   std::vector<unsigned> cRegionLowUsageThreshold;
   std::vector<unsigned> cRegionSideCompression;
   std::vector<unsigned> cRegionSide;
   unsigned cBottomRegionSide;
   unsigned cCoveredImageSide;
   std::vector<unsigned> cSequenceLength;
   //derived: whole number of overlapping subregions that fit into NeoParameters::ImageSide
   //image side covered by hierarchy. May not cover whole bitmap due to overlap
   unsigned cRegionCount;
   //number of pixels overlapping between adjacent sub-regions
   unsigned cEyeOverlap;
   Ui_Parameters * cUiNeoParameters;
   QDialog * cParameterDialog;

   // For restoring on Cancel
   bool cChanged;
   bool cLearningValid;
   
   std::vector<float> cRegionForgetThresholdOriginal;
   std::vector<unsigned> cRegionMemorySizeOriginal;
   std::vector<unsigned> cRegionSideCompressionOriginal;
   std::vector<unsigned> cRegionSideOriginal;
   unsigned cBottomRegionSideOriginal;
   unsigned cCoveredImageSideOriginal;
   std::vector<unsigned> cSequenceLengthOriginal;
   //derived: whole number of overlapping subregions that fit into NeoParameters::ImageSide
   //image side covered by hierarchy. May not cover whole bitmap due to overlap
   unsigned cRegionCountOriginal;
   //number of pixels overlapping between adjacent sub-regions
   unsigned cEyeOverlapOriginal;
   bool cForgettingOnOriginal;
   bool cDeletionByPercentageOriginal;

   unsigned cFirstPassSaccadesLimit;
   unsigned cSecondPassSaccadesLimit;

public:
   float    GetRegionForgetThreshold(unsigned i) { return cRegionForgetThreshold[i]; };  // accessor
   bool     GetDeletionByPercentage() { return cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Checked; };
   void     SetDeletionByPercentage( bool pEnable ) { cUiNeoParameters->DeletionByPercentage->setCheckState( pEnable==true ? Qt::Checked: Qt::Unchecked ); };
   bool     GetForgettingOn() { return cUiNeoParameters->DoNotForget->checkState() != Qt::Checked; };  // box is 'do not forget'
   void     SetForgettingOn( bool pEnable ) { cUiNeoParameters->DoNotForget->setCheckState( pEnable==true ? Qt::Unchecked:Qt::Checked ); };  // box is 'do not forget'
   unsigned GetRegionSideCompression(unsigned i) { return cRegionSideCompression[i]; };// accessor
   unsigned GetBottomRegionSide() { return cBottomRegionSide; };// accessor
   unsigned GetCoveredImageSide() { return cCoveredImageSide; };// accessor
   unsigned GetSequenceLength(unsigned i) { return cSequenceLength[i]; };// accessor
   //derived: whole number of overlapping subregions that fit into NeoParameters::ImageSide
   //image side covered by hierarchy. May not cover whole bitmap due to overlap
   unsigned GetRegionCount() { return cRegionCount; };// accessor
   unsigned GetRegionSide( unsigned i) { return cRegionSide[i]; } // accessor
   //number of pixels overlapping between adjacent sub-regions
   unsigned GetEyeOverlap() { return cEyeOverlap; };// accessor

   unsigned GetRegionMemorySize(unsigned i);
   unsigned GetRegionLowUsageThreshold(unsigned i);

   unsigned GetFirstPassSaccadesLimit()
   { QString lText = cUiNeoParameters->FirstPassSaccadesLimit->text();
     return lText.toUInt();
   };
   unsigned GetSecondPassSaccadesLimit()
   { QString lText = cUiNeoParameters->SecondPassSaccadesLimit->text();
   return lText.toUInt();
   };

   //bool GetVisualiseLearningSaccades()
   //{
   //   return (cUiNeoParameters->VisualizeTrainingSaccades->checkState() == Qt::Checked);
   //}

   void SaveOriginal()
   {
      int lRegions = cRegionCount;
      cRegionForgetThresholdOriginal = cRegionForgetThreshold;
      cRegionMemorySizeOriginal = cRegionMemorySize;
      cRegionSideCompressionOriginal = cRegionSideCompression;
      cRegionSideOriginal = cRegionSide;
      cBottomRegionSideOriginal = cBottomRegionSide;
      cCoveredImageSideOriginal = cCoveredImageSide;
      cSequenceLengthOriginal = cSequenceLength;
      cRegionCountOriginal = cRegionCount;
      cEyeOverlapOriginal = cEyeOverlap;
      cForgettingOnOriginal = GetForgettingOn();
      cDeletionByPercentageOriginal = GetDeletionByPercentage();
   }

   void RestoreOriginal()
   {
      // Do this first else values get overwritten
      SetDefaultsForRegionCount( (int)cRegionCountOriginal );
      if ( cForgettingOnOriginal )
      {
         SetForgettingOn( true );
      }
      else
      {
         SetForgettingOn( false );
      }

      if ( cDeletionByPercentageOriginal )
      {
         SetDeletionByPercentage( true );
      }
      else
      {
         SetDeletionByPercentage( false );
      }

      //cForgettingOn = cForgettingOnOriginal;

      cRegionForgetThreshold.clear();
      cRegionMemorySize.clear();
      //cRegionLowUsageThreshold.clear();
      cRegionSideCompression.clear();
      cRegionSide.clear();
      cBottomRegionSide = cBottomRegionSideOriginal;
      cCoveredImageSide = cCoveredImageSideOriginal;
      cSequenceLength.clear();
      cRegionCount = cRegionCountOriginal;
      cEyeOverlap = cEyeOverlapOriginal;

      cRegionForgetThreshold	= cRegionForgetThresholdOriginal;
      cRegionMemorySize	= cRegionMemorySizeOriginal;
      cRegionSideCompression = cRegionSideCompressionOriginal;
      cRegionSide = cRegionSideOriginal;
      cBottomRegionSide = cBottomRegionSideOriginal;
      cCoveredImageSide = cCoveredImageSideOriginal;
      cSequenceLength = cSequenceLengthOriginal;
      cRegionCount = cRegionCountOriginal;
      cEyeOverlap = cEyeOverlapOriginal;

      UpdateSpinnersFromData();

      cChanged=false;
      cUiNeoParameters->Revert->setEnabled(false);
   }


private:
   void SetSideCompression( unsigned c0=0, unsigned c1=0, unsigned c2=0, unsigned c3=0 );

   bool SetValidCompression(unsigned &c0, unsigned &c1, unsigned &c2, unsigned &c3);

   void AdjustRegionCompression( int pRegion, double &pSide, unsigned pSide0, unsigned &pCompression, QSpinBox** pSpinBoxSideCompression );

   void SetForgetThreshold ( float f0=0, float f1=0, float f2=0, float f3=0 );

   void SetMemorySize ( unsigned c0=0, unsigned c1=0, unsigned c2=0, unsigned c3=0 );

   void SetRegionLowUsageForgetThreshold(unsigned c0, unsigned c1, unsigned c2, unsigned c3); 

   void DisplaySideSize();

   //protected:
   //   void closeEvent(QCloseEvent *event)
   //   {
   //      event->ignore();
   //      return;
   //   };

};
#endif
