/*
This code is based on example source code for the book
C++ GUI Programming with Qt 4 by
Jasmin Blanchette and Mark Summerfield, ISBN 0-13-187249-4.

iconeditor
Copyright ?2004 Trolltech?AS. All Rights Reserved.
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

#ifndef ICONEDITOR_H
#define ICONEDITOR_H

#include <Qt/QColor.h>
#include <Qt/QImage.h>
#include <Qt/QWidget.h>

class IconEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)
    Q_PROPERTY(QImage iconImage READ iconImage WRITE setIconImage)
    Q_PROPERTY(int zoomFactor READ zoomFactor WRITE setZoomFactor)

public:
    IconEditor(QWidget *parent = 0, int pWidth=32, int pHeight=32, int pZoom=8 );

    void setPenColor(const QColor &newColor);
    QColor penColor() const { return curColor; }
    void setZoomFactor(int newZoom);
    int zoomFactor() const { return zoom; }
    void setIconImage(const QImage &newImage);
    QImage & getIconImage();
    QImage iconImage() const { return cImage; }
    QSize sizeHint() const;
    void scale(double,double);
    void rotate90();
    void moveXY(int pX, int pY);
    void AddNoise( float pNoiseFactor, bool pInvertPixels=true, unsigned pBorderX=0, unsigned pBorderY=0 );
    void SetBottomRight8X8(unsigned pX, unsigned pY)
    {
       if ( pX < 8 && pY < 8 )
       {
         setImagePixel(QPoint(pX,pY), true);
       }
    }

    void ClearBottomRight( unsigned pX=16, unsigned pY=16 )
    {
      for ( int i=0; i< pX; i++ )
         for ( int j=0; j< pY; j++ )
            setImagePixel(QPoint(i,j), false );
    }

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void setImagePixel(const QPoint &pos, bool opaque);
    void toggleImagePixel(const QPoint &pos);
    QRect pixelRect(int i, int j) const;

    QColor curColor;
    QImage cImage;
    int zoom;
};

#endif
