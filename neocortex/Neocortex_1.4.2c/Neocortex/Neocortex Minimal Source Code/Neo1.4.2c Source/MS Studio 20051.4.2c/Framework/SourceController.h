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

#ifndef IMAGECONTROLLER_H
#define IMAGECONTROLLER_H

#include <Qt/QDir.h>
#include <Qt/QFileDialog.h>
#include "NeoParameters.h"
#include "../Precompile/ui_Neo.h"
#include "../Framework/GuiUtils.h"
#include "ISaccade.h"
//#endif

// We can have a singleton controller object and vary the pattern or we can have multiple controllers
// Because the saccade logic is fairly simple it is proposed to implement the saccading code as
// functions within a single SaccadePattern (strategy) object.
class ImageSourceController : public QObject
{
   Q_OBJECT
      signals:
         // Implements the IVision interface.
         // delivers vision information to the client by callback thought IVision
         void VisionService( BmpArray *, int );
         //void SigInterrupt();
      public slots:
         void OpenImage( bool ); // slot for menu activation
         void OpenImage( ); // slot for button
         void SaveImageAs();
         void ClearImage();
         void Rotate();
         void ZoomOut();
         void ZoomIn();
         void MoveUp();
         void MoveDown();
         void MoveLeft();
         void MoveRight();
         void Interrupt();
         void AddNoise();

private:
   int cRunning;
   BmpArray cOrigPixArray;
   GuiUtils * cUtils;
   unsigned minRow, minCol, height, width;
   int cSaccadePattern;
   int cSaccadesCompleted;
   BmpArray cTestPixArray, cSavePixArray;
   int cSaccadeCountReqd;
   void SaveCopyCurrentTestImage();
   ISaccade * cSaccade;


public:

   void RestoreCopyCurrentTestImage();

   ImageSourceController( GuiUtils* pInOut, int pImageSide )
   {
      cRunning = false;
      cUtils = pInOut;
      cSaccadePattern = NeoParameters::eRasterPattern;

      // Initialize test pix array
      BmpArray * lTestPixArray = NULL;
      lTestPixArray = new std::vector< std::vector<unsigned> >(NeoParameters::ImageSide, std::vector<unsigned>(NeoParameters::ImageSide,0));
      cTestPixArray = *lTestPixArray;
      delete lTestPixArray;
      lTestPixArray = NULL;

      ClearImage();

      QObject::connect(cUtils->GetGui().actionOpen,  SIGNAL(triggered(bool)),  this, SLOT(OpenImage(bool)) );
      QObject::connect(cUtils->GetGui().OpenImage,    SIGNAL(clicked()),    this, SLOT(OpenImage()) );
      QObject::connect(cUtils->GetGui().SaveImageAs,  SIGNAL(clicked()),    this, SLOT(SaveImageAs()) );
      QObject::connect(cUtils->GetGui().ClearImage,   SIGNAL(clicked()),    this, SLOT(ClearImage()) );
      QObject::connect(cUtils->GetGui().Rotate,       SIGNAL(clicked()),    this, SLOT(Rotate()) );
      QObject::connect(cUtils->GetGui().zoomOut,      SIGNAL(clicked()),    this, SLOT(ZoomOut()) );
      QObject::connect(cUtils->GetGui().zoomIn,       SIGNAL(clicked()),    this, SLOT(ZoomIn()) );
      QObject::connect(cUtils->GetGui().ShiftUp,      SIGNAL(clicked()),    this, SLOT(MoveUp()) );
      QObject::connect(cUtils->GetGui().ShiftDown,      SIGNAL(clicked()),    this, SLOT(MoveDown()) );
      QObject::connect(cUtils->GetGui().ShiftLeft,      SIGNAL(clicked()),    this, SLOT(MoveLeft()) );
      QObject::connect(cUtils->GetGui().ShiftRight,      SIGNAL(clicked()),    this, SLOT(MoveRight()) );
      QObject::connect(cUtils->GetGui().AddNoise,      SIGNAL(clicked()),    this, SLOT(AddNoise()) );

      //QObject::connect(this, SIGNAL( SigInterrupt() ), this, SLOT( Interrupt()) );
      //QObject::connect(cUtils->GetGui().Stop,   SIGNAL(clicked()),    this, SLOT(Interrupt()) );
   };

   virtual ~ImageSourceController(void){};

   //void keyPressEvent(QKeyEvent *event){};

   void SetBitmap(BmpArray &pPixArray, int pSideCompression);

   void SetSaccadePattern( int pPattern, ISaccade *pSaccade  );
   void SetSaccadeCountReqd(int pSaccadeCount);
   int  GetSaccadesCompleted();

   void run(int pCalledFor);

   //void FindImageBoundaries(BmpArray &pixArray, unsigned &minRow, unsigned &minCol,
   //   unsigned &height, unsigned &width);

   //bool ShiftImage(  BmpArray *pOrigPixArray, unsigned minRow, unsigned minCol,
   //   unsigned height, unsigned width,
   //   int shiftedRow, int shiftedCol, BmpArray *pShiftedPixArray);

};

class FileSourceController : public QObject
{
   Q_OBJECT
      signals:
      // Implements the IFileService interface.
      // delivers file names to the client by callback thought IFileService
      void FileService( std::string pFile, std::string pSubdir, int );
      //void SigInterrupt();
     public slots:
         void Interrupt();

private:
   bool cIWasInterrupted;
   QString cInterruptMessage;
   GuiUtils * cUtils;
   QString cOrigFolder;
   int cSaccadeCount;
   int cRunning;

public:

   FileSourceController( GuiUtils* pUtils )
   {
      cUtils=pUtils;
      QObject::connect(cUtils->GetGui().Stop,   SIGNAL(clicked()),    this, SLOT(Interrupt()) );
      //QObject::connect(this, SIGNAL( SigInterrupt() ), this, SLOT( Interrupt()) );
      cIWasInterrupted = false;
      cRunning = false;
   };

   virtual ~FileSourceController(void){};

   bool run(int pRunFor, std::string pInterruptMessage, int pLimitPerFolder=-1, int pNoOfFoldersToRead=-1); // -1 default means no limit

   void SetTopLevelFolder( QString &pFolder );

};
#endif
