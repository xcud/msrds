Neocortex Version 1.4.2c
========================

This release note refers to the download release of Neocortex which is a ZIP archive that contains 
source, a Windows XP executable, training data and documentation for the Neocortex implementation 
of a hierarchical memory model of learning by the mammalian neocortex.
The program source is available from Source Forge together with project files for 
Microsoft Visual Studio 2005, MinGW (for Windows) and the general Suse Linux environment.

The information is also relevant if you download the source code for building Neocortex.

CONTENTS:

        CONTENT OF DOWNLOAD
	PREREQUISITES
	INSTALLATION OF VERSION 1.4.2c RUNTIME
	INSTALLATION OF VERSION 1.4.2c FOR DEVELOPMENT
	CHANGES FROM VERSION 1.4
	ISSUES WITH THIS RELEASE

CONTENT OF DOWNLOAD
-------------------

GPLv3.txt
NeoV1.4 User Guide 1_0.pdf (A few new facilities have been added since then - please see below)
ReadMe NeoV1.4.2.Cumulative.txt (This document)

.\Neocortex Documents                     Papers by the original author, Saulius Garalevicius.
   Documents about Previous versions of program
   2005 Memory-Prediction Framework paper.pdf  
   \Documents about Previous versions of program
      2005 12 Using MPF for Pattern Recognition Final Report 2005.pdf
      2005 Analysis and Implementation of MPF 2005.pdf

.\Neocortex Program\                      Program and supporting files
   Microsoft.VC80.CRT.manifest            Latest unreleased experimental versions are 
   msvcp80.dll                                   available at http://zotric.spaces.live.com.
   msvcr80.dll
   Neo.exe                                 

.\Neocortex Source Code\
   GPLv3.txt
   Neo Version 1.4.2 source.zip            Main files for the C++ source code
   Readme.txt

.\Numenta Links (Links to Numenta web site)
   URL for DileepThesis.pdf                (DPhil thesis of Numenta Co-founder Dileep George)  
   URL for Numenta_HTM_Learning_Algos.pdf  (Theory of this type of model)
   URL for Numenta_HTM_Concepts.pdf        (Basic description of the theory)
   URL for ProblemsThatFitHTMs.pdf         (How the technology can be used)

.\Test Objects
   \Test Objects\more
   \Test Objects\some

.\Training Objects
   \Training Objects\More
   \Training Objects\Some



PREREQUISITES 
-------------

Windows XP Service pack 2 
For development: Microsoft Visual Studio 2005 standard edition; Service pack 1
                 or the MinGW 3.4.2 environment with C++ and MinGW32-make.

Suse Linux 10.x
Packages in Linux to compile and link C++.

You will need to download and compile the appropriate Open Source (free) version of Qt from www.trolltech.com.
The following Open Source Qt versions have been tested for compiling and running Neocortex version 1.4.2x

	Qt version 4.4.3 (Windows XP/Studio) 
        Qt version 4.4.3 (windows XP/Studo, statically linked to Qt)
	Qt version 4.3.1 (Suse Linux 10.x)
        Qt version 4.4.3 (Windows XP/Studio) with bundled MinGW from Nokia/Trolltech

The runtime version of Neocortex if compiled with static linking of Qt is expected to run under Windows Vista.
The dynamically linked version has not been tested with Vista.

A statically linked version is supplied with this release.


INSTALLATION OF VERSION 1.4.2c RUNTIME
-------------------------------------

Unzip the archive into a folder of your choice e.g. the Program Files folder.
The program Files folder is usually located at C:\Program Files.

Run the Neo.exe file by changing to the "Program Files\Neocortex\Neocortex Program" folder and double clicking on Neo.exe.
You will be able easily to navigate within the program to the training and test data folders.
Please see the User Guide for details and examples.


INSTALLATION OF VERSION 1.4.2c FOR DEVELOPMENT.
----------------------------------------------

Note:  Minimal source code is provided for information only and to comply with the GPL.
Please obtain latest released source and project files from Source Forge:
http://neocortex.cvs.sourceforge.net/neocortex

In Tortoise CVS you can use the following settings:
CVSROOT                        :ext:<your user name>@neocortex.cvs.sourceforge.net:/cvsroot/neocortex
Protocol can be set to          Secure Shell
Server                          neocortex.cvs.sourceforge.net
Repository folder               /cvsroot/neocortex
Module:                         .

For Linux or in MinGw - copy source files from the MS Studio 2005 directory to the the equivalent folders 
in the Linux or WindowsMinGW directory (MinGw/<projectName> or Linux/<projectName>).  
Also copy the CImg-1.2.5 directory.  Compile the programs as instructed by the readme.txt file in the relevant directory.
Run the program Neo.exe (for MinGw) or Neocortex (for Linux) from the target directory.

For Visual Studio 2005 ensure that $(QTDIR)\include and $(QTDIR)\include\QtDesigner are in the Studio include file path.  
(Use Tools/Options/Projects and Solutions/VC++ Directories and select 'Include files' from the drop down category list.
For Visual Studio - simply set the startup project to Neo 
(this is important - Studio 2005 may crash if you get it wrong and this can cause the PC to become unstable) 
Build the project Neo.
Note that if you change the main window Neo.ui, the icon editor grid does not show at all.  But the control is there.
 
CHANGES FROM VERSION 1.4
------------------------

   Version 1.4.1 
   -------------
1.  Out of range saccades fixed (Originally version 1.4A)

2.  Fixed menus.  These no longer worked in version 1.4.

3.  The current directory was not always retained - fixed.

Framework/SaccadeImpl.cpp; (for version 1.4A)
new revision: 1.4; previous revision: 1.3

Framework/SourceController.cpp;
new revision: 1.5; previous revision: 1.4

Framework/SourceController.h;
new revision: 1.6; previous revision: 1.5

Framework/Thinker.cpp;
new revision: 1.5; previous revision: 1.4

Neo/MainWindow.cpp;
new revision: 1.7; previous revision: 1.6

Neo/MainWindow.h;
new revision: 1.4; previous revision: 1.3

Precompile/About.ui;
new revision: 1.8; previous revision: 1.7

Precompile/Neo.ui;
new revision: 1.5; previous revision: 1.4

iconeditor/iconeditor.vcproj;
new revision: 1.2; previous revision: 1.1

   Version 1.4.2
   -------------

1. Hover help on Parameters window

2. Do not update spinners on checking Deletion Method! 

3. Visualise training saccades 

Framework/NeoParameters.h;
new revision: 1.5; previous revision: 1.4

Framework/Neoparameters.cpp;
new revision: 1.6; previous revision: 1.5

Framework/Thinker.cpp;
new revision: 1.6; previous revision: 1.5

Precompile/Parameters.ui;
new revision: 1.6; previous revision: 1.5

   Version 1.4.2b
   --------------

1. Performance enhancement for recognition.

2. Removed redundant resource.h from Precompile project

3. New build files for MinGW (Please see readme in WindowsMinGW directory).

MPF/Hippo.cpp;
new revision: 1.5; previous revision: 1.4

MPF/SubRegion.cpp;
new revision: 1.2; previous revision: 1.1

Precompile/About.ui;
new revision: 1.10; previous revision: 1.9

Precompile/Precompile.vcproj;
new revision: 1.4; previous revision: 1.3

   Version 1.4.2c
   --------------

1. Fixed control disablement issues during recognition.

2. Brought 'Visualize Training Saccades onto main window.
   This can be toggled during training.

Framework/NeoParameters.h;
new revision: 1.6; previous revision: 1.5

Framework/NeoParameters.cpp;
new revision: 1.7; previous revision: 1.6

Framework/Thinker.cpp;
new revision: 1.7; previous revision: 1.6

MPF/NeoRegion.cpp;
new revision: 1.6; previous revision: 1.5

MPF/NeoRegion.h;
new revision: 1.5; previous revision: 1.4

MPF/SubRegion.cpp;
new revision: 1.3; previous revision: 1.2

Neo/MainWindow.cpp;
new revision: 1.8; previous revision: 1.7

Precompile/About.ui;
new revision: 1.11; previous revision: 1.10

Precompile/Neo.ui;
new revision: 1.6; previous revision: 1.5

Precompile/Parameters.ui;
new revision: 1.8; previous revision: 1.7

Framework/SaccadeImpl.cpp;
new revision: 1.5; previous revision: 1.4




   File change summary (1.4.1, 1.4.2, 1.4.2b, 1.4.2c)
   --------------------------------------------------

SaccadeImpl.cpp	        Framework	Fixed out of range saccades	Bug fix
SourceController.cpp	Framework	Menu event signatures           System - QT ver 4.4.3 events
Thinker.cpp	        Framework	Visualise training saccades	Enhancement
MainWindow.cpp	        Neo	        Fixed Default folders
                                        Menu event signature changed"	System - QT ver 4.4.3 events
NeoParameters.h	        Framework	Visualise training saccades	
NeoParameters.cpp	Framework	Do not update spinners on 
                                        checking Deletion Method!	Bug fix
SourceController.h	Framework	Menu event signature	        System - QT ver 4.4.3 events
MainWindow.h	        Neo	        Menu event signature	        System - QT ver 4.4.3 events
About.ui	        Precompile	Version	
Neo.ui	                Precompile	Menu event signature	        System - QT ver 4.4.3 events
Parameters.ui	        Precompile	Hover shows tooltip help	Enhancement
Precompile.vcproj       Precompile      Removed redundant resource.h    Cosmetic
SubRegion.cpp           MPF             Performance increase            Enhancement
Hippo.cpp               MPF             Performance increase            Enhancement

ISSUES WITH THIS RELEASE

1. The C++ exception handling only currently works for Windows. It is planned to fix this at a future
   release. A #ifdef _WINDOWS is used to ensure that exceptions are only compiled in for Visual Studio 
   (MinGW does not set this variable). Further error checking will be added as required.
   There are just a couple of points where exception handling has been added and these are probably not
   very useful.  But is a model for further work.  Exceptions could not be compiled, as written, with MinGW.

2. The program is not tested with 64 bit compilers. Compiler warnings from Microsoft Visual
   Studio indicate some work to be done for Win 64.

3. The Gui requires some cosmetic amendment for Linux.  Please see release note for LInux build files.

4. For the development istallation the icon editor plugin does not display anything in Designer.
   This is a fault with iconeditorplugin.  The plugin does not appear in teh control selection list.
   The sizing handles of the grid do, however, appear when the control is selected.
   If you start Designer outside Visual Studio then open Neo.ui the edit control does appear (as a black square)
   and the control is present in the list.

14th December 2008
