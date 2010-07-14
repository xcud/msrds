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
#ifndef ISACCADE
#define ISACCADE

#include <vector>
#include <Qt/QObject.h>
typedef std::vector< std::vector<unsigned> > BmpArray;

class ISaccade : public QObject
{
public:
   Q_OBJECT
signals:
   // To implement the IVision interface.
   // delivers vision information to the client by callback thought IVision
   void VisionService( BmpArray *, int );
   //void VisionService2( BmpArray *, int );
   public slots:
public:
   //ISaccade(void);
   virtual ~ISaccade(void){};
   virtual void Initialize( BmpArray * pImage, int pImageSide, int pCoveredImageSide ) = 0;
   virtual BmpArray * Run( int pRunFor , int & pSaccadesCompleted, int pSaccadesRequested=-1 ) = 0;
};

#endif

