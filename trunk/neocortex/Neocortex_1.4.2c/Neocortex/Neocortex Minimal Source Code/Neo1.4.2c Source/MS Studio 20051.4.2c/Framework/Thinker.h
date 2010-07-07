/*
Neocortex - Memory-Prediction Framework
Copyright (C) 2007 Saulius J. Garalevicius
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

#ifndef ThinkerH
#define ThinkerH

class ImageSourceController;
class FileSourceController;
#include <QFileDialog>
#include "NeoParameters.h"
#include "../Precompile/ui_Neo.h"
#include "../Framework/GuiUtils.h"
class NeoRegion;
class BitmapVision;
class Hippocampus;

class Thinker : public QObject
{
   Q_OBJECT
      public slots:
         // This is the equivalent of an interface
         // SourceController implements the interface
         // Every class, such as BitmapVision, that has this slot will get the signal from anyone who implements the interface.
         //
         // Because there is more than one slot per signal, learning/recognition mode is handled by having two 
         // signatures for the interface (two interfaces if you prefer); using a dummy int parameter to distinguish them.
         // A more generic method might be better - perhaps having the parameter for all slots and using aGUID to distinguish
         // the actual target.  So the variable in the distinguishing parameter has been called pSource
         void IVisionService (BmpArray * pPixMap, int pSource); 
         void IFileService   (std::string pFileName, std::string pSubdirectory, int pFor);
            
      signals: // MUST have this

private:
    //bool cRunning;
    bool cIWasInterrupted;

    void LearnExample   (std::string pFileName, std::string pSubdirectory);
    void RecognizeFile(std::string pFileName, std::string pSubdirectory);
    NeoRegion **Cortex;
    BitmapVision *Eye;
    Hippocampus *Hippo;
    GuiUtils &cUtils;
    BmpArray cThinkerPixArray;
    std::string cPreviousExample;

    void LearnImage(BmpArray &pixArray, std::string name,
                    unsigned pLearningRegion, bool pFeedbackStage); // I.e. Feedback is possible because we are above region zero

    void BeginRecognition();
    void ProcessSaccade(BmpArray &pixArray);
    void LearnExample(BmpArray &pOrigPixArray, std::string name,
                      unsigned learningRegion, bool feedbackPossible);
    bool cInitForInferenceDone;
    ImageSourceController *cIImageSourceContoller;  
    FileSourceController  *cIFileSourceContoller;  
    double cIFileRecognitionCumulativeAccuracy;
    int cIFileRecognitionNumTests;

    std::string cName;
    unsigned   cLearningRegion; 
    bool       cFeedbackStage;
    unsigned cNumberOfRegions;
    bool cLearningValid;

public:
    Thinker(GuiUtils *inout );
    virtual ~Thinker();
    void LearnFolder();
    bool LearningValid();
    void InitForInference();
    bool LearningDone();

    void   RecognizeFlashed(BmpArray &pPixArray);
    void   RecognizeMoving(BmpArray &pPixArray, int pSaccadeCount);
private:
    void   RecognizeMoving(BmpArray &pPixArray);
public:
    int    GetSaccadesCompleted();
    void   RecognizeFolder(int pSaccadesMin, int pSaccadesMax);
    double RecogniseAndTestAccuracy(BmpArray &pPixArray, std::string correctName);

    void DisplayAllMemories();

    void CalcAndDisplayResults(unsigned int pSaccadeCount, bool  pLog, bool pImages);

    FileSourceController * GetFileController();
    
    // If Thinker holds number of new categories for adding to learning we may need something like this
    // unsigned cNoOfNewCategories;
    // void InitLearning() { //cNoOfNewCategories=0; };
    // unsigned GetNoOfNewCategories(){ return cNoOfNewCategories; };

};

#endif
