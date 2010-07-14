/*
Neocortex - Memory-Prediction Framework
Copyright (C) 2007/2008 Saulius J. Garalevicius 
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

#include <Qt/QDir.h>
#include <Qt/QFileDialog.h>
#include <Qt/QInputDialog.h>
#include "../Framework/NeoParameters.h"
#include "../Precompile/ui_Neo.h"
#include "../Framework/GuiUtils.h"
#include "ISaccade.h"
#include "SaccadeImpl.h"

#include "SourceController.h" // required for mingw

#include "../iconeditor/iconeditor.h"

void ImageSourceController::Interrupt()
{
   // This is so that we can interrupt a long saccade sequence
   if ( cRunning )
   {
      // Check model for FileSourceController before reinstating this
      //cRunning = false; // only interrupts the current image processing
      //QInputDialog::getText(cUtils->GetGui().centralwidget, QString("Press Enter to continue"), QString("Neo is Paused") );
   }
};

void ImageSourceController::SaveCopyCurrentTestImage()
{
   // Get this from the editor - i.e. get what is on display and save it
   for ( int i=0; i< NeoParameters::ImageSide; i++ )
   {
      for ( int j=0; j< NeoParameters::ImageSide; j++ )
      {
         (cSavePixArray)[i][j] = (cTestPixArray)[i][j];
      }
   }
}

void ImageSourceController::RestoreCopyCurrentTestImage()
{
   if ( cSavePixArray.size() > 0 )
   {
      for ( int i=0; i< NeoParameters::ImageSide; i++ )
      {
         for ( int j=0; j< NeoParameters::ImageSide; j++ )
         {
            (cTestPixArray)[i][j] = (cSavePixArray)[i][j];
         }
      }
      // Why does this not work?
      cUtils->ShowTestImage(cTestPixArray);
      QApplication::processEvents();
   }
}    

void ImageSourceController::SetBitmap(BmpArray &pPixArray, int pSideCompression)
{
   cOrigPixArray = pPixArray;
}; // Gets bitmap from somewhere and the controller makes a copy

void ImageSourceController::SetSaccadePattern( int pPattern, ISaccade *pSaccade )
{
   cSaccadePattern = pPattern;
   cSaccade = pSaccade;
   // Re-emit signal from saccade
   QObject::connect(cSaccade, SIGNAL( VisionService( BmpArray *, int ) ), this, SIGNAL (VisionService( BmpArray *, int )) );
};

void ImageSourceController::SetSaccadeCountReqd(int pSaccadeCountReqd)
{
   cSaccadeCountReqd = pSaccadeCountReqd;
};

int ImageSourceController::GetSaccadesCompleted()
{
   return cSaccadesCompleted; 
}

// The Saccade class emits each image of the selected saccade sequence in turn.
void ImageSourceController::run( int pCalledFor )
{
   cRunning = true;
   BmpArray * lpShiftedPixArray;
   cSaccade->Initialize( &cOrigPixArray, NeoParameters::ImageSide,  cUtils->GetNeoParameters()->GetCoveredImageSide() );
   cSaccadesCompleted = 0;
   lpShiftedPixArray = cSaccade->Run( pCalledFor, cSaccadesCompleted, cSaccadeCountReqd );
   // It is important to disconnect so that the creator of the saccade pattern can delete it.
   QObject::disconnect(cSaccade, SIGNAL( VisionService( BmpArray *, int ) ), this, SIGNAL (VisionService( BmpArray *, int )) );

   cRunning = false;  // is this needed?
}


void ImageSourceController::ClearImage()
{
   // If the 'Clear' button has been pressed when images are being processed we do 
   // not want to clear the current image.  Clearing the image can still be done by the user.
   if ( !cRunning )
   {
      BmpArray * lSavePixArray;
      lSavePixArray = NULL;
      lSavePixArray = new std::vector< std::vector<unsigned> >(NeoParameters::ImageSide, std::vector<unsigned>(NeoParameters::ImageSide,0));//unsigned * [ImageSide];
      cSavePixArray = *lSavePixArray;
      delete lSavePixArray;
      lSavePixArray = NULL;

      for ( int i=0; i< NeoParameters::ImageSide; i++ )
      {
         for ( int j=0; j< NeoParameters::ImageSide; j++ )
         {
            (cTestPixArray)[i][j] = cUtils->LogicalWhite;
         }
      }
      // Clear current image
      SaveCopyCurrentTestImage();
      cUtils->ShowTestImage(cTestPixArray);
   }
   // emit SigInterrupt(); // This is only a local signal and does nothing at 11/01/2008
}

void ImageSourceController::OpenImage( ) // for button click
{
   OpenImage( true ); // arbitrary value
}
void ImageSourceController::OpenImage( bool pDummy )
{
   //QApplication::processEvents();
   QString lCurrentDir = cUtils->GetDirectory();
   QDir::setCurrent( lCurrentDir ); 
   QString lCurrentFile  
      = QFileDialog::getOpenFileName( cUtils->GetGui().centralwidget,
                                      tr("Open file"), 
                                      lCurrentDir, 
                                      tr("Image files (*.bmp)") ); 
   //,0 ,QFileDialog::DontUseNativeDialog | QFileDialog::DontUseSheet | QFileDialog::DontResolveSymlinks );

   if ( lCurrentFile.length() > 0 )
   {
      QImage lImage0( (const QString)lCurrentFile ); 
      QSize lSizeCheck = lImage0.size();
      QSize lExpectedSize(NeoParameters::ImageSide,NeoParameters::ImageSide);
      bool lImageOK = ( lSizeCheck == lExpectedSize );
      if ( lImageOK )
      {
         QImage * lThisImage = new QImage(NeoParameters::ImageSide,NeoParameters::ImageSide,QImage::Format_RGB32);
         *lThisImage = lImage0.convertToFormat(QImage::Format_RGB32);

         cUtils->ImageToBmpArray( cTestPixArray, lThisImage );

         SaveCopyCurrentTestImage();

         cUtils->ShowTestImage(cTestPixArray);

         delete lThisImage;
         lThisImage = NULL;

      }
   }
   // Cannot seem to make this work.
   //else
   //{
   //   RestoreCopyCurrentTestImage();
   //   QApplication::processEvents();
   //}
}

void ImageSourceController::SaveImageAs()
{
   const QString lCurrentDir = cUtils->GetDirectory();
   QDir::setCurrent( lCurrentDir );
   QString lCurrentFile 
      = QFileDialog::getSaveFileName( cUtils->GetGui().centralwidget, 
                                      tr("Save file") ,
                                      lCurrentDir, 
                                      tr("Image files (*.bmp)") ); 
   //,0 ,QFileDialog::DontUseNativeDialog | QFileDialog::DontUseSheet | QFileDialog::DontResolveSymlinks );

   if ( lCurrentFile.length() > 0 )
   {
      QImage lImage = cUtils->GetGui().iconeditor->iconImage();
      bool lSuccess = lImage.save(lCurrentFile, "BMP" );
      if ( !lSuccess )
      {
         cUtils->Log("Failed to save file");
      }
   }
}

void ImageSourceController::Rotate()
{
   cUtils->GetGui().iconeditor->rotate90();
   QImage lImage = cUtils->GetGui().iconeditor->iconImage();

   // Update application's bitmap
   cUtils->ImageToBmpArray(cTestPixArray, &lImage);
   // May need to do this to ensure update of screen
   cUtils->ShowTestImage(cTestPixArray);

   return;
}


void ImageSourceController::ZoomOut()
{
   cUtils->GetGui().iconeditor->scale(double(0.8),double(0.8));

   QImage lImage = cUtils->GetGui().iconeditor->iconImage();
   // Update application's bitmap
   cUtils->ImageToBmpArray(cTestPixArray, &lImage);
   // May need to do this to ensure update of screen
   //cUtils->ShowTestImage(cTestPixArray);

   return;
}

void ImageSourceController::ZoomIn()
{
   cUtils->GetGui().iconeditor->scale(double(1.2),double(1.2));

   QImage lImage = cUtils->GetGui().iconeditor->iconImage();
   // Update application's bitmap
   cUtils->ImageToBmpArray(cTestPixArray, &lImage);
   // May need to do this to ensure update of screen
   //cUtils->ShowTestImage(cTestPixArray);

   return;
}

void ImageSourceController::MoveUp()
{
   cUtils->GetGui().iconeditor->moveXY(0,-1);

   QImage lImage = cUtils->GetGui().iconeditor->iconImage();
   // Update application's bitmap
   cUtils->ImageToBmpArray(cTestPixArray, &lImage);
   // May need to do this to ensure update of screen
   //cUtils->ShowTestImage(cTestPixArray);
   return;

}
void ImageSourceController::MoveDown()
{
   cUtils->GetGui().iconeditor->moveXY(0,1);

   QImage lImage = cUtils->GetGui().iconeditor->iconImage();
   // Update application's bitmap
   cUtils->ImageToBmpArray(cTestPixArray, &lImage);
   // May need to do this to ensure update of screen
   //cUtils->ShowTestImage(cTestPixArray);
   return;
}
void ImageSourceController::MoveLeft()
{
   cUtils->GetGui().iconeditor->moveXY(-1,0);

   QImage lImage = cUtils->GetGui().iconeditor->iconImage();
   // Update application's bitmap
   cUtils->ImageToBmpArray(cTestPixArray, &lImage);
   // May need to do this to ensure update of screen
   //cUtils->ShowTestImage(cTestPixArray);
   return;
}
void ImageSourceController::MoveRight()
{
   cUtils->GetGui().iconeditor->moveXY(1,0);

   QImage lImage = cUtils->GetGui().iconeditor->iconImage();
   // Update application's bitmap
   cUtils->ImageToBmpArray(cTestPixArray, &lImage);
   // May need to do this to ensure update of screen
   //cUtils->ShowTestImage(cTestPixArray);
   return;
}

void ImageSourceController::AddNoise()
{
   // Set the border so that noise is only set in the vicinity of the image
   // Set the percentage noise to approximately 10% of pixels in the setting area
   int lNoiseFactorX10 = cUtils->GetGui().NoiseFactor10->value();
   int lNoiseFactorX100 = cUtils->GetGui().NoiseFactor100->value();
   bool lInvertPixels = (cUtils->GetGui().InverNoisePixels->checkState() == Qt::Checked);
   cUtils->GetGui().iconeditor->AddNoise( float(lNoiseFactorX10)/10.0 + float(lNoiseFactorX100)/100.0, lInvertPixels );
   return;
}

void FileSourceController::SetTopLevelFolder(QString &pFolder)
{
   cOrigFolder = pFolder;
};


void FileSourceController::Interrupt()
{
   if ( cRunning )  // Prevents display of message when clearing no run  in progress.
   {
      int lRes = QMessageBox::warning(NULL, tr("Neocortex"), cInterruptMessage, QMessageBox::Yes | QMessageBox::No);
      if ( lRes == QMessageBox::Yes )
      {
         cIWasInterrupted = true;
      }
      else
      {
         cIWasInterrupted = false;
      }
   }
   //QInputDialog::getText(cUtils->GetGui().centralwidget, QString("Press Enter to continue"), QString("Neo is Paused") );
};

bool FileSourceController::run( int pRunFor, std::string pInterruptMessage, int pLimitPerFolder/*=-1*/, int pNoOfFoldersToRead/*=-1*/ )
{
   // Does learning or retrieval for all folders.
   // Each folder represents a 'category' of object (cat, dog etc.)
   // Within each folder are example images of the category

   // Interrupt flag is local and set each time run is called.
   // It can only be set by FileSourceController::Interrupt()

   // Interrupt flag is returned by run(...)

   cInterruptMessage = pInterruptMessage.c_str();
   cIWasInterrupted = false;
   cRunning = true;

   QDir lDirectory = cUtils->GetDirectory();
   QString lRoot(lDirectory.absolutePath());
   lRoot+='/'; // absolutePath returns without trailing //

   QString lFilenameOut("");

   unsigned lNoOfFoldersRead = 0;
   // For each folder
   foreach (QString lqstrSubDir, lDirectory.entryList( QDir::Dirs | QDir::NoDotAndDotDot ))
   {
      lNoOfFoldersRead++;
      int lFileCount = 0;
      QDir lSubDirectory(lRoot+lqstrSubDir);

      // For each example file in the folder
      foreach ( QString lFile,lSubDirectory.entryList(QDir::Files) )
      {
         if ( lFile.endsWith(QString(".bmp"),Qt::CaseInsensitive) )
         {
            cUtils->Log("Example file " + lFile.toStdString());
            QString lPath = lSubDirectory.absolutePath();
            // absolutePath returns without trailing //
            QString lqFileNameOut( lPath + '/' + lFile );
            std::string lFileNameOut = lqFileNameOut.toStdString();
            std::string lstrSubDir = lqstrSubDir.toStdString();
            emit FileService( lFileNameOut, lstrSubDir, pRunFor ) ;
            lFileCount++;
            if ( (pLimitPerFolder >0) && ( lFileCount==pLimitPerFolder) )
            {
               break;
            }
         }
      }

      if ( cIWasInterrupted )
      {
         break;
      }

      // if pNoOfFoldersToRead==0 or -1 it will just carry on to end of set
      // otherwise it will stop now if we have read all the required folders.
      if ( pNoOfFoldersToRead == lNoOfFoldersRead )
      {
         break;
      }
   } 
   cRunning = false;
   return cIWasInterrupted;
}
