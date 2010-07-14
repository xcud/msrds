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
#include "../Precompile/ui_Parameters.h"
#include <QtGui/QSpinBox.h>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <Qt/QString.h>
#include <QtGui/QMessageBox>
#include <QtGui/QWidget>
#define NO_OVERLAP

NeoParameters::NeoParameters()
{
   cBottomRegionSide = 0;
   cCoveredImageSide = 0;
   cRegionCount = 0;
   //number of pixels overlapping between adjacent sub-regions
   cEyeOverlap = 0;
   cParameterDialog = NULL;

   cUiNeoParameters = new Ui_Parameters();
   cParameterDialog = new QDialog();
   cUiNeoParameters->setupUi( cParameterDialog );

   // Munge around to get teh window without a system menu. 
   // Does not seem possible simply to XOR out he system menu.
   Qt::WindowFlags lNewFlags;
   lNewFlags = Qt::FramelessWindowHint;
   lNewFlags |= Qt::WindowTitleHint;
   const Qt::WindowFlags lOldFlags = cParameterDialog->windowFlags();
   cParameterDialog->setWindowFlags( lNewFlags );

   QObject::connect(cUiNeoParameters->Revert, SIGNAL(clicked()), (const QObject *) this, SLOT(ParamsRevert()) );
   QObject::connect(cUiNeoParameters->OK, SIGNAL(clicked()), (const QObject *) this, SLOT(ParamsOK()) );

   // Do not set the callbacks until we are ready to use them
   cRegionCount = 0;
   SetDefaultsForRegionCount(3);  // force defaults
   SetControlsForMemoryStrategy(false);
   DisplaySideSize();
   // Now set the control callbacks
   // Note that we do read all of them whenever any one changes but there are nor real performance issues and it simplifies the code.
   QObject::connect(cUiNeoParameters->RegionCount, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(SetDefaultsForRegionCount(int)));
   QObject::connect(cUiNeoParameters->DeletionByPercentage, SIGNAL(toggled(bool)), (const QObject *) this, SLOT(SetDefaultsForRegionCount(bool)));
   // If any spinner is changed update all of them even though only  one has changed.  Saves code and performance not an issue.
   //sequence is forgotten if its frequency < average frequency * cForgetThreshold
   //larger threshold - more is forgotten; zero threshold - nothing is forgotten
   QObject::connect(cUiNeoParameters->ForgetThreshold_0, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->ForgetThreshold_1, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->ForgetThreshold_2, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->ForgetThreshold_3, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));

   QObject::connect(cUiNeoParameters->MaxMemory_0, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->MaxMemory_1, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->MaxMemory_2, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->MaxMemory_3, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->DoNotForget, SIGNAL(toggled(bool)),     (const QObject *) this, SLOT(SetControlsForMemoryStrategy(bool)));


   QObject::connect(cUiNeoParameters->SideCompression_0, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->SideCompression_1, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->SideCompression_2, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->SideCompression_3, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));

   QObject::connect(cUiNeoParameters->LowUsageThreshold_0, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->LowUsageThreshold_1, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->LowUsageThreshold_2, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->LowUsageThreshold_3, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));


#ifdef OVERLAP
   cEyeOverlap = 2;
#else
   cEyeOverlap = 0;
#endif
}

void NeoParameters::SetParametersDialogExec( int& pChanged, bool pLearningValid )
{
   SaveOriginal();
   cParameterDialog->setModal(true);
   cChanged=false;
   cUiNeoParameters->Revert->setEnabled(false);
   cLearningValid = pLearningValid;
   cParameterDialog->exec();
   pChanged = cChanged;
}

void NeoParameters::ParamsOK()
{
   if ( cLearningValid && cChanged )
   {
      int i = QMessageBox::warning( NULL, tr("Neocortex"), tr("Current trained hierarchy will be deleted.  Do you wish to delete the hierarchy?"),
                                   QMessageBox::Yes | QMessageBox::No);
      if ( i == QMessageBox::No )
      {
         return;
      }
   }
   cParameterDialog->hide();
}

void NeoParameters::ParamsRevert()
{
   RestoreOriginal();
   cChanged = false;
//   QObject::disconnect(cUiNeoParameters->Revert, SIGNAL(clicked()), (const QObject *) this, SLOT(ParamsRevert()) );
   cUiNeoParameters->Revert->setEnabled(false);
//   QObject::connect(cUiNeoParameters->Revert, SIGNAL(clicked()), (const QObject *) this, SLOT(ParamsRevert()) );
   //cParameterDialog->hide();
}

// Sets the required region count.
// Can be called when the spinner is changed.
// Can also be called from the main program thus requiring the spinner to be set


void NeoParameters::SetControlsForMemoryStrategy( bool pConstrainMemory )
{
   if ( pConstrainMemory )
   {
      cUiNeoParameters->ForgetThreshold_0->setDisabled( true );
      cUiNeoParameters->ForgetThreshold_1->setDisabled( true );
      cUiNeoParameters->ForgetThreshold_2->setDisabled( true );
      cUiNeoParameters->ForgetThreshold_3->setDisabled( true );
      cUiNeoParameters->MaxMemory_0->setDisabled(false);
      cUiNeoParameters->MaxMemory_1->setDisabled(false);
      cUiNeoParameters->MaxMemory_2->setDisabled(false);
      cUiNeoParameters->MaxMemory_3->setDisabled(false);
      cUiNeoParameters->LowUsageThreshold_0->setDisabled(false);
      cUiNeoParameters->LowUsageThreshold_1->setDisabled(false);
      cUiNeoParameters->LowUsageThreshold_2->setDisabled(false);
      cUiNeoParameters->LowUsageThreshold_3->setDisabled(false);
   }
   else
   {
      cUiNeoParameters->ForgetThreshold_0->setDisabled( false );
      cUiNeoParameters->ForgetThreshold_1->setDisabled( false );
      cUiNeoParameters->ForgetThreshold_2->setDisabled( false );
      cUiNeoParameters->ForgetThreshold_3->setDisabled( false );
      cUiNeoParameters->MaxMemory_0->setDisabled(true);
      cUiNeoParameters->MaxMemory_1->setDisabled(true);
      cUiNeoParameters->MaxMemory_2->setDisabled(true);
      cUiNeoParameters->MaxMemory_3->setDisabled(true);
      cUiNeoParameters->LowUsageThreshold_0->setDisabled(true);
      cUiNeoParameters->LowUsageThreshold_1->setDisabled(true);
      cUiNeoParameters->LowUsageThreshold_2->setDisabled(true);
      cUiNeoParameters->LowUsageThreshold_3->setDisabled(true);
   }

   if ( cRegionCount == 4 )
   {
      cUiNeoParameters->MaxMemory_0->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:100 );
      cUiNeoParameters->MaxMemory_1->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:200 );
      cUiNeoParameters->MaxMemory_2->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:400 );
      cUiNeoParameters->MaxMemory_3->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:1000 );
   }
   if ( cRegionCount == 3 )
   {
      cUiNeoParameters->MaxMemory_0->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:100 );
      cUiNeoParameters->MaxMemory_1->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:200 );
      cUiNeoParameters->MaxMemory_2->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:1000 );
   }
   if ( cRegionCount == 2 )
   {
      cUiNeoParameters->MaxMemory_0->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:100 );
      cUiNeoParameters->MaxMemory_1->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:1000 );
   }
}
void NeoParameters::SetDefaultsForRegionCount( bool pDummy )
{
   SetDefaultsForRegionCount( cUiNeoParameters->RegionCount->value() );
}

void NeoParameters::SetDefaultsForRegionCount( int pRegionCount )
{
   
   cChanged = true;
//   QObject::disconnect(cUiNeoParameters->Revert, SIGNAL(clicked()), (const QObject *) this, SLOT(ParamsRevert()) );
   cUiNeoParameters->Revert->setEnabled(true);
//   QObject::connect(cUiNeoParameters->Revert, SIGNAL(clicked()), (const QObject *) this, SLOT(ParamsRevert()) );
   
   cRegionCount = pRegionCount;
   
   cRegionSideCompression.clear();
   for ( unsigned i=0; i< cRegionCount; i++ )
   {
      cRegionSideCompression.push_back(0);
   }

   cRegionSide.clear();
   for ( unsigned i=0; i< cRegionCount; i++ )
   {
      cRegionSide.push_back(0);
   }

   // In case called to set the Region count on the spinner
   // If the spinner is already set it will not matter.
   cUiNeoParameters->RegionCount->setValue(cRegionCount);
   
   // see if it's a new region count.  If so we will need to set the default values later.
//   bool lNewRegionSetting = true;
//   lNewRegionSetting = cRegionCount != lRegionCount;
   

   QLabel * lSide[4];
   QLabel * lLabSide[4];
   QLabel * lLabMaxMemory[4];
   QSpinBox * lLowUsageThreshold[4]; //[0] = cUiNeoParameters->LowUsageThreshold;
   QDoubleSpinBox * lForgetThreshold[4];
   QSpinBox * lSideCompression[4];
   QSpinBox * lMemoryConstraint[4];

   // Set pointers to controls
   lForgetThreshold[0] =  cUiNeoParameters->ForgetThreshold_0;
   lForgetThreshold[1] =  cUiNeoParameters->ForgetThreshold_1;
   lForgetThreshold[2] =  cUiNeoParameters->ForgetThreshold_2;
   lForgetThreshold[3] =  cUiNeoParameters->ForgetThreshold_3;
   lSideCompression[0] =  cUiNeoParameters->SideCompression_0;
   lSideCompression[1] =  cUiNeoParameters->SideCompression_1;
   lSideCompression[2] =  cUiNeoParameters->SideCompression_2;
   lSideCompression[3] =  cUiNeoParameters->SideCompression_3;
   lLabSide[0]         =  cUiNeoParameters->LabSide_0;
   lLabSide[1]         =  cUiNeoParameters->LabSide_1;
   lLabSide[2]         =  cUiNeoParameters->LabSide_2;
   lLabSide[3]         =  cUiNeoParameters->LabSide_3;
   lSide[0]            =  cUiNeoParameters->Side_0; 
   lSide[1]            =  cUiNeoParameters->Side_1; 
   lSide[2]            =  cUiNeoParameters->Side_2; 
   lSide[3]            =  cUiNeoParameters->Side_3; 
   lMemoryConstraint[0] = cUiNeoParameters->MaxMemory_0;
   lMemoryConstraint[1] = cUiNeoParameters->MaxMemory_1;
   lMemoryConstraint[2] = cUiNeoParameters->MaxMemory_2;
   lMemoryConstraint[3] = cUiNeoParameters->MaxMemory_3;
   lLabMaxMemory[0] = cUiNeoParameters->LabMaxMemory_0;
   lLabMaxMemory[1] = cUiNeoParameters->LabMaxMemory_1;
   lLabMaxMemory[2] = cUiNeoParameters->LabMaxMemory_2;
   lLabMaxMemory[3] = cUiNeoParameters->LabMaxMemory_3;
   lLowUsageThreshold[0] = cUiNeoParameters->LowUsageThreshold_0;
   lLowUsageThreshold[1] = cUiNeoParameters->LowUsageThreshold_1;
   lLowUsageThreshold[2] = cUiNeoParameters->LowUsageThreshold_2;
   lLowUsageThreshold[3] = cUiNeoParameters->LowUsageThreshold_3;

   for ( int i=0; i< 4; i++ )
   {
      lForgetThreshold[i]->setValue ( 0.0 );
      lSideCompression[i]->setValue( 0 );
      if ( i > cRegionCount-1 )
      {
         lForgetThreshold[i]->hide();
         lSideCompression[i]->hide();
         lLabSide[i]->setVisible( false );
         lLabMaxMemory[i]->hide();
         lSide[i]->setVisible( false );
         lMemoryConstraint[i]->setVisible( false );
         lLowUsageThreshold[i]->setVisible( false );
      }
      else
      {
         lForgetThreshold[i]->show();
         lSideCompression[i]->show();
         lLabSide[i]->setVisible( true );
         lLabMaxMemory[i]->show();
         lSide[i]->setVisible( true );
         lMemoryConstraint[i]->setVisible( true );
         lLowUsageThreshold[i]->setVisible( true );
      }
   }

   if ( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Checked )
   {
      cUiNeoParameters->ForgetThresholdLegend->setText( QString("Delete a Percentage of memories"));
   }
   else
   {
      cUiNeoParameters->ForgetThresholdLegend->setText( QString("Delete memories below threshold"));
   }
   if ( cRegionCount == 4 )
   {
      cUiNeoParameters->ForgetThreshold_0->setValue( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Unchecked ? 0.07:40.00 );
      cUiNeoParameters->ForgetThreshold_1->setValue( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Unchecked ? 3.5:98.00 );
      cUiNeoParameters->ForgetThreshold_2->setValue( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Unchecked ? 0.5:95.00 );
      cUiNeoParameters->ForgetThreshold_3->setValue( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Unchecked ? 2.0:95.00 );
      cUiNeoParameters->SideCompression_0->setValue( 4 );
      cUiNeoParameters->SideCompression_1->setValue( 2 );
      cUiNeoParameters->SideCompression_2->setValue( 2 );
      cUiNeoParameters->SideCompression_3->setValue( 2 );
      cUiNeoParameters->MaxMemory_0->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:100 );
      cUiNeoParameters->MaxMemory_1->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:200 );
      cUiNeoParameters->MaxMemory_2->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:400 );
      cUiNeoParameters->MaxMemory_3->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:1000 );
   }

   // if ( lNewRegionSetting && cRegionCount == 3 )
   if ( cRegionCount == 3 )
   {
      cUiNeoParameters->ForgetThreshold_0->setValue( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Unchecked ? 0.07:40.00 );
      cUiNeoParameters->ForgetThreshold_1->setValue( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Unchecked ? 3.5:98.00 );
      cUiNeoParameters->ForgetThreshold_2->setValue( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Unchecked ? 0.25:90.00 );
      cUiNeoParameters->SideCompression_0->setValue( 4 );
      cUiNeoParameters->SideCompression_1->setValue( 2 );
      cUiNeoParameters->SideCompression_2->setValue( 2 );
      cUiNeoParameters->MaxMemory_0->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:100 );
      cUiNeoParameters->MaxMemory_1->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:200 );
      cUiNeoParameters->MaxMemory_2->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:1000 );
   }
   // if ( lNewRegionSetting && cRegionCount == 2 )
   if ( cRegionCount == 2 )
   {
      cUiNeoParameters->ForgetThreshold_0->setValue( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Unchecked ? 0.1:40.00 );
      cUiNeoParameters->ForgetThreshold_1->setValue( cUiNeoParameters->DeletionByPercentage->checkState() == Qt::Unchecked ? 3.0:95.00 );
      cUiNeoParameters->SideCompression_0->setValue( 4 );
      cUiNeoParameters->SideCompression_1->setValue( 2 );
      cUiNeoParameters->MaxMemory_0->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:100 );
      cUiNeoParameters->MaxMemory_1->setValue( cUiNeoParameters->DoNotForget->checkState() == Qt::Unchecked ? 30000:1000 );
   }

   //   if ( lNewRegionSetting )
   //{
   SetForgetThreshold( 
      cUiNeoParameters->ForgetThreshold_0->value()
      , cUiNeoParameters->ForgetThreshold_1->value()
      , cUiNeoParameters->ForgetThreshold_2->value()
      , cUiNeoParameters->ForgetThreshold_3->value() 
      );

   SetMemorySize(
      cUiNeoParameters->MaxMemory_0->value()
      , cUiNeoParameters->MaxMemory_1->value()
      , cUiNeoParameters->MaxMemory_2->value()
      , cUiNeoParameters->MaxMemory_3->value() );
   //SetMemorySize( 78, 20, 914, 1169 );

   SetRegionLowUsageForgetThreshold(2, 2, 2, 2);


   unsigned int c0 = cUiNeoParameters->SideCompression_0->value();
   unsigned int c1 = cUiNeoParameters->SideCompression_1->value();
   unsigned int c2 = cUiNeoParameters->SideCompression_2->value();
   unsigned int c3 = cUiNeoParameters->SideCompression_3->value();
   SetSideCompression(c0, c1, c2, c3);
   //}

   // Sequence length is always 1 so no need to get from dialog.

   cSequenceLength.clear();
   
   for ( unsigned i=0; i < cRegionCount; i++ )
   {
      cSequenceLength.push_back(1); 
   }
   // cSequenceLength[0] = 3; // crashes during recognition!

};

void NeoParameters::UpdateDataFromSpinners()
{
   cChanged = true;
//   QObject::disconnect(cUiNeoParameters->Revert, SIGNAL(clicked()), (const QObject *) this, SLOT(ParamsRevert()) );
   cUiNeoParameters->Revert->setEnabled(true);
//   QObject::connect(cUiNeoParameters->Revert, SIGNAL(clicked()), (const QObject *) this, SLOT(ParamsRevert()) );

   SetForgetThreshold(
      cUiNeoParameters->ForgetThreshold_0->value()
      , cUiNeoParameters->ForgetThreshold_1->value()
      , cUiNeoParameters->ForgetThreshold_2->value()
      , cUiNeoParameters->ForgetThreshold_3->value() );

   SetMemorySize(
      cUiNeoParameters->MaxMemory_0->value()
      , cUiNeoParameters->MaxMemory_1->value()
      , cUiNeoParameters->MaxMemory_2->value()
      , cUiNeoParameters->MaxMemory_3->value() );

   SetRegionLowUsageForgetThreshold(
      cUiNeoParameters->LowUsageThreshold_0->value()
      , cUiNeoParameters->LowUsageThreshold_1->value()
      , cUiNeoParameters->LowUsageThreshold_2->value()
      , cUiNeoParameters->LowUsageThreshold_3->value() );

   unsigned int c0 = cUiNeoParameters->SideCompression_0->value();
   unsigned int c1 = cUiNeoParameters->SideCompression_1->value();
   unsigned int c2 = cUiNeoParameters->SideCompression_2->value();
   unsigned int c3 = cUiNeoParameters->SideCompression_3->value();
   SetSideCompression(c0, c1, c2, c3);
};

void NeoParameters::UpdateSpinnersFromData()
{
   // Disable events on spinners before updating else th events will fire and mess everything up!
   QObject::disconnect(cUiNeoParameters->RegionCount, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(SetDefaultsForRegionCount(int)));
   cUiNeoParameters->RegionCount->setValue( cRegionCount );
   QObject::connect(cUiNeoParameters->RegionCount, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(SetDefaultsForRegionCount(int)));

// Disconnect 0 and 1
   QObject::disconnect(cUiNeoParameters->ForgetThreshold_0, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->ForgetThreshold_1, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));

   QObject::disconnect(cUiNeoParameters->MaxMemory_0, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->MaxMemory_1, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
 
   QObject::disconnect(cUiNeoParameters->SideCompression_0, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->SideCompression_1, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
//

   cUiNeoParameters->ForgetThreshold_0->setValue(cRegionForgetThreshold[0]);
   cUiNeoParameters->SideCompression_0->setValue(cRegionSideCompression[0]);
   cUiNeoParameters->Side_0->setNum((double)cRegionSide[0]);
   cUiNeoParameters->MaxMemory_0->setValue(cRegionMemorySize[0]);

   cUiNeoParameters->ForgetThreshold_1->setValue(cRegionForgetThreshold[1]);
   cUiNeoParameters->SideCompression_1->setValue(cRegionSideCompression[1]);
   cUiNeoParameters->Side_1->setNum((double)cRegionSide[1]);
   cUiNeoParameters->MaxMemory_1->setValue(cRegionMemorySize[1]);

   //reconnect
   QObject::connect(cUiNeoParameters->ForgetThreshold_0, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->ForgetThreshold_1, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));

   QObject::connect(cUiNeoParameters->MaxMemory_0, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->MaxMemory_1, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));

   QObject::connect(cUiNeoParameters->SideCompression_0, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->SideCompression_1, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));      

   if ( cRegionCount == 2 )
   {
      return;
   }

   // Disconnect 2
   QObject::disconnect(cUiNeoParameters->ForgetThreshold_2, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->MaxMemory_2, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->SideCompression_2, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   //
   cUiNeoParameters->ForgetThreshold_2->setValue(cRegionForgetThreshold[2]);
   cUiNeoParameters->SideCompression_2->setValue(cRegionSideCompression[2]);
   cUiNeoParameters->Side_2->setNum((double)cRegionSide[2]);
   cUiNeoParameters->MaxMemory_2->setValue(cRegionMemorySize[2]);

   // Reconnect
   QObject::connect(cUiNeoParameters->ForgetThreshold_2, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->MaxMemory_2, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->SideCompression_2, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   if ( cRegionCount == 3 )
   {
      return;
   }

   // Disconnect 3
   QObject::disconnect(cUiNeoParameters->ForgetThreshold_3, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->MaxMemory_3, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->SideCompression_3, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   //
   cUiNeoParameters->ForgetThreshold_3->setValue(cRegionForgetThreshold[3]);
   cUiNeoParameters->SideCompression_3->setValue(cRegionSideCompression[3]);
   cUiNeoParameters->Side_3->setNum((double)cRegionSide[3]);
   cUiNeoParameters->MaxMemory_3->setValue(cRegionMemorySize[3]);

   // Reconnect
   QObject::connect(cUiNeoParameters->ForgetThreshold_3, SIGNAL(valueChanged(double)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->MaxMemory_3, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->SideCompression_3, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
};



void NeoParameters::SetSideCompression( unsigned c0, unsigned c1, unsigned c2, unsigned c3 )
{
   // This currently always returns true
   if ( !SetValidCompression(c0, c1, c2, c3) )
   {
      //if ( Qt::Checked != cUiNeoParameters->IgnoreErrors->checkState() )
      QMessageBox::information( NULL, "Neocortex", "Warning.  Invalid region side detected",  QMessageBox::Ok );
   }

   switch ( cRegionCount )
   {
   case 1:
      cRegionSideCompression[0] = c0;
      break;
   case 2:
      cRegionSideCompression[0] = c0;
      cRegionSideCompression[1] = c1;
      break;
   case 3:
      cRegionSideCompression[0] = c0;
      cRegionSideCompression[1] = c1;
      cRegionSideCompression[2] = c2;
      break;
   case 4:
      cRegionSideCompression[0] = c0;
      cRegionSideCompression[1] = c1;
      cRegionSideCompression[2] = c2;
      cRegionSideCompression[3] = c3;
      break;
   }
   DisplaySideSize();
}

// &c0 - &c3 are the values the user has attempted to set in the spin boxes
// These values can be modified to make the region sizes integral numbers
bool NeoParameters::SetValidCompression(unsigned &c0, unsigned &c1, unsigned &c2, unsigned &c3)
{
// Disable spinner signals
   QObject::disconnect(cUiNeoParameters->SideCompression_0, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->SideCompression_1, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->SideCompression_2, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::disconnect(cUiNeoParameters->SideCompression_3, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));

   double lSide = 0;
   double lIntPart;
   double lTest;
   unsigned lSide0 = 0;
   unsigned lSide1 = 0;
   unsigned lSide2 = 0;
   unsigned lSide3 = 0;

   QSpinBox * lSpinBoxSideCompression[4];
   lSpinBoxSideCompression[0] =  cUiNeoParameters->SideCompression_0;
   lSpinBoxSideCompression[1] =  cUiNeoParameters->SideCompression_1;
   lSpinBoxSideCompression[2] =  cUiNeoParameters->SideCompression_2;
   lSpinBoxSideCompression[3] =  cUiNeoParameters->SideCompression_3;

   bool lRet = true;

   unsigned i;
   for( i = 0; i < cRegionCount; i++ )
   {
      switch ( i )
      {
      case 0:
         {
            // if c0 has become larger than the side size one level down (the actual image) we must treat this
            //    as the case where we search from zero to the minimum value that gives an integral
            //    side size at this level.
            if ( ImageSide < c0 )
            {
               c0=ImageSide;
            }            
            
            lSide = ( double(ImageSide - cEyeOverlap) / double(c0 - cEyeOverlap) );
            lTest = std::modf(lSide, &lIntPart);
            if ( lTest > 0 && c0 > cRegionSideCompression[0] ) //going up
            {
               while ( c0 < ImageSide )  // i.e. cannot be greater that the size of the image
               {
                  c0++;
                  lSide = ( double(ImageSide - cEyeOverlap) / double(c0 - cEyeOverlap) );
                  lTest = std::modf(lSide, &lIntPart);
                  if ( lTest == double(0) )
                  {
                     // set spinner to next correct value
                     cRegionSideCompression[0] = c0;
                     lSpinBoxSideCompression[0]->setValue( double(c0) );
                     break;
                  }
               }
            }
            else 
            {
               if ( lTest > 0 && cRegionSideCompression[0] >c0 ) //going down
               {
                  while ( c0 > 1 )
                  {
                     c0--;
                     lSide = ( double(ImageSide - cEyeOverlap) / double(c0 - cEyeOverlap) );
                     lTest = std::modf(lSide, &lIntPart);
                     if ( lTest == double(0) )
                     {
                        // set spinner to next correct value
                        cRegionSideCompression[0] = c0;
                        lSpinBoxSideCompression[0]->setValue( double(c0) );
                        break;
                     }
                  }
               }
               else
               {
                  // Setting was OK
                  cRegionSideCompression[0] = c0;
                  lSpinBoxSideCompression[0]->setValue( double(c0) );
               }
            }

            lSide0 = lSide;
            cRegionSide[i] = lSide;
            break;
         }
      case 1:
         {
            AdjustRegionCompression(1, lSide, lSide0, c1, lSpinBoxSideCompression);
            lSide1 = lSide;
            cRegionSide[i] = lSide;
            break;
         }
      case 2:
         {
            AdjustRegionCompression(2, lSide, lSide1, c2, lSpinBoxSideCompression);
            lSide2 = lSide;
            cRegionSide[i] = lSide;
            break;
         }
      case 3:
         {
            AdjustRegionCompression(3, lSide, lSide2, c3, lSpinBoxSideCompression);
            lSide3 = lSide;
            cRegionSide[i] = lSide;
            break;
         }
      default:
         break;
      }

      cUiNeoParameters->SideCompression_Hippo->setNum( (int)cRegionSide[cRegionCount-1] );
   }
   // Re-Enable spinner signals
   QObject::connect(cUiNeoParameters->SideCompression_0, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->SideCompression_1, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->SideCompression_2, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   QObject::connect(cUiNeoParameters->SideCompression_3, SIGNAL(valueChanged(int)), (const QObject *) this, SLOT(UpdateDataFromSpinners()));
   return lRet; 
}

// pRegion is the current region number
// &pSide returns the current adjusted image side
// pSide0 is the size of the next region down (towards the input)
// &pCompression is the value the user has attempted to set in the current QSpinBox and returns the modified value
// QSpinBox** enables us to set the spinners.  Note that the QSpinBox events have been disabled.
void NeoParameters::AdjustRegionCompression( int pRegion, double &pSide, unsigned pSide0, unsigned &pCompression, QSpinBox** pSpinBoxSideCompression )
{
   double lIntPart;
   double lTest;
   pSide = pSide0;
   // if pCompression has become larger than the side size one level down we must treat this
   //    as the case where we search from zero to the minimum value that gives and integral
   //    side size at this level.
   if ( pSide0 < pCompression )
   {
      pCompression=pSide0;
   }
   pSide /= double(pCompression);
   lTest = std::modf(pSide, &lIntPart);
   if ( lTest > 0 && cRegionSideCompression[pRegion] < pCompression ) // going up
   {
      while ( pCompression < pSide0 ) // i.e. cannot be greater than the size of the next level down
      {
         pSide = pSide0;
         pCompression++;
         pSide/= double(pCompression); 
         lTest = std::modf(pSide, &lIntPart);
         if ( lTest == double(0) )
         {
            // set spinner to next correct value
            cRegionSideCompression[pRegion] = pCompression;
            pSpinBoxSideCompression[pRegion]->setValue( double(pCompression) );
            break;
         }
      }
   }
   else
   {
      if ( lTest > 0 && cRegionSideCompression[pRegion] > pCompression ) // going down
      {
         while ( pCompression > 1 ) // work downwards until we find a whole number side size
         {
            pSide = pSide0;
            pCompression--;
            pSide/= double(pCompression); 
            lTest = std::modf(pSide, &lIntPart);
            if ( lTest == double(0) )
            {
               // set spinner to next correct value
               cRegionSideCompression[pRegion] = pCompression;
               pSpinBoxSideCompression[pRegion]->setValue( double(pCompression) );
               break;
            }
         }
      }

      else
      {
         if ( lTest > 0 && cRegionSideCompression[pRegion] == pCompression ) 
            // staying the same but wrong anyway!
         {
            pCompression=0;
            while ( pCompression < pSide0 )// work upwards until we find a whole number side size
            {
               pSide = pSide0;
               pCompression++;
               pSide/= double(pCompression); 
               lTest = std::modf(pSide, &lIntPart);
               if ( lTest == double(0) )
               {
                  // set spinner to next correct value
                  cRegionSideCompression[pRegion] = pCompression;
                  pSpinBoxSideCompression[pRegion]->setValue( double(pCompression) );
                  break;
               }
            }
         }
         else
         {
            // Setting was OK
            cRegionSideCompression[pRegion] = pCompression;
            pSpinBoxSideCompression[pRegion]->setValue( double(pCompression) );
         }
      }
   }
}

void NeoParameters::SetForgetThreshold ( float f0, float f1, float f2, float f3 )
{
   cRegionForgetThreshold.clear();

   for ( int i=0; i < cRegionCount; i++)
   {
      cRegionForgetThreshold.push_back(0.0);
   }

   cRegionForgetThreshold[0] = f0;
   if ( cRegionCount == 1 ) 
      return;

   cRegionForgetThreshold[1] = f1;
   if ( cRegionCount == 2 ) 
      return;

   cRegionForgetThreshold[2] = f2;
   if ( cRegionCount == 3 ) 
      return;

   // Region count == 4
   cRegionForgetThreshold[3] = f3;
}

unsigned NeoParameters::GetRegionLowUsageThreshold(unsigned i) 
{ 
   return cRegionLowUsageThreshold[i]; 
};

unsigned NeoParameters::GetRegionMemorySize(unsigned i) 
{ 
   return cRegionMemorySize[i]; 
};

void NeoParameters::SetMemorySize ( unsigned c0, unsigned c1, unsigned c2, unsigned c3  )
{
   cRegionMemorySize.clear();
   for ( int i=0; i< cRegionCount; i++ )
   {
      cRegionMemorySize.push_back(0);
   }
   cRegionMemorySize[0] = c0;
   if ( cRegionCount == 1 ) 
      return;

   cRegionMemorySize[1] = c1;
   if ( cRegionCount == 2 ) 
      return;

   cRegionMemorySize[2] = c2;
   if ( cRegionCount == 3 ) 
      return;

   // Region count == 4
   cRegionMemorySize[3] = c3;
}

void NeoParameters::SetRegionLowUsageForgetThreshold(unsigned c0, unsigned c1, unsigned c2, unsigned c3) 
{ 
   cRegionLowUsageThreshold.clear();
   for ( int i=0; i< cRegionCount; i++ )
   {
      cRegionLowUsageThreshold.push_back(0);
   }
   cRegionLowUsageThreshold[0] = c0;
   if ( cRegionCount == 1 ) 
      return;

   cRegionLowUsageThreshold[1] = c1;
   if ( cRegionCount == 2 ) 
      return;

   cRegionLowUsageThreshold[2] = c2;
   if ( cRegionCount == 3 ) 
      return;

   // Region count == 4
   cRegionLowUsageThreshold[3] = c3;
};


void NeoParameters::DisplaySideSize()
{
   double lSide = 0;  // Crashes Release build if trying to set a spinner with this as int.
   // Failure on memory allocation.

   //derived: whole number of overlapping subregions that fit into ImageSide
   lSide =
      ( double(ImageSide - cEyeOverlap) / double(cRegionSideCompression[0] - cEyeOverlap) );

   cBottomRegionSide = lSide;

   //image side covered by hierarchy. May not cover whole bitmap due to overlap
   cCoveredImageSide =
      cBottomRegionSide * (cRegionSideCompression[0] - cEyeOverlap) + cEyeOverlap;

   cUiNeoParameters->Side_0->setNum(0);
   cUiNeoParameters->Side_1->setNum(0);
   cUiNeoParameters->Side_2->setNum(0);
   cUiNeoParameters->Side_3->setNum(0);

   unsigned i;
   for(i = 0; i < cRegionCount; i++)
   {
      //if(i == 0)
      // lSide is already set and if it is fractional we need to know about it!
      //lSide = double(GetBottomRegionSide());
      if ( i > 0 )
         lSide /= double(GetRegionSideCompression(i));
      switch (i)
      {
      case 0:
         cUiNeoParameters->Side_0->setNum(lSide);
         break;
      case 1:
         cUiNeoParameters->Side_1->setNum(lSide);
         break;
      case 2:
         cUiNeoParameters->Side_2->setNum(lSide);
         break;
      case 3:
         cUiNeoParameters->Side_3->setNum(lSide);
         break;
      default:
         break;
      }
   }
}
