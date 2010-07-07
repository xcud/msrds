/*
Neocortex - Memory-Prediction Framework
Copyright (C) 2007/2008 Saulius J. Garalevicius
Modified by David Green (QT based GUI, event-model data sources, User interface enhancements) (QT based GUI, event-model data sources, User interface enhancements)

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
#include <QApplication>
#include "../Framework/NeoParameters.h"
#include "../Precompile/ui_About.h"
#include "../Precompile/ui_Neo.h"
#include "../Framework/NeoParameters.h"
#include "../Framework/GuiUtils.h"
#include <QFileDialog>
#include <QMessageBox>
#include "../Framework/Thinker.h"
#include <sstream>

#include "MainWindow.h"


MyMainWindow::MyMainWindow()  //QWidget *parent)
: QMainWindow(0)//, Ui::mainWindow(*this)
{
   cMainUi.setupUi(this);

   // Munge around to get the window without a system menu. 
   // Does not seem possible simply to XOR out he system menu.
   //Qt::WindowFlags lNewFlags;
   //lNewFlags = Qt::FramelessWindowHint;
   //lNewFlags |= Qt::WindowTitleHint;
   //  Not needed lNewFlags |= Qt::WindowMinimizeButtonHint;  // you geta  full system menu if you do this
   //const Qt::WindowFlags lOldFlags = this->windowFlags();
   //this->setWindowFlags( lNewFlags );

   QObject::connect(cMainUi.actionOpen,       SIGNAL(triggered(bool)),    this, SLOT(GuiOpenImage(bool)) );
   QObject::connect(cMainUi.OpenImage,         SIGNAL(clicked()),      this, SLOT(GuiOpenImage(bool)) );
   QObject::connect(cMainUi.actionExit,       SIGNAL(triggered(bool)),    this, SLOT(GuiExitApp(bool)) );

   QObject::connect(cMainUi.LearnFolder,       SIGNAL(clicked()),      this, SLOT(GuiLearnFolder()) );
   QObject::connect(cMainUi.RecognizeFlash,    SIGNAL(clicked()),      this, SLOT(GuiRecognizeFlashed()) );
   QObject::connect(cMainUi.RecognizeMoving,   SIGNAL(clicked()),      this, SLOT(GuiRecognizeMoving()) );
   QObject::connect(cMainUi.TestSelectedFolder,SIGNAL(clicked()),      this, SLOT(GuiRecognizeFolder()) );

   QObject::connect(cMainUi.NormalizeResults,  SIGNAL(toggled(bool)),  this, SLOT(GuiDisplayPredictionConfidence(bool)));

   cUtils = new GuiUtils( &(this->cMainUi) ); // ImageThinker construction requires this
   ImageThinker=NULL;

   // GuiAbout dialog
   QObject::connect(cMainUi.actionAbout, SIGNAL(triggered(bool)),  this, SLOT(GuiAbout(bool)) );

   this->cMainUi.SetParameters->setDisabled(false);
   this->cMainUi.RecognizeFlash->setDisabled(true);
   this->cMainUi.RecognizeMoving->setDisabled(true);
   this->cMainUi.TestSelectedFolder->setDisabled(true);
   this->cMainUi.LearnFolder->setDisabled(false);
   this->cMainUi.SetParameters->setDisabled(false);
   this->cMainUi.LearnFolder->setDisabled(false);
   this->cMainUi.Stop->setDisabled(true);

   //ClearImage();

   QColor lPenColour(0,0,0);
   QColor lBkgColour(255,255,255);
   cMainUi.iconeditor->setPenColor(lPenColour);

   ReadSettings();

   cDummyNoAction = false;

   // Parameter dialog
   cNeoParameters = cUtils->GetNeoParameters();
   QObject::connect(cMainUi.SetParameters, SIGNAL(clicked()), this, SLOT(GuiSetParameters()) );

   ImageThinker = new Thinker( cUtils );

   QString lCurrentDirectory(".");
   lCurrentDirectory = cUtils->GetDirectory();
   cUtils->SetDirectory( lCurrentDirectory );
}

//#include <QMouseEvent>
//void MyMainWindow::mousePressEvent(QMouseEvent *event)
//{
//   if (event->button() == Qt::LeftButton)
//   {
//      emit MousePressEvent();
//   }
//   else if (event->button() == Qt::RightButton)
//   {
//   }
//};

void MyMainWindow::GuiAbout( bool pDummy )
{
   // Could probably pre-create in constructor.
   cUiAbout = new Ui_About();
   QDialog * lAbout = new QDialog();
   cUiAbout->setupUi( lAbout );
   QObject::connect(cUiAbout->pushButton, SIGNAL(clicked()), (const QObject *) this, SLOT(AboutOK()) );
   lAbout->show();
}

void MyMainWindow::AboutOK()
{
   delete cUiAbout;
   cUiAbout = NULL;
}

void MyMainWindow::Interrupt()
{
   cDummyNoAction = true;
}

void MyMainWindow::GuiDisplayPredictionConfidence(bool pState)
{
   cUtils->DisplayPredictionConfidence(pState);
}

void MyMainWindow::closeEvent(QCloseEvent * pEvent)
{
   WriteSettings();
}

bool MyMainWindow::CheckIconImageContent( )
{
   // Checks to see if there is a meaningful content in the image
   int lMarkCount = 0;
   bool lOK = false;
   QColor lColour;
   unsigned int lAlphaMask = 0x00FFFFFF;

   QImage lImage = cMainUi.iconeditor->iconImage();

   for ( unsigned i=0; i< NeoParameters::ImageSide; i++ )
   {
      for ( int j=0; j< NeoParameters::ImageSide; j++ )
      {
         if ( (lImage.pixel(i, j) & lAlphaMask) == lColour.black() )
         {
            lMarkCount++;
         }
         if ( lMarkCount > 0 )
         {
            lOK = true;
            break;
         }
      }
   }
   return ( lOK );
}

void MyMainWindow::ClearLearningAndCreateNewThinker()
{

   // Re-initialise for new learning.
   if ( ImageThinker )
   {
      delete ImageThinker;
      ImageThinker = NULL;
      cUtils->ClearCategoryBitmaps();
      ImageThinker = new Thinker( cUtils );

      this->cMainUi.SetParameters->setDisabled(false);
      this->cMainUi.RecognizeFlash->setDisabled(true);
      this->cMainUi.RecognizeMoving->setDisabled(true);
      this->cMainUi.TestSelectedFolder->setDisabled(true);
      this->cMainUi.LearnFolder->setDisabled(false);
   }
}

void MyMainWindow::GuiSetParameters()
{
   int lParamsChanged=false;
   cNeoParameters->SetParametersDialogExec(lParamsChanged, ImageThinker->LearningValid());
   if ( lParamsChanged )
   {
      ClearLearningAndCreateNewThinker();
   }
   // ClearLearning will have created a new ImageThinker with the new parameters if user agreed to clear learning
}

void MyMainWindow::GuiLearnFolder()
{
   // Dialog enables user to specify the directory
   QString lCurrentDir = cUtils->GetDirectory();
   QString lDirectory = QFileDialog::getExistingDirectory (NULL,
      tr("Select Folder"), lCurrentDir ); //, QFileDialog::ShowDirsOnly | QFileDialog::DontUseSheet | QFileDialog::DontUseNativeDialog );

   if ( lDirectory.size() > 0 )
   {
      // Re-initialise for new learning.
      if ( ImageThinker && ImageThinker->LearningValid() )
      {
         int i = QMessageBox::warning(this, tr("Neocortex"), tr("Current trained hierarchy will be deleted.  Do you wish to continue?"), 
            QMessageBox::Yes | QMessageBox::No);
         if ( i != QMessageBox::Yes )
         {
            return;
         }
      }
  
      ClearLearningAndCreateNewThinker();
      cDummyNoAction = false;

      // Disabling the control is interpreted by the ShowTestImage code to means that
      // we are in Learning Mode and the check should be acted upon
      this->cMainUi.UpdateImageWindow->setEnabled(false);
      // Gets confused if you change during learning
      this->cMainUi.ScrollText->setEnabled(false);

      cDummyNoAction = false;

      std::ostringstream  lLogStream;
      lLogStream << "Region                       ";
      for ( unsigned i=0; i< cNeoParameters->GetRegionCount(); i++ )
      {
         lLogStream << i << "                     ";
      }
      lLogStream << std::endl;
      lLogStream << "Side Compression      ";
      for ( unsigned i=0; i< cNeoParameters->GetRegionCount(); i++ )
      {
         lLogStream << cNeoParameters->GetRegionSideCompression(i) << "                     ";
      }

      lLogStream << std::endl;
      lLogStream << "Side                           ";
      for ( unsigned i=0; i< cNeoParameters->GetRegionCount(); i++ )
      {
         lLogStream << cNeoParameters->GetRegionSide(i) << "                     ";
      }
      lLogStream << std::endl;
      if ( cNeoParameters->GetForgettingOn() )
      {
         lLogStream << "Forget Threshold      ";
      }
      else
      {
         lLogStream << "Low UsageThreshold ";
      }
      for ( unsigned i=0; i< cNeoParameters->GetRegionCount(); i++ )
      {
         if ( cNeoParameters->GetForgettingOn() )
         {
            lLogStream << cNeoParameters->GetRegionForgetThreshold(i) << "                 ";
         }
         else
         {
            lLogStream << 0 << cNeoParameters->GetRegionLowUsageThreshold(i) << "                   ";
         }
      }
      lLogStream << "" << std::endl;
      lLogStream << "Learning Saccades:  First Pass = " << cUtils->GetNeoParameters()->GetFirstPassSaccadesLimit();
      lLogStream << ", Second Pass = " << cUtils->GetNeoParameters()->GetSecondPassSaccadesLimit() << "." << " (Zero means no imposed limit.)" << std::endl;

      cUtils->Summary( std::string( lLogStream.str() ) );
      
      cUtils->SetDirectory(lDirectory);
      QDir::setCurrent( lDirectory );

      const QSize lSize( NeoParameters::ImageSide ,NeoParameters::ImageSide );
      
      this->cMainUi.RecognizeFlash->setDisabled(true);
      this->cMainUi.RecognizeMoving->setDisabled(true);
      this->cMainUi.TestSelectedFolder->setDisabled(true);      
      this->cMainUi.SetParameters->setDisabled(true);
      this->cMainUi.LearnFolder->setDisabled(true);
      
      this->cMainUi.Stop->setDisabled(false);
      ImageThinker->LearnFolder( );
      this->cMainUi.Stop->setDisabled(true);

      this->cMainUi.SetParameters->setDisabled(false);
      this->cMainUi.LearnFolder->setDisabled(false);

      if ( ImageThinker->LearningValid() )
      {
         this->cMainUi.RecognizeFlash->setDisabled(false);
         this->cMainUi.RecognizeMoving->setDisabled(false);
         this->cMainUi.TestSelectedFolder->setDisabled(false);
      }
      else
      {
         this->cMainUi.RecognizeFlash->setDisabled(true);
         this->cMainUi.RecognizeMoving->setDisabled(true);
         this->cMainUi.TestSelectedFolder->setDisabled(true);
      }
   }

   //if ( ! cDummyNoAction )
   {
      this->cMainUi.UpdateImageWindow->setEnabled(true);
      this->cMainUi.ScrollText->setEnabled(true);
   }

}

void MyMainWindow::GuiOpenImage( )
{
   GuiOpenImage( true ); //arbitrary value - not supplied by button click
}

void MyMainWindow::GuiOpenImage( bool pDummy )
{
   // NB this event is picked up separately (in a SourceController)  for actual processing.
   //    Here we are just concerned with the Gui controls
   if ( ImageThinker && ImageThinker->LearningDone() )
   {
      this->cMainUi.RecognizeFlash->setDisabled(false);
      this->cMainUi.RecognizeMoving->setDisabled(false);
   }
}

void MyMainWindow::GuiRecognizeFlashed()
{
   ImageThinker->InitForInference();
   //this->cMainUi.GuiLearnFolder->setEnabled(false);

   BmpArray * lTempPixArray;
   lTempPixArray = NULL;
   lTempPixArray = new std::vector< std::vector<unsigned> >(NeoParameters::ImageSide, std::vector<unsigned>(NeoParameters::ImageSide,0));//unsigned * [NeoParameters::ImageSide];
   BmpArray lTestPixArray = *lTempPixArray;
   delete lTempPixArray;
   lTempPixArray = NULL;

   QImage lImage = cMainUi.iconeditor->iconImage();
   cUtils->ImageToBmpArray(lTestPixArray, &lImage);
   if ( lTestPixArray.size() > 0 )
   {
      if ( CheckIconImageContent() )
      {
         // Could just use GuiRecognizeMoving with no saccades
         ImageThinker->RecognizeFlashed(lTestPixArray);
         ImageThinker->CalcAndDisplayResults(0, true, true );
      }
   }
}

void MyMainWindow::GuiRecognizeMoving()
{
   // The moving image is recognized starting at the currently displayed image.
   // Note that this is appears to be a change from MPF in which setting "Saccades From" to zero results
   // in an error and setting "Saccades from" to 1 gives the same result as "Recognize Flashed" i.e. no saccades.
   ImageThinker->InitForInference();
   //this->cMainUi.GuiLearnFolder->setEnabled(false);

   BmpArray * lTempPixArray;
   lTempPixArray = NULL;
   lTempPixArray = new std::vector< std::vector<unsigned> >(NeoParameters::ImageSide, std::vector<unsigned>(NeoParameters::ImageSide,0));//unsigned * [NeoParameters::ImageSide];
   BmpArray lTestPixArray = *lTempPixArray;
   delete lTempPixArray;
   lTempPixArray = NULL;

   QImage lImage = cMainUi.iconeditor->iconImage();

   {
      if ( CheckIconImageContent() )
      {
         cDummyNoAction = false;
         cUtils->ImageToBmpArray(lTestPixArray, &lImage);
         QString lStrSaccadesFrom = cMainUi.SaccadesFrom->text();
         unsigned lSaccadesFrom = lStrSaccadesFrom.toUInt();
         this->cMainUi.RecognizeFlash->setDisabled(true);
         this->cMainUi.RecognizeMoving->setDisabled(true);
         this->cMainUi.SetParameters->setDisabled(true);
         this->cMainUi.LearnFolder->setDisabled(true);
         this->cMainUi.TestSelectedFolder->setDisabled(true);
         ImageThinker->RecognizeMoving( lTestPixArray, lSaccadesFrom );
         this->cMainUi.RecognizeFlash->setDisabled(false);
         this->cMainUi.RecognizeMoving->setDisabled(false);
         this->cMainUi.SetParameters->setDisabled(false);
         this->cMainUi.LearnFolder->setDisabled(false);
         this->cMainUi.TestSelectedFolder->setDisabled(false);

         // If number of saccades was zero this is an artificial case in which we have set
         // it to zero on the screen. The fallback is to do a "Recognize Flashed"
         // ( because ImageContoller always emits at lest the first image) so we do
         // actually have a single result.
         int lNoOfSaccades = ImageThinker->GetSaccadesCompleted(); // saccades start at zero at which value a single image will be recognized
         ImageThinker->CalcAndDisplayResults( lNoOfSaccades, true, true );
         cUtils->ShowTestImage(lTestPixArray);
      }
   }
}

void MyMainWindow::GuiRecognizeFolder()
{
   // Disabling the control is interpreted by the ShowTestImage code to means that
   // we are in Learning Mode and the check should be acted upon
   this->cMainUi.UpdateImageWindow->setEnabled(false);
   QString lCurrentDir = cUtils->GetDirectory();
   QString lDirectory = QFileDialog::getExistingDirectory (NULL,
      tr("Select Folder"), lCurrentDir ); //, QFileDialog::ShowDirsOnly | QFileDialog::DontUseSheet| QFileDialog::DontUseNativeDialog );

   if ( lDirectory.size() > 0 )
   {
      // lDirectory+='\\';
      QDir::setCurrent( lDirectory ); // //SetCurrent( lDirectory );
      cUtils->SetDirectory(lDirectory);
      QString lStrSaccadesFrom = cMainUi.SaccadesFrom->text();
      unsigned lSaccadesFrom = lStrSaccadesFrom.toUInt();
      QString lStrSaccadesTo = cMainUi.SaccadesTo->text();
      unsigned lSaccadesTo = lStrSaccadesTo.toUInt();
      this->cMainUi.RecognizeFlash->setDisabled(true);
      this->cMainUi.RecognizeMoving->setDisabled(true);
      this->cMainUi.SetParameters->setDisabled(true);
      this->cMainUi.LearnFolder->setDisabled(true);
      this->cMainUi.TestSelectedFolder->setDisabled(true);
      this->cMainUi.Stop->setDisabled(false);
      ImageThinker->RecognizeFolder( lSaccadesFrom, lSaccadesTo );
      this->cMainUi.Stop->setDisabled(true);
      this->cMainUi.RecognizeFlash->setDisabled(false);
      this->cMainUi.RecognizeMoving->setDisabled(false);
      this->cMainUi.SetParameters->setDisabled(false);
      this->cMainUi.LearnFolder->setDisabled(false);
      this->cMainUi.TestSelectedFolder->setDisabled(false);
      // Notice that we display results for each folder so it has to be done in Thinker during folder iteration
   }
   this->cMainUi.UpdateImageWindow->setEnabled(true);
}


void MyMainWindow::GuiExitApp(bool pDummy)
{
   this->close();
}


int main(int argc, char **argv)
{

  QApplication app(argc, argv);

  MyMainWindow * lMainWindow = new MyMainWindow();

  lMainWindow->show();

 return app.exec();
}
