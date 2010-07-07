/*
This code is based on example source code for the book
C++ GUI Programming with Qt 4 by
Jasmin Blanchette and Mark Summerfield, ISBN 0-13-187249-4.

iconeditor
Copyright © 2004 Trolltech® AS. All Rights Reserved.

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
#include <QtPlugin>

#include "../iconeditor/iconeditor.h"
#include "iconeditorplugin.h"

IconEditorPlugin::IconEditorPlugin(QObject *parent)
    : QObject(parent)
{
}

QString IconEditorPlugin::name() const
{
    return "IconEditor";
}

QString IconEditorPlugin::includeFile() const
{
    return "../iconeditor/iconeditor.h";
}

QString IconEditorPlugin::group() const
{
    return tr("Image Manipulation Widgets");
}

QIcon IconEditorPlugin::icon() const
{
    return QIcon(":/images/iconeditor.png");
}

QString IconEditorPlugin::toolTip() const
{
   return tr("This widget based on code presented in Chapter 5 of <i>C++ GUI "
      "Programming with Qt 4</i> as an example of a custom Qt "
      "widget.");
}

QString IconEditorPlugin::whatsThis() const
{
    return tr("This widget based on code presented in Chapter 5 of <i>C++ GUI "
              "Programming with Qt 4</i> as an example of a custom Qt "
              "widget.");
}

bool IconEditorPlugin::isContainer() const
{
    return false;
}

QWidget *IconEditorPlugin::createWidget(QWidget *parent)
{
    return new IconEditor(parent);
}

Q_EXPORT_PLUGIN2(iconeditorplugin, IconEditorPlugin)
