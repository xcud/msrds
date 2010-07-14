/*
This code is based on example source code for the book
C++ GUI Programming with Qt 4 by
Jasmin Blanchette and Mark Summerfield, ISBN 0-13-187249-4.

iconeditor
Copyright ?2004 Trolltech?AS. All Rights Reserved.

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


//#include <QtGui>
#include <Qt/QPainter.h>
#include <Qt/qevent.h>
#include <Qt/QMatrix.h>
#include <Qt/QTransform.h>
#include <Qt/QRect.h>
#include "../CImg-1.2.5/CImg.h"

#include "../iconeditor/iconeditor.h"

IconEditor::IconEditor(QWidget *parent, int pWidth, int pHeight, int pZoom )
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    curColor = Qt::black;
    zoom = pZoom;

    cImage = QImage(pWidth, pHeight, QImage::Format_ARGB32);
    cImage.fill(qRgba(0, 0, 0, 255));
}

void IconEditor::setPenColor(const QColor &newColor)
{
    curColor = newColor;
}

void IconEditor::setZoomFactor(int newZoom)
{
    if (newZoom < 1)
        newZoom = 1;

    if (newZoom != zoom) {
        zoom = newZoom;
        update();
        updateGeometry();
    }
}

void IconEditor::setIconImage(const QImage &newImage)
{
    if (newImage != cImage) {
        cImage = newImage.convertToFormat(QImage::Format_ARGB32);
        update();
        updateGeometry();
    }
}

QImage & IconEditor::getIconImage()
{
   return cImage;
}

// Adds a noise factor to the extent of the cImage.
// By confining noise to the actual content we prevent any reduction is saccade movement.
// Thus the recognition will only be affected by the disruption of the picture.
void IconEditor::AddNoise( float pNoiseFactor, bool pInvertPixels/*=true*/, unsigned pBorderX/*=0*/, unsigned pBorderY/*=0 */ )
{
   if ( ! ( pNoiseFactor > 0 ))
      return;

   int lWidth = cImage.size().width();
   int lHeight = cImage.size().height();

   // Populate a QPolygon to enable us to get the cImage boundaries
   QPolygon lImagePolygon; 
   QColor lBkgColour(255,255,255);
   for ( int i=0; i< cImage.size().width(); i++ )
   {
      for ( int j=0; j< cImage.size().height(); j++ )
      {
         if (cImage.pixel(i,j) == penColor().rgba())
         {
            lImagePolygon <<QPoint(i,j);
         }
      }
   }


   QRect lObjectBoundingRect = lImagePolygon.boundingRect();

   int lObjectWidth = lObjectBoundingRect.width()-pBorderX*2;
   int lObjectHeight = lObjectBoundingRect.height()-pBorderY*2;
   int lObjectLeft = lObjectBoundingRect.left() +pBorderX;
   int lObjectTop = lObjectBoundingRect.top() +pBorderY;

   // Not used at present
   // int lGridHeight = cImage.height();
   // int lGridWidth = cImage.width();
   // Scale the noise factor to % cImage content area
   // double lGridArea = lGridHeight * lGridWidth;
   // double lObjectArea = lObjectWidth * lObjectHeight;
   
   double lNoiseFactor = pNoiseFactor; // * lGridArea / lObjectArea;

   for ( int i=0; i<lObjectWidth; i++ )
   {
      for ( int j=0; j< lObjectHeight; j++ )
      {
         // All we need to do is set the current pixel some percentage of the time
         if ( (double(rand()) / double(RAND_MAX)) < lNoiseFactor )
         {
            int c = i + lObjectLeft;
            int r = j + lObjectTop;
            if ( pInvertPixels )
            {
               toggleImagePixel(QPoint(c*zoom, r*zoom));
            }
            else
            {
               setImagePixel(QPoint(c*zoom, r*zoom), true);
            }
         }
      }
   }
   return;
}


QSize IconEditor::sizeHint() const
{
    QSize size = zoom * cImage.size();
    if (zoom >= 3)
        size += QSize(1, 1);
    return size;
}

void IconEditor::rotate90()
{
   // This function uses CImg template library.
   // There were problems with the Qt equivalent.
   using namespace cimg_library;

   // Populate a QPolygon for translations and measurements
   QPolygon lStartingPolygon; 
   int lnPoints = 0;
   QColor lBkgColour(255,255,255);
   for ( int i=0; i< cImage.size().width(); i++ )
   {
      for ( int j=0; j< cImage.size().height(); j++ )
      {
         if (cImage.pixel(j,i) == penColor().rgba())
         {
            lStartingPolygon <<QPoint(i,j);
            setImagePixel(QPoint(j*zoom, i*zoom), false); // clear the pixel to transparent - we re-display it later
            // Note the zoom factor - this is the scaling that is applied by setImagePixel
            // to map from mouse coordinates to cImage coordinates
            lnPoints++;
         }
      }
   }

   // This lets us get the top left of the actual cImage so we can restore the centre of the cImage to the correct position
   QRect lStartingRect = lStartingPolygon.boundingRect();
   QPoint lCenterStart = lStartingRect.center();

   // CImage rotate replaces Qt one
   const unsigned char white[] = { 255 }, black[] = { 0 };
   CImg<unsigned char> lImg(cImage.size().height(),cImage.size().width(),1,3);
   lImg.fill(0xFF);
   // Convert to CImg for cImage manipulation
   lnPoints = lStartingPolygon.size();
   for ( int z=0; z<lnPoints; z++ )
   {
      int i,j;
      lStartingPolygon.point( z, &j, &i );
      if ( (i < cImage.size().width()) && (j < cImage.size().height()) )
      {
         lImg.draw_point(i,j, black);
      }
   }

   CImg<unsigned char> lImg2(cImage.size().height(),cImage.size().width(),1,3);
   lImg2.fill(0xFF);
   int i11 = lImg.dimx();;
   int i12 = lImg.dimy();;
   int rr1 = lStartingRect.x();
   int rr2 = lStartingRect.y();

   //lImg.display();
   lImg2 = lImg.get_rotate(90.0); //lObjectRect.x(),lObjectRect.y());
   //lImg2.display();

   int i21 = lImg2.dimx();;
   int i22 = lImg2.dimy();;
   // Now get the cImage back from CImg
   QPolygon lIntermediatePolygon; //(ImageSide*ImageSide);
   lnPoints =0;
   for ( int i=0; i< lImg2.dimx(); i++ )
   {
      for ( int j=0; j< lImg2.dimy(); j++ )
      {
         if ( lImg2.pix2d(i,j) != *white ) // interpolations use colour!
         {
            if ( j< cImage.size().width() && i< cImage.size().height() )
            {
               lIntermediatePolygon << QPoint(j,i);
               lnPoints++;
            }
         }
      }
   }

   QPoint lCentre2 = lIntermediatePolygon.boundingRect().center();
   QMatrix lTranslate;
   lTranslate.translate(lCenterStart.x()-lCentre2.x(), lCenterStart.y() - lCentre2.y() );
   QPolygon lOutPolygon = lTranslate.map(lIntermediatePolygon);

   // Convert back to BitMap
   // cTestPixArray is now empty - it was cleared previously.
   lnPoints = lOutPolygon.size();
   // Now convert back to cImage
   lnPoints = lOutPolygon.size();
   for ( int z=0; z<lnPoints; z++ )
   {
      int i,j;
      lOutPolygon.point( z, &j, &i );
      if ( (i < cImage.size().width()) && (j < cImage.size().height()) )
      {
         setImagePixel(QPoint(i*zoom, j*zoom), true); //opaque
      }
   }
}
   //cUtils->ShowTestImage(cTestPixArray);

/* THIS DOES NOT WORK. TRANSLATING TO CENTRE WORKS BUT ROTATION SENDS IT WILD.
QPolygon lStartingPolygon; 
int lPoints = 0;
QColor lBkgColour(255,255,255);
for ( int i=0; i< cImage.size().width(); i++ )
{
for ( int j=0; j< cImage.size().height(); j++ )
{
if (cImage.pixel(j,i) == penColor().rgba())
{
lStartingPolygon <<QPoint(i,j);
setImagePixel(QPoint(j*zoom, i*zoom), false); // clear the pixel to transparent - we re-display it later
// Note the zoom factor - this is the scaling that is applied by setImagePixel
// to map from mouse coordinates to cImage coordinates
lPoints++;
}
}
}

// This lets us get the centre of the actual cImage so we can restore the centre of the cImage to the correct position
QRect lStartingRect = lStartingPolygon.boundingRect();
QPoint lCentreStart = lStartingRect.center();

// Move to centre of cImage area
QMatrix lTranslate;
lTranslate.translate(qreal(16)-qreal(lCentreStart.x()), qreal(16)-qreal(lCentreStart.y()) );
lStartingPolygon = lTranslate.map(lStartingPolygon);

// Now do the transformation on the polygon
QMatrix lTransform; 
lTransform.rotate(qreal(90));
QPolygon lXformedPolygon = lTransform.map(lStartingPolygon);

QRect lTransformedRect = lXformedPolygon.boundingRect();
QPoint lCentreEnd = lTransformedRect.center();

//lTranslate.translate( lCentreEnd.x(), lCentreEnd.y() );
//lXformedPolygon = lTranslate.map(lXformedPolygon);

// Now convert back to cImage

lPoints = lXformedPolygon.size();
for ( int z=0; z<lPoints; z++ )
{
int i,j;
lXformedPolygon.point( z, &j, &i );
if ( (i < cImage.size().width()) && (j < cImage.size().height()) )
{
setImagePixel(QPoint(i*zoom, j*zoom), true); //opaque
}
}
*/


void IconEditor::scale(double dx, double dy)
{
   QPolygon lStartingPolygon; 
   int lPoints = 0;
   QColor lBkgColour(255,255,255);
   for ( int i=0; i< cImage.size().width(); i++ )
   {
      for ( int j=0; j< cImage.size().height(); j++ )
      {
         if (cImage.pixel(j,i) == penColor().rgba())
         {
            lStartingPolygon <<QPoint(i,j);
            setImagePixel(QPoint(j*zoom, i*zoom), false); // clear the pixel to transparent - we re-display it later
            // Note the zoom factor - this is the scaling that is applied by setImagePixel
            // to map from mouse coordinates to cImage coordinates
            lPoints++;
         }
      }
   }

   // This lets us get the top left of the actual cImage so we can restore the centre of the cImage to the correct position
   QRect lStartingRect = lStartingPolygon.boundingRect();

   // Now do the transformations on the polygon
   QMatrix lTransform; 
   lTransform.scale((qreal)dx, (qreal)dy);

   QPolygon lXformedPolygon = lTransform.map(lStartingPolygon);

   QMatrix lTranslate;
   lTranslate.translate(qreal(lStartingRect.top()), qreal(lStartingRect.left()) );

   QRect lTransformedRect = lXformedPolygon.boundingRect();
   lTranslate.translate( -lTransformedRect.top(), -lTransformedRect.left() );

   QPolygon lXlatedPolygon = lTranslate.map(lXformedPolygon);

   // Now convert back to cImage
   lPoints = lXlatedPolygon.size();
   for ( int z=0; z<lPoints; z++ )
   {
      int i,j;
      lXlatedPolygon.point( z, &j, &i );
      if ( (i < cImage.size().width()) && (j < cImage.size().height()) )
      {
         setImagePixel(QPoint(i*zoom, j*zoom), true); //opaque
      }
   }
}

void IconEditor::moveXY( int pX, int pY )
{
   QPolygon lStartingPolygon; 
   int lPoints = 0;
   QColor lBkgColour(255,255,255);
   for ( int i=0; i< cImage.size().width(); i++ )
   {
      for ( int j=0; j< cImage.size().height(); j++ )
      {
         if (cImage.pixel(j,i) == penColor().rgba())
         {
            lStartingPolygon <<QPoint(i,j);
            setImagePixel(QPoint(j*zoom, i*zoom), false); // clear the pixel to transparent - we re-display it later
            // Note the zoom factor - this is the scaling that is applied by setImagePixel
            // to map from mouse coordinates to cImage coordinates
            lPoints++;
         }
      }
   }

   QMatrix lTranslate;
   // y,x ??
   lTranslate.translate(qreal(pY), qreal(pX) );

   QPolygon lXlatedPolygon = lTranslate.map(lStartingPolygon);

   // Now convert back to cImage
   lPoints = lXlatedPolygon.size();
   for ( int z=0; z<lPoints; z++ )
   {
      int i,j;
      lXlatedPolygon.point( z, &j, &i );
      if ( (i < cImage.size().width()) && (j < cImage.size().height()) )
      {
         setImagePixel(QPoint(i*zoom, j*zoom), true); //opaque
      }
   }

}

void IconEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setImagePixel(event->pos(), true);
    } else if (event->button() == Qt::RightButton) {
        setImagePixel(event->pos(), false);
    }
}

void IconEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        setImagePixel(event->pos(), true);
    } else if (event->buttons() & Qt::RightButton) {
        setImagePixel(event->pos(), false);
    }
}

void IconEditor::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (zoom >= 3) {
        painter.setPen(palette().foreground().color());
        for (int i = 0; i <= cImage.width(); ++i)
            painter.drawLine(zoom * i, 0,
                             zoom * i, zoom * cImage.height());
        for (int j = 0; j <= cImage.height(); ++j)
            painter.drawLine(0, zoom * j,
                             zoom * cImage.width(), zoom * j);
    }

    for (int i = 0; i < cImage.width(); ++i) {
        for (int j = 0; j < cImage.height(); ++j) {
            QRect rect = pixelRect(i, j);
            if (!event->region().intersect(rect).isEmpty()) {
                QColor color = QColor::fromRgba(cImage.pixel(i, j));
                painter.fillRect(rect, color);
            }
        }
    }
}

void IconEditor::toggleImagePixel(const QPoint &pos)
{
   int i = pos.x() / zoom;
   int j = pos.y() / zoom;

   QColor lBkgColour(255,255,255);

   if (cImage.rect().contains(i, j)) {
      if (cImage.pixel(i,j) == lBkgColour.rgba()) {
         cImage.setPixel(i, j, penColor().rgba());
      } else {
         cImage.setPixel(i, j, lBkgColour.rgba());
      }

      update(pixelRect(i, j));
   }
}

void IconEditor::setImagePixel(const QPoint &pos, bool opaque)
{
    int i = pos.x() / zoom;
    int j = pos.y() / zoom;

    QColor lBkgColour(255,255,255);

    if (cImage.rect().contains(i, j)) {
        if (opaque) {
            cImage.setPixel(i, j, penColor().rgba());
        } else {
            cImage.setPixel(i, j, lBkgColour.rgba());
        }

        update(pixelRect(i, j));
    }
}

QRect IconEditor::pixelRect(int i, int j) const
{
    if (zoom >= 3) {
        return QRect(zoom * i + 1, zoom * j + 1, zoom - 1, zoom - 1);
    } else {
        return QRect(zoom * i, zoom * j, zoom, zoom);
    }
}
