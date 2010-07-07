/*
Neocortex - Memory-Prediction Framework
Copyright (C) 2007/2008 Saulius J. Garalevicius
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

#ifndef MpfH
#define MpfH

#include "../Precompile/ui_About.h"
#include "../Precompile/ui_Neo.h"
//#include "../Precompile/ui_Parameters.h"
#include "../Framework/NeoParameters.h"

#include <QSettings>

class GuiUtils;
class Thinker;

// NB Ui is the namespace.  mainWindow just subclasses Ui_mainWindow
class MyMainWindow : public QMainWindow, public Ui_About, private Ui::mainWindow
{
   Q_OBJECT
   signals:
      //void MousePressEvent();
   public slots:
      //void SaveImageAs();
      void GuiOpenImage( bool ); //slot for menu item
      void GuiOpenImage( ); //slot for button
      void GuiLearnFolder();
      void GuiSetParameters();
      void ClearLearningAndCreateNewThinker();
      void GuiRecognizeFlashed();
      void GuiRecognizeMoving();
      void GuiRecognizeFolder();
      void GuiExitApp(bool);
      void closeEvent(QCloseEvent *pEvent);
      void GuiDisplayPredictionConfidence(bool pState);
      void Interrupt();
      void GuiAbout( bool );
      void AboutOK();
public:
   MyMainWindow(); //QWidget *parent=0);
   ~MyMainWindow()
   {
      delete cUiAbout;
      cUiAbout = NULL;
   }

   QSize cImageZoom;

private:
   Ui::mainWindow cMainUi;
   bool cDummyNoAction;
   Ui_About * cUiAbout;
   NeoParameters * cNeoParameters;

protected:
      //bool eventFilter( QObject *obj, QEvent *ev );


private:
   void ReadSettings()
   {
      QSettings settings("OpenSource", "Neocortex");
      QPoint pos = settings.value("pos", QPoint(0, 0)).toPoint();
      QSize size = settings.value("size", QSize(0, 0)).toSize();
      // First time through the values will not exist
      if ( !size.isEmpty() )
      {
         resize(size);
         move(pos);
      }
   };

   void WriteSettings()
   {
      QSettings settings("OpenSource", "Neocortex");
      settings.setValue("pos", pos());
      settings.setValue("size", size());
   }    std::vector<QGraphicsView*> PredictionOutputs;
   GuiUtils * cUtils;
   Thinker * ImageThinker;
   bool CheckIconImageContent( );
};
#endif
