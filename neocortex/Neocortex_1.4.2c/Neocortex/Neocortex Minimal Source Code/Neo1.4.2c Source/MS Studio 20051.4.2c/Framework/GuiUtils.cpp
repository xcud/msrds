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

#include <string>
#include <vector>
#include <Qt/QPixmap.h>
#include <Qt/QDir.h>
#include <Qt/QRect.h>
#include <Qt/QSize.h>
#include <Qt/QImage.h>
//#include "../iconeditor/iconeditor.h"
#include "../Precompile/ui_Neo.h"
#ifdef _WINDOWS
#include <exception>
#endif
#include <Qt/QInputDialog.h>

#include "GuiUtils.h"

GuiUtils::GuiUtils(Ui::mainWindow *pGui)
: LogicalBlack(0), LogicalWhite(1)
{
  cGui = pGui;
  cNeoparameters = new NeoParameters();

  //unsigned test = NeoParameters::ImageSide;

  // cScene = &pScene;
  cPixmapItem = NULL;
  cCurrentImage = NULL;
  //Get into array form for convenience
  cPredictionOutputs.push_back(cGui->graphicsView0);
  cPredictionOutputs.push_back(cGui->graphicsView1);
  cPredictionOutputs.push_back(cGui->graphicsView2);
  cPredictionOutputs.push_back(cGui->graphicsView3);
  cPredictionOutputs.push_back(cGui->graphicsView4);
  cPredictionOutputs.push_back(cGui->graphicsView5);
  cPredictionOutputs.push_back(cGui->graphicsView6);
  cPredictionOutputs.push_back(cGui->graphicsView7);
  cPredictionOutputs.push_back(cGui->graphicsView8);
  cPredictionOutputs.push_back(cGui->graphicsView9);

  cPredictionConfidence.push_back(cGui->progressBar_1);
  cPredictionConfidence.push_back(cGui->progressBar_2);
  cPredictionConfidence.push_back(cGui->progressBar_3);
  cPredictionConfidence.push_back(cGui->progressBar_4);
  cPredictionConfidence.push_back(cGui->progressBar_5);
  cPredictionConfidence.push_back(cGui->progressBar_6);
  cPredictionConfidence.push_back(cGui->progressBar_7);
  cPredictionConfidence.push_back(cGui->progressBar_8);
  cPredictionConfidence.push_back(cGui->progressBar_9);
  cPredictionConfidence.push_back(cGui->progressBar_10);

  cGui->progressBar_1->setRange(0,10);
  cGui->progressBar_2->setRange(0,10);
  cGui->progressBar_3->setRange(0,10);
  cGui->progressBar_4->setRange(0,10);
  cGui->progressBar_5->setRange(0,10);
  cGui->progressBar_6->setRange(0,10);
  cGui->progressBar_7->setRange(0,10);
  cGui->progressBar_8->setRange(0,10);
  cGui->progressBar_9->setRange(0,10);
  cGui->progressBar_10->setRange(0,10);

  for ( int i = 0; i<10; i++ )
     cCurrentPredictions.push_back(0.0);
   
  // This counts the number of categories.
  // It will be the same as the size of the CategoryBitmaps vector.
  // It can be used in cases where we want to add to learned data although this has not been implemented so far.
   cNoOfNewCategories = 0;
   cCurrentDirectory = "";
}

void GuiUtils::Log(const std::string s){
   //cGui->ResultMemo->show();
   if ( cGui->ScrollText->isChecked() )
   {
      cGui->ResultMemo->append(s.c_str());
      QRect lRect = cGui->ResultMemo->contentsRect();
      cGui->ResultMemo->update(lRect);
      cGui->ResultMemo->repaint(lRect);
      QApplication::processEvents();
   }
}

void GuiUtils::Summary(const std::string s){
   cGui->SummaryMemo->append(s.c_str());
   QRect lRect = cGui->SummaryMemo->contentsRect();
   cGui->SummaryMemo->update(lRect);
   cGui->SummaryMemo->repaint(lRect);
}

// Converts a QPixmap into an array of bytes
void GuiUtils::ImageToBmpArray( BmpArray &pPixArray, QImage *pImage )
{
   unsigned r, c;

   QColor lColour;
   unsigned int lAlphaMask = 0x00FFFFFF;

   for(r = 0; r < NeoParameters::ImageSide; r++)
      for(c = 0; c < NeoParameters::ImageSide; c++)
      {
         unsigned tt = pImage->pixel(c,r);
         if ( (pImage->pixel(c,r) & lAlphaMask) == lColour.black() )
         {
            (pPixArray)[r][c] = LogicalBlack;
         }
         else
         {
            (pPixArray)[r][c] = LogicalWhite;
         }
      }
}

void GuiUtils::BmpArrayToImage(QImage **pImage, BmpArray &pPixArray)
{
   *pImage = new QImage(NeoParameters::ImageSide,NeoParameters::ImageSide, QImage::Format_RGB32);
   unsigned int lAlphaMask = 0; //0x00FFFFFF;
   QColor lBlack(0,0,0,lAlphaMask);
   QColor lWhite(255,255,255,lAlphaMask);
   for(unsigned r = 0; r < NeoParameters::ImageSide; r++)
   {
      for(unsigned c = 0; c < NeoParameters::ImageSide; c++)
      {
         if((pPixArray)[r][c] == LogicalBlack)
         {
            (*pImage)->setPixel(c,r,lBlack.rgb());
         }
         else
         if((pPixArray)[r][c] == LogicalWhite)
         {
            (*pImage)->setPixel(c,r,lWhite.rgb());
         }
#ifdef _WINDOWS
         else
         {
            throw std::exception("Invalid pixel value " ); //+ IntToStr(r) + ", " + IntToStr(c));
         }
#endif
      }
   }
}

// Create an image from the bitmap array representation.
// Then show the image in the Image window on the form.
void GuiUtils::ShowTestImage(BmpArray  &pPixArray)
{
   QImage * lImage = NULL;
#ifdef _WINDOWS
   try
   {    BmpArrayToImage(&lImage, pPixArray); }
   catch ( const char * e )
   { throw std::exception(e); }
#else
   BmpArrayToImage(&lImage, pPixArray);
#endif
   cGui->iconeditor->setIconImage(*lImage);
   // Previous version using ImageView
   //cGui->ImageView->updateSceneRect( cGui->ImageView->rect() );
   cGui->iconeditor->repaint();
   QApplication::processEvents();
}

// Create an image from the bitmap array representation.
// Then show the image in the Image window on the form.
BmpArray GuiUtils::GetTestImage()
{
   BmpArray * lTempPixArray;
   lTempPixArray = NULL;
   lTempPixArray = new std::vector< std::vector<unsigned> >(NeoParameters::ImageSide, std::vector<unsigned>(NeoParameters::ImageSide,0));//unsigned * [NeoParameters::ImageSide];
   BmpArray lRetImage = *lTempPixArray;
   delete lTempPixArray;
   lTempPixArray = NULL;

   QImage lImage = cGui->iconeditor->getIconImage();
   ImageToBmpArray(lRetImage, &lImage);
   return lRetImage;
}
//void GuiUtils::Pause()
//{
//   QInputDialog::getText(cGui->centralwidget, QString("Press Enter to continue"), QString("Neo is Paused") );
////  ShowMessage("Press any key to continue...");
//}

void GuiUtils::DisplayPredictionImages(std::vector<unsigned> indices, std::vector<double> pMaxBel)
{
  size_t lNrCategories = GetNoOfCategories();
  for(unsigned i=0; i < NeoParameters::PredictionCount; i++)
  {
    QGraphicsScene * lScene = new QGraphicsScene;
    //this->cPredictionConfidence[i]->setValue(pMaxBel[i]*10);
    cCurrentPredictions[i] = pMaxBel[i];
    if ( indices[i] < lNrCategories )
    {
       lScene->addPixmap(*(cCategoryBitmaps[indices[i]]));
       QGraphicsScene * temp = this->cPredictionOutputs[i]->scene();
       delete temp;
       temp = NULL;
       this->cPredictionOutputs[i]->setScene(lScene);
       this->cPredictionOutputs[i]->setFixedSize(30,30);
       this->cPredictionOutputs[i]->show();
    }
  }

  // Parameter is currently ignored
  DisplayPredictionConfidence(false);
}

// Displays the [PredictionCount] best predictions on the form and a graphical representation
// Using a progress bar.
void GuiUtils::DisplayPredictionConfidence(bool pState)
{
   // NB pState is not valid unless the state has changed
   double lNormalizationConstant = cCurrentPredictions[0];
   double lCurrentvalue;
   for(unsigned i=0; i < NeoParameters::PredictionCount; i++)
   {
      lCurrentvalue = cCurrentPredictions[i];
      if ( cGui->NormalizeResults->checkState() == Qt::Unchecked )
      {
         this->cPredictionConfidence[i]->setValue(lCurrentvalue*10.0);
      }
      else
      {
         this->cPredictionConfidence[i]->setValue(lCurrentvalue*10.0/lNormalizationConstant);
      }
      //this->cPredictionConfidence[i]->repaint();
      //QApplication::processEvents();
   }
}


// Replaces ReadCategoryBitmaps
// Allows us to interrupt processing at level zero.
// This just loads the supplied image and saves it to display in the
// category windows (prediction Outputs).
// Note that this must be called during learning in order of the categories
void GuiUtils::AddCategoryBitmap( QString& pFullPath )
{
   QPixmap * lPixmap = new QPixmap();
   bool lbSucceeded = lPixmap->load(pFullPath);
   cCategoryBitmaps.push_back( lPixmap );
   cNoOfNewCategories++;
}

void GuiUtils::ClearCategoryBitmaps()
{
   cGui->progressBar_1->setRange(0,10);
   cGui->progressBar_2->setRange(0,10);
   cGui->progressBar_3->setRange(0,10);
   cGui->progressBar_4->setRange(0,10);
   cGui->progressBar_5->setRange(0,10);
   cGui->progressBar_6->setRange(0,10);
   cGui->progressBar_7->setRange(0,10);
   cGui->progressBar_8->setRange(0,10);
   cGui->progressBar_9->setRange(0,10);
   cGui->progressBar_10->setRange(0,10);

   for ( int i = 0; i<10; i++ )
   {
      //QGraphicsScene * temp = new QGraphicsScene();
      this->cPredictionOutputs[i]->setScene(NULL);
      this->cPredictionOutputs[i]->setFixedSize(30,30);
      this->cPredictionOutputs[i]->show();
      cPredictionConfidence[i]->setValue(0.0);   
   }
   cCategoryBitmaps.clear();
   cNoOfNewCategories=0;
}

void GuiUtils::SetDirectory(QString pCurrentDirectory)
{
   cCurrentDirectory = pCurrentDirectory;
   // Removed ReadCategoryBitmaps();
   // We may interrupt processing so we do not want to read all the bitmaps here.
   // Rather we should read the first image of each category as it is learned
   // See AddCategoryBitmap()
}

QString &GuiUtils::GetDirectory()
{
   return cCurrentDirectory;
}
