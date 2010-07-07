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

#ifndef InOutH
#define InOutH

#include "NeoParameters.h"

typedef std::vector< std::vector<unsigned> > BmpArray;

class GuiUtils //: public QDialog
{
  private:
     QString lCurrentFile;

     std::vector<QPixmap*> cCategoryBitmaps;
     std::vector<QGraphicsView*> cPredictionOutputs;
     std::vector<QProgressBar*> cPredictionConfidence;
     std::vector<double> cCurrentPredictions;
     Ui::mainWindow *cGui;

     //int cSaccadesFrom;
     //int cSaccadesTo;
     QString cCurrentDirectory;
     QGraphicsScene * cScene;
     QGraphicsPixmapItem * cPixmapItem;
     QPixmap * cCurrentImage;

     // Number of categories learned.
     unsigned cNoOfNewCategories;
     NeoParameters * cNeoparameters;

  public:
    unsigned GetNoOfCategories(){ return cCategoryBitmaps.size(); };
    void AddCategoryBitmap( QString& pFullPath);
    void ClearCategoryBitmaps();
    void BmpArrayToImage(QImage **pImage, BmpArray &pPixArray);
    GuiUtils(Ui::mainWindow *pGUI);
    const unsigned LogicalBlack;
    const unsigned LogicalWhite;
    void Log(const std::string s);
    void Summary(const std::string s);
    void ImageToBmpArray(BmpArray &pPixArray, QImage *pImage);
    void ShowTestImage(BmpArray &pixArray);
    BmpArray GetTestImage();
    Ui::mainWindow& GetGui(){ return *cGui; }
    void DisplayPredictionImages(std::vector<unsigned> indices, std::vector<double> pMaxBel);
    void SetDirectory(QString);
    QString  &GetDirectory();
    //void Pause();
    void DisplayPredictionConfidence(bool pState);
    NeoParameters * GetNeoParameters() { return cNeoparameters; };
};
#endif
