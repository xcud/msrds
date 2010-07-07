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

#include "NeoParameters.h"
#include <sstream>
#include "../Precompile/ui_Neo.h"
#include "../Framework/GuiUtils.h"
// All of these MPF files are needed. Need to check and reduce dependencies -> an interface
#include "../MPF/Sequence.h"
#include <QMap>
#include "../MPF/SparseArray.h"
#include "../MPF/SubRegion.h"
#include "../MPF/Source.h"
#include "../MPF/NeoRegion.h"
#include "../MPF/Sense.h"
#include "../MPF/Hippo.h"
// End MPF 'interface'
#include <QPixmap>
#include <QMessageBox>
#include "ISaccade.h"
#include "SaccadeImpl.h"
#include "SourceController.h"
#include "Thinker.h"


Thinker::Thinker( GuiUtils *pInOut ) :cUtils(*pInOut)
{
   //cRunning = false;
   cIWasInterrupted = false;
   cPreviousExample.clear();
   cLearningValid = false;

   cNumberOfRegions = cUtils.GetNeoParameters()->GetRegionCount();

   cIImageSourceContoller = new ImageSourceController(pInOut, NeoParameters::ImageSide);
   cIFileSourceContoller  = new FileSourceController(pInOut);
   QObject::connect (cIImageSourceContoller, SIGNAL(VisionService(BmpArray *, int)), 
      this, SLOT(IVisionService(BmpArray *, int)));

   QObject::connect (cIFileSourceContoller,  SIGNAL(FileService(std::string, std::string, int)), 
      this, SLOT(IFileService(std::string, std::string, int)));

   BmpArray * lPixArray;
   lPixArray = NULL;
   lPixArray = new std::vector< std::vector<unsigned> >(NeoParameters::ImageSide, std::vector<unsigned>(NeoParameters::ImageSide,0));//unsigned * [ImageSide];
   cThinkerPixArray = *lPixArray;
   delete lPixArray;
   lPixArray = NULL;

   Eye = new BitmapVision(NeoParameters::ImageSide, NeoParameters::ImageSide, cUtils.GetNeoParameters()->GetEyeOverlap());

   Cortex = new NeoRegion*[cNumberOfRegions];
   unsigned i, side = 0;
   for(i = 0; i < cNumberOfRegions; i++){
      if(i == 0)
         side = cUtils.GetNeoParameters()->GetBottomRegionSide();
      else
         side /= cUtils.GetNeoParameters()->GetRegionSideCompression(i);

      Cortex[i] = new NeoRegion(side, side, cUtils.GetNeoParameters()->GetSequenceLength(i), cUtils.GetNeoParameters()->GetRegionSideCompression(i), i, cUtils);
      if(i > 0)
         Cortex[i]->SetChild(Cortex[i-1]);
      cInitForInferenceDone = false;
      }

   for(i = 0; i < cNumberOfRegions-1; i++)
      Cortex[i]->SetParent(Cortex[i+1]);

   Cortex[0]->SetChild(Eye);
   Eye->SetParent(Cortex[0]);

   Hippo = new Hippocampus(cUtils, side);//side of top region == side compression of hippocampus to give '1'
   Hippo->SetChild(Cortex[cNumberOfRegions-1]);
   Cortex[cNumberOfRegions-1]->SetParent(Hippo);
}

// Required interface for receiving files to be learned or recognized
// This is the callback function that receives, successively, each single file to learn or recognize
// The files come from the implementation provided by FileController (in this case)
// Any file controller that implements this interface can deliver files to Thinker.
void Thinker::IFileService( std::string pFilename, std::string pSubdirectory, int pType ){
   if ( pType == NeoParameters::eLearning ){
      LearnExample( pFilename, pSubdirectory );
      }
   if ( pType == NeoParameters::eRecognition ){
      RecognizeFile( pFilename, pSubdirectory );
      }
}

// Required interface for receiving images to be learned or recognized
// This is the callback function that receives, successively, each single image to learn or recognize
// The images come from the implementation provided by FileController (in this case)
// Typically each image to be learned or recognized is delivered several times in accordance with 
// a saccade pattern in the controller.
// Any image source controller that implements this interface can deliver images to Thinker.
void Thinker::IVisionService( BmpArray *pPixArray, int pType ){
   if ( pType == NeoParameters::eLearning ){
      //if (cUtils.GetNeoParameters()->GetVisualiseLearningSaccades() )
      //   cUtils.ShowTestImage(*pPixArray);
      if ( cUtils.GetGui().VisualiseTrainingSaccades->isChecked() )
      {
         cUtils.ShowTestImage(*pPixArray);
      }
      LearnImage(*pPixArray, cName, cLearningRegion, cFeedbackStage);
      }
   if ( pType == NeoParameters::eRecognition ){
      cUtils.ShowTestImage(*pPixArray);
      ProcessSaccade(*pPixArray);
      }
   }

Thinker::~Thinker(){
  for(unsigned i = 0; i < cNumberOfRegions; i++)
    delete Cortex[i];
  QObject::disconnect (cIImageSourceContoller, SIGNAL(VisionService(BmpArray *, int)), 
     this, SLOT(IVisionService(BmpArray *, int)));

  QObject::disconnect (cIFileSourceContoller,  SIGNAL(FileService(std::string, std::string, int)), 
     this, SLOT(IFileService(std::string, std::string, int)));

  delete cIImageSourceContoller;
  cIImageSourceContoller = NULL;

  delete cIFileSourceContoller;
  cIFileSourceContoller = NULL;

  delete Cortex;
  Cortex = NULL;

  delete Eye;
  Eye = NULL;

  delete Hippo;
  Hippo = NULL;
}

//learns a single shifted image
void Thinker::LearnImage(BmpArray &pixArray, std::string name,
                         unsigned pLearningRegion, bool pFeedbackStage){
  Eye->SetBitmap(pixArray);
  Hippo->SetTextOutput(name); //add name to be learned to Mem

  Eye->FeedForward(pLearningRegion, pFeedbackStage); //cascading through all regions to hippocampus

  if(pFeedbackStage){ //assume pLearningRegion > 0
    Cortex[pLearningRegion-1]->Contextual(); //construct CPDMatrix in child region
    if(pLearningRegion == cNumberOfRegions-1) //top region
      Cortex[pLearningRegion]->Contextual(); //construct CPDMatrix from hippocampus context
  }
}

bool Thinker::LearningValid()
{
   return cLearningValid;
}

//learns the categories (one per sub-folder) in the selected folder
void Thinker::LearnFolder()
{
   // By keeping a record of the number of categories learned from the folder at level zero we can
   // allow the feedback processing to read just those categories
   // InitLearning(){};
   unsigned lNoOfCategoriesToRead = 0;  // zero means all
   bool lFirstForwardPass = true;
   cLearningValid = true;
   for(unsigned i = 0; i < cNumberOfRegions; i++) 
   { 
      //i is the region that is memorizing patterns
      std::ostringstream  lLogStream;
      lLogStream << "*** Memorizing patterns in region " << i; //<< std::endl;
      cUtils.Log( std::string( lLogStream.str() ) );
      cLearningRegion = i; 
      cFeedbackStage =  false;

      // Set up file source controller
      cIFileSourceContoller->SetTopLevelFolder(cUtils.GetDirectory());

      int lFirstSaccadeLimit = cUtils.GetNeoParameters()->GetFirstPassSaccadesLimit();
      if ( lFirstSaccadeLimit == 0 )
      {
         lFirstSaccadeLimit = -1;
      }
      cIImageSourceContoller->SetSaccadeCountReqd( lFirstSaccadeLimit ); // Do all saccades == -1 
      RasterSaccade * lSaccade = new RasterSaccade(NeoParameters::ImageSide);
      cIImageSourceContoller->SetSaccadePattern(NeoParameters::eRasterPattern, (ISaccade *)lSaccade );

      //cRunning = true;
      const int lReadAll = -1; // zero means all example files in each folder
      
      //memorize all patterns in level i

      std::string lInterruptMsg("");
      if ( lFirstForwardPass )
      {
         lInterruptMsg = "Do you wish to interrupt learning?  \nIf so then learning will continue with higher regions and can be interrupted again.\n";
      }
      else
      {
         lInterruptMsg = "Do you wish to stop?  Learning, will be invalid if you do.\n";
      }

      Cortex[i]->InitLowUsageFailureCount();
      cIWasInterrupted = cIFileSourceContoller->run( NeoParameters::eLearning, lInterruptMsg, lReadAll, lNoOfCategoriesToRead );

      if ( cUtils.GetNoOfCategories() == 0 )
      {
         cLearningValid = false;
         QMessageBox::information(NULL, tr("Neocortex"), tr("No images found.  The folder must contain category folders within which are the actual image files ."), QMessageBox::Ok );         
         break;
      }
      if ( Cortex[i]->GetLowUsageFailureCount() > 0)
      {
         std::ostringstream  lLogStream;
         lLogStream << "Region " << i << " Feed-forward. Discarded " << Cortex[i]->GetLowUsageFailureCount() << " new sequences."; //<< std::endl; 
         cUtils.Summary( std::string( lLogStream.str() ) );
         Cortex[i]->InitLowUsageFailureCount();
      }

      delete lSaccade;
      lSaccade = NULL;

      Cortex[i]->ForgetRareMemories( cUtils.GetNeoParameters()->GetForgettingOn() ); //forget rare patterns in level i if required

      // If learning has been interrupted then flag this by setting the number of categories read.
      // But only allow this in the first pass (bottom region) else subsequent learning 
      // will be incomplete and will be invalid anyway
      if ( cIWasInterrupted && lFirstForwardPass )
      {
         // If we were interrupted during level zero, forward pass, we can continue by reading just the categories already read.
         lNoOfCategoriesToRead   = cUtils.GetNoOfCategories();
         std::ostringstream  lLogStream;
         lLogStream << "*** Learning continues with objects seen so far."; //<< std::endl;
         cUtils.Log( std::string( lLogStream.str() ) );
         //QMessageBox::information(NULL, tr("Neocortex"), tr("Learning continues with objects seen so far."), QMessageBox::Ok );
         cIWasInterrupted = false; // Cancel the interrupt as we are still in business
      }

      if ( cIWasInterrupted  )
      {
         if ( !lFirstForwardPass )
         {
            cLearningValid = false;
            break;
         }
      }

      lFirstForwardPass = false;

      if(i > 0)
      { // Process feedback if level is > 0 
         std::ostringstream  lLogStream;
         lLogStream << "*** Embedding context in child of region " << i; //<< std::endl;
         cUtils.Log( std::string( lLogStream.str() ) );
         cLearningRegion = i; 
         cFeedbackStage =  true;
         // Set up file source controller
         cIFileSourceContoller->SetTopLevelFolder(cUtils.GetDirectory());

         int lSecondSaccadeLimit = cUtils.GetNeoParameters()->GetSecondPassSaccadesLimit();
         if ( lSecondSaccadeLimit == 0 )
         {
            lSecondSaccadeLimit = -1;
         }

         cIImageSourceContoller->SetSaccadeCountReqd( lSecondSaccadeLimit ); // Do all saccades == -1 
         RasterSaccade * lSaccade = new RasterSaccade(NeoParameters::ImageSide);
         cIImageSourceContoller->SetSaccadePattern( NeoParameters::eRasterPattern, (ISaccade *)lSaccade );

         //feedback from level i to form CPD in level i-1

         // Only read examples that were seen at level zero
         // If number of categories to read is zero this means to read them all
         std::string lInterruptMsg ("Do you wish to stop?  Learning, will be invalid if you do.\n");
         cIWasInterrupted = cIFileSourceContoller->run( NeoParameters::eLearning, lInterruptMsg, lReadAll, lNoOfCategoriesToRead );

         delete lSaccade;
         if ( cIWasInterrupted )
         {
            //QMessageBox::information(NULL, tr("Neocortex"), tr("Interrupted.  Learning is now invalid."), QMessageBox::Ok );
            cLearningValid = false;
            break;
         }
     }
   }
   std::ostringstream  lLogStream;
   if ( cLearningValid )
   {
      lLogStream << "*** Finished learning."; //<< std::endl;
   }
   else
   {
      lLogStream << "*** Learning invalid."; //<< std::endl;
   }

   cUtils.Log( std::string( lLogStream.str() ) );
   // Why does this not work?
   cIImageSourceContoller->RestoreCopyCurrentTestImage();
   cFeedbackStage = false; // use this as a flag for interrupt
}

void Thinker::LearnExample( std::string pFilename, std::string pSubdirectory ){
   QPixmap lPixmap;
   QString lqFileName = pFilename.c_str();
   bool lbSucceeded = lPixmap.load( lqFileName );
   int lResult = 0;

   if ( cPreviousExample != pSubdirectory && cLearningRegion == 0 )
   {
      QString lqFullPath( (const char *)pFilename.c_str() );
      cUtils.AddCategoryBitmap( lqFullPath );
      cPreviousExample = pSubdirectory;
   }

   if ( lbSucceeded )
   {
      cUtils.ImageToBmpArray( cThinkerPixArray, &lPixmap.toImage() );
      if ( !(cUtils.GetGui().UpdateImageWindow->checkState() == Qt::Unchecked) ){
         cUtils.ShowTestImage(cThinkerPixArray);
      }
      cName = pSubdirectory;

      cIImageSourceContoller->SetBitmap( cThinkerPixArray, 0 );  // P2 == Side compression
      RasterSaccade * lSaccade = new RasterSaccade(NeoParameters::ImageSide);
      cIImageSourceContoller->SetSaccadePattern( NeoParameters::eRasterPattern, (ISaccade *)lSaccade );
      cIImageSourceContoller->run( NeoParameters::eLearning ); // Uses the IVisionService callback

      delete lSaccade;
      lSaccade = NULL;
   }
}


bool Thinker::LearningDone(){
   return cInitForInferenceDone;
}

void Thinker::InitForInference()
{
   // Make sure the initialisation is done just once
   if ( cInitForInferenceDone )
   {
      return;
   }
   for(unsigned i = 0; i < cNumberOfRegions; i++)
    Cortex[i]->InitForInference();
   Hippo->InitForInference();
   cInitForInferenceDone = true;
}

void Thinker::BeginRecognition(){
  for(unsigned i = 0; i < cNumberOfRegions; i++)
    Cortex[i]->BeginRecognition(); //initialize all Pi to 1
  Hippo->BeginRecognition();
}

void Thinker::ProcessSaccade(BmpArray &pixArray){ //single flashed image recognition
  Eye->SetBitmap(pixArray);
  for(unsigned i = 0; i < cNumberOfRegions; i++)
    Cortex[i]->Recognize();
  Hippo->Recognize();
}

void Thinker::RecognizeFlashed(BmpArray &pOrigPixArray){

  BeginRecognition();
  ProcessSaccade(pOrigPixArray);
}


void Thinker::RecognizeMoving( BmpArray &pOrigPixArray, int pSaccadeCount )
{
   cIImageSourceContoller->SetSaccadeCountReqd( pSaccadeCount );
   RecognizeMoving( pOrigPixArray ); }


void Thinker::RecognizeMoving( BmpArray &pOrigPixArray )
{
   BeginRecognition();
   PingPongSaccade * lSaccade = new PingPongSaccade(NeoParameters::ImageSide);
   cIImageSourceContoller->SetSaccadePattern(NeoParameters::ePingPongPattern, (ISaccade *)lSaccade);
   cIImageSourceContoller->SetBitmap(pOrigPixArray, 0);  // 2nd param is side compression
   cIImageSourceContoller->run(NeoParameters::eRecognition);
   delete lSaccade;
   lSaccade = NULL;
}

int Thinker::GetSaccadesCompleted(){
   return cIImageSourceContoller->GetSaccadesCompleted();
}

void Thinker::RecognizeFolder( int pSaccadesMin, int pSaccadesMax )
{
   InitForInference();
   QString lDirectory = cUtils.GetDirectory();


   //QString lRoot(lDirectory.absolutePath());
   //lRoot+='/'; // absolutePath returns without trailing //

   cIWasInterrupted = false;
   // For each required number of saccades
   // zero means NO saccades - i.e. static image recognition
   PingPongSaccade * lSaccade = new PingPongSaccade(NeoParameters::ImageSide);
   for ( int i=pSaccadesMin; i < pSaccadesMax+1; i++ ){
      cIFileRecognitionNumTests = 0;
      cIFileRecognitionCumulativeAccuracy = 0;
      std::ostringstream  lLogStream;
      lLogStream << "******** Testing " << i << " saccades ********"; //<< '\0';
      cUtils.Log( std::string( lLogStream.str() ) );
      
      cIImageSourceContoller->SetSaccadeCountReqd(i);
      cIImageSourceContoller->SetSaccadePattern( NeoParameters::ePingPongPattern, (ISaccade *)lSaccade );
      cIFileSourceContoller->SetTopLevelFolder(lDirectory);

      std::string lInterruptMsg ("Do you wish to stop?\n");
      cIWasInterrupted = cIFileSourceContoller->run( NeoParameters::eRecognition, lInterruptMsg, 1 ); // First item in folder only
      
      // Do not need to delete the saccade yet.
      if ( !cIWasInterrupted )
      {
         std::ostringstream  lLogStream2;
         lLogStream2 << "Testing " << i << " saccades: accuracy = " << float(cIFileRecognitionCumulativeAccuracy/cIFileRecognitionNumTests); //<< std::endl;
         cUtils.Summary( std::string( lLogStream2.str() ) );
      }
      else break;
   }
   cIImageSourceContoller->ClearImage();
   delete lSaccade;
   lSaccade = NULL;
}

void Thinker::RecognizeFile( std::string pFileName, std::string pSubdirectory ){
   QPixmap lPixmap;
   QString lqFileName = pFileName.c_str();
   bool lbSucceeded = lPixmap.load( lqFileName );
   int lResult = 0;

   if ( lbSucceeded ){
      cUtils.ImageToBmpArray( cThinkerPixArray, &lPixmap.toImage() );
      cUtils.ShowTestImage(cThinkerPixArray);
      if ( cUtils.GetGui().NoiseOn->checkState() == Qt::Checked )
      {
         int lNoiseFactorX10 = cUtils.GetGui().NoiseFactor10->value();
         int lNoiseFactorX100 = cUtils.GetGui().NoiseFactor100->value();
         bool lInvertPixels = (cUtils.GetGui().InverNoisePixels->checkState() == Qt::Checked);
         cUtils.GetGui().iconeditor->AddNoise( float(lNoiseFactorX10)/10.0 + float(lNoiseFactorX100)/100.0, lInvertPixels );
         cThinkerPixArray = cUtils.GetTestImage();
         cUtils.ShowTestImage(cThinkerPixArray);
      }

      double lResult = RecogniseAndTestAccuracy(cThinkerPixArray, pSubdirectory);
      if ( lResult == 0 ){
         std::ostringstream  lLogStream2;
         lLogStream2 << "** " << pSubdirectory << " was not recognized.";
         cUtils.Log( std::string( lLogStream2.str() ) );                     
      }
      else{
            std::ostringstream  lLogStream2;
            lLogStream2 << pSubdirectory;
            cUtils.Log( std::string( lLogStream2.str() ) );                     
      }
      if ( lResult == 0 )
      {
         Hippo->DisplayResults(true, false);
      }
      else
      {
         Hippo->DisplayResults(true, true);
      }
      cIFileRecognitionCumulativeAccuracy+=lResult;
      cIFileRecognitionNumTests++;
   }
}

//returns recognition accuracy (yes/no) for the moving image
double Thinker::RecogniseAndTestAccuracy( BmpArray &pPixArray, std::string correctName )
{
   RecognizeMoving( pPixArray );

   // If number of saccades was zero this is an artificial case in which we have set
   // it to zero on the screen. The fallback is to do a "Recognize Flashed" 
   // ( because ImageContoller always emits at least the first image) so we always
   // have at least a single result i.e. for the case where the number of saccades is zero.
   Hippo->CalculateResults(GetSaccadesCompleted()+1); // Passing in the number of results
   if(Hippo->GetResultName(0) == correctName)
      return 1;
   else
      return 0;
}

void Thinker::DisplayAllMemories(){
  Hippo->DisplayAllMemories();
  for(unsigned i = 0; i < cNumberOfRegions; i++){
     std::ostringstream  lLogStream;
     lLogStream << "Region " << i << " memory count = " << Cortex[i]->GetMemCount(); //<< std::endl;
     cUtils.Log( std::string( lLogStream.str() ) );
     // IntToStr(Cortex[i]->GetMemCount()));
  }
}

void Thinker::CalcAndDisplayResults( unsigned int pSaccadeCount, bool pLog, bool pImages ){
   // Number of results is one greater than the saccade count because zero saccades means a single flashed image 
   unsigned lNoOfResults = pSaccadeCount+1;
   Hippo->CalculateResults(lNoOfResults);
   std::ostringstream  lLogStream;
   lLogStream << "No of saccades = " << pSaccadeCount; //<< std::endl;
   cUtils.Log( std::string( lLogStream.str() ) );
   Hippo->DisplayResults( pLog, pImages );
}

FileSourceController * Thinker::GetFileController()
{
   return cIFileSourceContoller;
}
