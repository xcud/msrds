/********************************************************************************
** Form generated from reading UI file 'Neo.ui'
**
** Created: Thu Jul 29 20:25:22 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEO_H
#define UI_NEO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>
#include "../iconeditor/iconeditor.h"

QT_BEGIN_NAMESPACE

class Ui_mainWindow
{
public:
    QAction *actionExit;
    QAction *actionOpen;
    QAction *actionAbout;
    QWidget *centralwidget;
    QLabel *label;
    QTextEdit *SummaryMemo;
    QTextEdit *ResultMemo;
    QFrame *frame;
    QGraphicsView *graphicsView0;
    QGraphicsView *graphicsView1;
    QGraphicsView *graphicsView2;
    QGraphicsView *graphicsView3;
    QGraphicsView *graphicsView4;
    QGraphicsView *graphicsView5;
    QGraphicsView *graphicsView6;
    QGraphicsView *graphicsView7;
    QGraphicsView *graphicsView8;
    QGraphicsView *graphicsView9;
    QProgressBar *progressBar_1;
    QProgressBar *progressBar_3;
    QProgressBar *progressBar_4;
    QProgressBar *progressBar_5;
    QProgressBar *progressBar_6;
    QProgressBar *progressBar_7;
    QProgressBar *progressBar_9;
    QProgressBar *progressBar_10;
    QProgressBar *progressBar_2;
    QProgressBar *progressBar_8;
    QFrame *frame_2;
    QCheckBox *ScrollText;
    QCheckBox *UpdateImageWindow;
    QPushButton *SetParameters;
    QPushButton *LearnFolder;
    QFrame *frame_3;
    QCheckBox *NoiseOn;
    QSpinBox *NoiseFactor10;
    QLabel *label_6;
    QLabel *label_5;
    QCheckBox *InverNoisePixels;
    QCheckBox *NormalizeResults;
    QSpinBox *NoiseFactor100;
    QLabel *label_7;
    QLabel *label_4;
    QLabel *label_8;
    IconEditor *iconeditor;
    QPushButton *OpenImage;
    QPushButton *SaveImageAs;
    QPushButton *ClearImage;
    QFrame *frame_5;
    QPushButton *RecognizeMoving;
    QLabel *label_2;
    QLineEdit *SaccadesFrom;
    QPushButton *RecognizeFlash;
    QPushButton *TestSelectedFolder;
    QLineEdit *SaccadesTo;
    QLabel *label_3;
    QCheckBox *VisualiseTrainingSaccades;
    QFrame *frame_6;
    QPushButton *zoomOut;
    QPushButton *AddNoise;
    QPushButton *ShiftDown;
    QPushButton *ShiftLeft;
    QPushButton *ShiftUp;
    QPushButton *ShiftRight;
    QPushButton *zoomIn;
    QPushButton *Rotate;
    QPushButton *Stop;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menuAbout;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *mainWindow)
    {
        if (mainWindow->objectName().isEmpty())
            mainWindow->setObjectName(QString::fromUtf8("mainWindow"));
        mainWindow->resize(740, 720);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mainWindow->sizePolicy().hasHeightForWidth());
        mainWindow->setSizePolicy(sizePolicy);
        mainWindow->setMinimumSize(QSize(740, 720));
        mainWindow->setMaximumSize(QSize(740, 720));
        actionExit = new QAction(mainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionOpen = new QAction(mainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionAbout = new QAction(mainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAbout->setCheckable(true);
        centralwidget = new QWidget(mainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(290, 0, 379, 20));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        SummaryMemo = new QTextEdit(centralwidget);
        SummaryMemo->setObjectName(QString::fromUtf8("SummaryMemo"));
        SummaryMemo->setGeometry(QRect(270, 137, 471, 191));
        SummaryMemo->setFocusPolicy(Qt::ClickFocus);
        ResultMemo = new QTextEdit(centralwidget);
        ResultMemo->setObjectName(QString::fromUtf8("ResultMemo"));
        ResultMemo->setGeometry(QRect(270, 330, 471, 271));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(232);
        sizePolicy1.setHeightForWidth(ResultMemo->sizePolicy().hasHeightForWidth());
        ResultMemo->setSizePolicy(sizePolicy1);
        ResultMemo->setFocusPolicy(Qt::ClickFocus);
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(9, 18, 721, 111));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy2);
        frame->setMaximumSize(QSize(750, 200));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        graphicsView0 = new QGraphicsView(frame);
        graphicsView0->setObjectName(QString::fromUtf8("graphicsView0"));
        graphicsView0->setGeometry(QRect(38, 34, 40, 40));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(graphicsView0->sizePolicy().hasHeightForWidth());
        graphicsView0->setSizePolicy(sizePolicy3);
        graphicsView0->setMinimumSize(QSize(40, 40));
        graphicsView0->setMaximumSize(QSize(40, 40));
        graphicsView0->viewport()->setProperty("cursor", QVariant(QCursor(Qt::ArrowCursor)));
        graphicsView0->setFocusPolicy(Qt::NoFocus);
        graphicsView0->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView0->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView1 = new QGraphicsView(frame);
        graphicsView1->setObjectName(QString::fromUtf8("graphicsView1"));
        graphicsView1->setGeometry(QRect(106, 34, 40, 40));
        sizePolicy3.setHeightForWidth(graphicsView1->sizePolicy().hasHeightForWidth());
        graphicsView1->setSizePolicy(sizePolicy3);
        graphicsView1->setMinimumSize(QSize(40, 40));
        graphicsView1->setMaximumSize(QSize(40, 40));
        graphicsView1->setFocusPolicy(Qt::NoFocus);
        graphicsView1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView2 = new QGraphicsView(frame);
        graphicsView2->setObjectName(QString::fromUtf8("graphicsView2"));
        graphicsView2->setGeometry(QRect(174, 34, 40, 40));
        sizePolicy3.setHeightForWidth(graphicsView2->sizePolicy().hasHeightForWidth());
        graphicsView2->setSizePolicy(sizePolicy3);
        graphicsView2->setMinimumSize(QSize(40, 40));
        graphicsView2->setMaximumSize(QSize(40, 40));
        graphicsView2->setFocusPolicy(Qt::NoFocus);
        graphicsView2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView3 = new QGraphicsView(frame);
        graphicsView3->setObjectName(QString::fromUtf8("graphicsView3"));
        graphicsView3->setGeometry(QRect(243, 34, 40, 40));
        sizePolicy3.setHeightForWidth(graphicsView3->sizePolicy().hasHeightForWidth());
        graphicsView3->setSizePolicy(sizePolicy3);
        graphicsView3->setMinimumSize(QSize(40, 40));
        graphicsView3->setMaximumSize(QSize(40, 40));
        graphicsView3->setFocusPolicy(Qt::NoFocus);
        graphicsView3->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView4 = new QGraphicsView(frame);
        graphicsView4->setObjectName(QString::fromUtf8("graphicsView4"));
        graphicsView4->setGeometry(QRect(311, 34, 40, 40));
        sizePolicy3.setHeightForWidth(graphicsView4->sizePolicy().hasHeightForWidth());
        graphicsView4->setSizePolicy(sizePolicy3);
        graphicsView4->setMinimumSize(QSize(40, 40));
        graphicsView4->setMaximumSize(QSize(40, 40));
        graphicsView4->setFocusPolicy(Qt::NoFocus);
        graphicsView4->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView4->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView5 = new QGraphicsView(frame);
        graphicsView5->setObjectName(QString::fromUtf8("graphicsView5"));
        graphicsView5->setGeometry(QRect(380, 34, 40, 40));
        sizePolicy3.setHeightForWidth(graphicsView5->sizePolicy().hasHeightForWidth());
        graphicsView5->setSizePolicy(sizePolicy3);
        graphicsView5->setMinimumSize(QSize(40, 40));
        graphicsView5->setMaximumSize(QSize(40, 40));
        graphicsView5->setFocusPolicy(Qt::NoFocus);
        graphicsView5->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView5->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView6 = new QGraphicsView(frame);
        graphicsView6->setObjectName(QString::fromUtf8("graphicsView6"));
        graphicsView6->setGeometry(QRect(448, 34, 40, 40));
        sizePolicy3.setHeightForWidth(graphicsView6->sizePolicy().hasHeightForWidth());
        graphicsView6->setSizePolicy(sizePolicy3);
        graphicsView6->setMinimumSize(QSize(40, 40));
        graphicsView6->setMaximumSize(QSize(40, 40));
        graphicsView6->setFocusPolicy(Qt::NoFocus);
        graphicsView6->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView6->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView7 = new QGraphicsView(frame);
        graphicsView7->setObjectName(QString::fromUtf8("graphicsView7"));
        graphicsView7->setGeometry(QRect(516, 34, 40, 40));
        sizePolicy3.setHeightForWidth(graphicsView7->sizePolicy().hasHeightForWidth());
        graphicsView7->setSizePolicy(sizePolicy3);
        graphicsView7->setMinimumSize(QSize(40, 40));
        graphicsView7->setMaximumSize(QSize(40, 40));
        graphicsView7->setFocusPolicy(Qt::NoFocus);
        graphicsView7->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView7->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView8 = new QGraphicsView(frame);
        graphicsView8->setObjectName(QString::fromUtf8("graphicsView8"));
        graphicsView8->setGeometry(QRect(585, 34, 40, 40));
        sizePolicy3.setHeightForWidth(graphicsView8->sizePolicy().hasHeightForWidth());
        graphicsView8->setSizePolicy(sizePolicy3);
        graphicsView8->setMinimumSize(QSize(40, 40));
        graphicsView8->setMaximumSize(QSize(40, 40));
        graphicsView8->setFocusPolicy(Qt::NoFocus);
        graphicsView8->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView8->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView9 = new QGraphicsView(frame);
        graphicsView9->setObjectName(QString::fromUtf8("graphicsView9"));
        graphicsView9->setGeometry(QRect(653, 34, 40, 40));
        sizePolicy3.setHeightForWidth(graphicsView9->sizePolicy().hasHeightForWidth());
        graphicsView9->setSizePolicy(sizePolicy3);
        graphicsView9->setMinimumSize(QSize(40, 40));
        graphicsView9->setMaximumSize(QSize(40, 40));
        graphicsView9->setFocusPolicy(Qt::NoFocus);
        graphicsView9->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView9->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        progressBar_1 = new QProgressBar(frame);
        progressBar_1->setObjectName(QString::fromUtf8("progressBar_1"));
        progressBar_1->setGeometry(QRect(22, 11, 14, 95));
        progressBar_1->setMaximum(99);
        progressBar_1->setValue(50);
        progressBar_1->setOrientation(Qt::Vertical);
        progressBar_3 = new QProgressBar(frame);
        progressBar_3->setObjectName(QString::fromUtf8("progressBar_3"));
        progressBar_3->setGeometry(QRect(158, 11, 14, 95));
        progressBar_3->setMaximum(99);
        progressBar_3->setValue(50);
        progressBar_3->setOrientation(Qt::Vertical);
        progressBar_4 = new QProgressBar(frame);
        progressBar_4->setObjectName(QString::fromUtf8("progressBar_4"));
        progressBar_4->setGeometry(QRect(227, 11, 14, 95));
        progressBar_4->setMaximum(99);
        progressBar_4->setValue(50);
        progressBar_4->setOrientation(Qt::Vertical);
        progressBar_5 = new QProgressBar(frame);
        progressBar_5->setObjectName(QString::fromUtf8("progressBar_5"));
        progressBar_5->setGeometry(QRect(295, 11, 14, 95));
        progressBar_5->setMaximum(99);
        progressBar_5->setValue(50);
        progressBar_5->setOrientation(Qt::Vertical);
        progressBar_6 = new QProgressBar(frame);
        progressBar_6->setObjectName(QString::fromUtf8("progressBar_6"));
        progressBar_6->setGeometry(QRect(364, 11, 14, 95));
        progressBar_6->setMaximum(99);
        progressBar_6->setValue(50);
        progressBar_6->setOrientation(Qt::Vertical);
        progressBar_7 = new QProgressBar(frame);
        progressBar_7->setObjectName(QString::fromUtf8("progressBar_7"));
        progressBar_7->setGeometry(QRect(432, 11, 14, 95));
        progressBar_7->setMaximum(99);
        progressBar_7->setValue(50);
        progressBar_7->setOrientation(Qt::Vertical);
        progressBar_9 = new QProgressBar(frame);
        progressBar_9->setObjectName(QString::fromUtf8("progressBar_9"));
        progressBar_9->setGeometry(QRect(569, 11, 14, 95));
        progressBar_9->setMaximum(99);
        progressBar_9->setValue(50);
        progressBar_9->setOrientation(Qt::Vertical);
        progressBar_10 = new QProgressBar(frame);
        progressBar_10->setObjectName(QString::fromUtf8("progressBar_10"));
        progressBar_10->setGeometry(QRect(637, 11, 14, 95));
        progressBar_10->setMaximum(99);
        progressBar_10->setValue(50);
        progressBar_10->setOrientation(Qt::Vertical);
        progressBar_2 = new QProgressBar(frame);
        progressBar_2->setObjectName(QString::fromUtf8("progressBar_2"));
        progressBar_2->setGeometry(QRect(90, 11, 14, 95));
        progressBar_2->setMaximum(99);
        progressBar_2->setValue(50);
        progressBar_2->setOrientation(Qt::Vertical);
        progressBar_8 = new QProgressBar(frame);
        progressBar_8->setObjectName(QString::fromUtf8("progressBar_8"));
        progressBar_8->setGeometry(QRect(500, 11, 14, 95));
        progressBar_8->setMaximum(99);
        progressBar_8->setValue(50);
        progressBar_8->setOrientation(Qt::Vertical);
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(2, 516, 261, 61));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        ScrollText = new QCheckBox(frame_2);
        ScrollText->setObjectName(QString::fromUtf8("ScrollText"));
        ScrollText->setGeometry(QRect(10, 35, 70, 19));
        ScrollText->setChecked(true);
        UpdateImageWindow = new QCheckBox(frame_2);
        UpdateImageWindow->setObjectName(QString::fromUtf8("UpdateImageWindow"));
        UpdateImageWindow->setGeometry(QRect(130, 35, 131, 19));
        UpdateImageWindow->setChecked(true);
        SetParameters = new QPushButton(frame_2);
        SetParameters->setObjectName(QString::fromUtf8("SetParameters"));
        SetParameters->setGeometry(QRect(10, 7, 111, 24));
        LearnFolder = new QPushButton(frame_2);
        LearnFolder->setObjectName(QString::fromUtf8("LearnFolder"));
        LearnFolder->setGeometry(QRect(131, 7, 112, 24));
        frame_3 = new QFrame(centralwidget);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(422, 609, 311, 70));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        NoiseOn = new QCheckBox(frame_3);
        NoiseOn->setObjectName(QString::fromUtf8("NoiseOn"));
        NoiseOn->setGeometry(QRect(280, 2, 23, 13));
        NoiseFactor10 = new QSpinBox(frame_3);
        NoiseFactor10->setObjectName(QString::fromUtf8("NoiseFactor10"));
        NoiseFactor10->setGeometry(QRect(187, 36, 46, 20));
        NoiseFactor10->setMinimum(0);
        NoiseFactor10->setMaximum(9);
        NoiseFactor10->setValue(0);
        label_6 = new QLabel(frame_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(192, 21, 81, 14));
        label_5 = new QLabel(frame_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 2, 121, 14));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label_5->setFont(font1);
        InverNoisePixels = new QCheckBox(frame_3);
        InverNoisePixels->setObjectName(QString::fromUtf8("InverNoisePixels"));
        InverNoisePixels->setGeometry(QRect(10, 22, 111, 17));
        InverNoisePixels->setChecked(true);
        InverNoisePixels->setTristate(false);
        NormalizeResults = new QCheckBox(frame_3);
        NormalizeResults->setObjectName(QString::fromUtf8("NormalizeResults"));
        NormalizeResults->setGeometry(QRect(10, 42, 107, 17));
        NoiseFactor100 = new QSpinBox(frame_3);
        NoiseFactor100->setObjectName(QString::fromUtf8("NoiseFactor100"));
        NoiseFactor100->setGeometry(QRect(237, 36, 46, 20));
        NoiseFactor100->setMaximum(9);
        NoiseFactor100->setValue(1);
        label_7 = new QLabel(frame_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(148, 1, 78, 14));
        label_7->setFont(font1);
        label_4 = new QLabel(frame_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(230, 1, 46, 14));
        label_8 = new QLabel(frame_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(152, 21, 30, 31));
        label_8->setWordWrap(true);
        iconeditor = new IconEditor(centralwidget);
        iconeditor->setObjectName(QString::fromUtf8("iconeditor"));
        iconeditor->setGeometry(QRect(4, 138, 257, 257));
        OpenImage = new QPushButton(centralwidget);
        OpenImage->setObjectName(QString::fromUtf8("OpenImage"));
        OpenImage->setGeometry(QRect(5, 399, 81, 24));
        OpenImage->setFocusPolicy(Qt::StrongFocus);
        SaveImageAs = new QPushButton(centralwidget);
        SaveImageAs->setObjectName(QString::fromUtf8("SaveImageAs"));
        SaveImageAs->setGeometry(QRect(96, 400, 75, 24));
        SaveImageAs->setFocusPolicy(Qt::StrongFocus);
        ClearImage = new QPushButton(centralwidget);
        ClearImage->setObjectName(QString::fromUtf8("ClearImage"));
        ClearImage->setGeometry(QRect(181, 399, 80, 24));
        ClearImage->setFocusPolicy(Qt::StrongFocus);
        frame_5 = new QFrame(centralwidget);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        frame_5->setGeometry(QRect(3, 609, 411, 70));
        frame_5->setFrameShape(QFrame::StyledPanel);
        frame_5->setFrameShadow(QFrame::Raised);
        RecognizeMoving = new QPushButton(frame_5);
        RecognizeMoving->setObjectName(QString::fromUtf8("RecognizeMoving"));
        RecognizeMoving->setGeometry(QRect(290, 8, 110, 24));
        RecognizeMoving->setFocusPolicy(Qt::NoFocus);
        label_2 = new QLabel(frame_5);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(128, 7, 121, 20));
        label_2->setTextFormat(Qt::AutoText);
        label_2->setWordWrap(true);
        SaccadesFrom = new QLineEdit(frame_5);
        SaccadesFrom->setObjectName(QString::fromUtf8("SaccadesFrom"));
        SaccadesFrom->setGeometry(QRect(248, 8, 30, 20));
        sizePolicy3.setHeightForWidth(SaccadesFrom->sizePolicy().hasHeightForWidth());
        SaccadesFrom->setSizePolicy(sizePolicy3);
        SaccadesFrom->setMaximumSize(QSize(30, 20));
        SaccadesFrom->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        RecognizeFlash = new QPushButton(frame_5);
        RecognizeFlash->setObjectName(QString::fromUtf8("RecognizeFlash"));
        RecognizeFlash->setGeometry(QRect(10, 10, 111, 21));
        RecognizeFlash->setFocusPolicy(Qt::NoFocus);
        TestSelectedFolder = new QPushButton(frame_5);
        TestSelectedFolder->setObjectName(QString::fromUtf8("TestSelectedFolder"));
        TestSelectedFolder->setGeometry(QRect(290, 38, 111, 24));
        TestSelectedFolder->setFocusPolicy(Qt::NoFocus);
        SaccadesTo = new QLineEdit(frame_5);
        SaccadesTo->setObjectName(QString::fromUtf8("SaccadesTo"));
        SaccadesTo->setEnabled(true);
        SaccadesTo->setGeometry(QRect(248, 36, 30, 20));
        sizePolicy3.setHeightForWidth(SaccadesTo->sizePolicy().hasHeightForWidth());
        SaccadesTo->setSizePolicy(sizePolicy3);
        SaccadesTo->setMaximumSize(QSize(30, 20));
        SaccadesTo->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        SaccadesTo->setReadOnly(false);
        label_3 = new QLabel(frame_5);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(128, 37, 101, 16));
        VisualiseTrainingSaccades = new QCheckBox(frame_5);
        VisualiseTrainingSaccades->setObjectName(QString::fromUtf8("VisualiseTrainingSaccades"));
        VisualiseTrainingSaccades->setEnabled(true);
        VisualiseTrainingSaccades->setGeometry(QRect(12, 33, 101, 31));
        VisualiseTrainingSaccades->setChecked(false);
        frame_6 = new QFrame(centralwidget);
        frame_6->setObjectName(QString::fromUtf8("frame_6"));
        frame_6->setGeometry(QRect(2, 433, 261, 71));
        frame_6->setFocusPolicy(Qt::NoFocus);
        frame_6->setFrameShape(QFrame::StyledPanel);
        frame_6->setFrameShadow(QFrame::Raised);
        zoomOut = new QPushButton(frame_6);
        zoomOut->setObjectName(QString::fromUtf8("zoomOut"));
        zoomOut->setGeometry(QRect(170, 6, 81, 24));
        zoomOut->setFocusPolicy(Qt::NoFocus);
        AddNoise = new QPushButton(frame_6);
        AddNoise->setObjectName(QString::fromUtf8("AddNoise"));
        AddNoise->setGeometry(QRect(6, 37, 80, 20));
        AddNoise->setFocusPolicy(Qt::NoFocus);
        ShiftDown = new QPushButton(frame_6);
        ShiftDown->setObjectName(QString::fromUtf8("ShiftDown"));
        ShiftDown->setGeometry(QRect(118, 46, 24, 20));
        ShiftDown->setFocusPolicy(Qt::NoFocus);
        ShiftLeft = new QPushButton(frame_6);
        ShiftLeft->setObjectName(QString::fromUtf8("ShiftLeft"));
        ShiftLeft->setGeometry(QRect(95, 27, 24, 20));
        ShiftLeft->setFocusPolicy(Qt::NoFocus);
        ShiftUp = new QPushButton(frame_6);
        ShiftUp->setObjectName(QString::fromUtf8("ShiftUp"));
        ShiftUp->setGeometry(QRect(117, 6, 24, 20));
        ShiftUp->setFocusPolicy(Qt::NoFocus);
        ShiftRight = new QPushButton(frame_6);
        ShiftRight->setObjectName(QString::fromUtf8("ShiftRight"));
        ShiftRight->setGeometry(QRect(140, 26, 24, 20));
        ShiftRight->setFocusPolicy(Qt::NoFocus);
        zoomIn = new QPushButton(frame_6);
        zoomIn->setObjectName(QString::fromUtf8("zoomIn"));
        zoomIn->setGeometry(QRect(170, 36, 80, 24));
        zoomIn->setFocusPolicy(Qt::NoFocus);
        Rotate = new QPushButton(frame_6);
        Rotate->setObjectName(QString::fromUtf8("Rotate"));
        Rotate->setGeometry(QRect(6, 7, 81, 20));
        Rotate->setFocusPolicy(Qt::NoFocus);
        Stop = new QPushButton(centralwidget);
        Stop->setObjectName(QString::fromUtf8("Stop"));
        Stop->setGeometry(QRect(3, 580, 260, 24));
        Stop->setFont(font1);
        Stop->setFocusPolicy(Qt::NoFocus);
        mainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(mainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 740, 18));
        menubar->setDefaultUp(false);
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menuAbout = new QMenu(menubar);
        menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
        mainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(mainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        mainWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(OpenImage, SaveImageAs);
        QWidget::setTabOrder(SaveImageAs, ClearImage);
        QWidget::setTabOrder(ClearImage, SetParameters);
        QWidget::setTabOrder(SetParameters, LearnFolder);
        QWidget::setTabOrder(LearnFolder, ScrollText);
        QWidget::setTabOrder(ScrollText, UpdateImageWindow);
        QWidget::setTabOrder(UpdateImageWindow, SaccadesFrom);
        QWidget::setTabOrder(SaccadesFrom, SaccadesTo);
        QWidget::setTabOrder(SaccadesTo, InverNoisePixels);
        QWidget::setTabOrder(InverNoisePixels, NormalizeResults);
        QWidget::setTabOrder(NormalizeResults, NoiseOn);
        QWidget::setTabOrder(NoiseOn, NoiseFactor10);
        QWidget::setTabOrder(NoiseFactor10, NoiseFactor100);
        QWidget::setTabOrder(NoiseFactor100, SummaryMemo);
        QWidget::setTabOrder(SummaryMemo, ResultMemo);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menuAbout->menuAction());
        menu_File->addAction(actionExit);
        menu_File->addAction(actionOpen);
        menuAbout->addAction(actionAbout);

        retranslateUi(mainWindow);

        QMetaObject::connectSlotsByName(mainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *mainWindow)
    {
        mainWindow->setWindowTitle(QApplication::translate("mainWindow", "Neocortex", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("mainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("mainWindow", "Open", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("mainWindow", "About", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("mainWindow", "Ten Best Predictions", 0, QApplication::UnicodeUTF8));
        ScrollText->setText(QApplication::translate("mainWindow", "Scroll Text", 0, QApplication::UnicodeUTF8));
        UpdateImageWindow->setText(QApplication::translate("mainWindow", "Update image window", 0, QApplication::UnicodeUTF8));
        SetParameters->setText(QApplication::translate("mainWindow", "Set Parameters", 0, QApplication::UnicodeUTF8));
        LearnFolder->setText(QApplication::translate("mainWindow", "Select && Learn Folder", 0, QApplication::UnicodeUTF8));
        NoiseOn->setText(QString());
        label_6->setText(QApplication::translate("mainWindow", "X 10%     X 1%", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("mainWindow", "Recognition Options", 0, QApplication::UnicodeUTF8));
        InverNoisePixels->setText(QApplication::translate("mainWindow", "Invert Noise Pixels", 0, QApplication::UnicodeUTF8));
        NormalizeResults->setText(QApplication::translate("mainWindow", "Normalize Results", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("mainWindow", "Select&Test.  ", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("mainWindow", "Noise On", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("mainWindow", "Noise Factor", 0, QApplication::UnicodeUTF8));
        OpenImage->setText(QApplication::translate("mainWindow", "Open", 0, QApplication::UnicodeUTF8));
        SaveImageAs->setText(QApplication::translate("mainWindow", "Save As...", 0, QApplication::UnicodeUTF8));
        ClearImage->setText(QApplication::translate("mainWindow", "Clear Image", 0, QApplication::UnicodeUTF8));
        RecognizeMoving->setText(QApplication::translate("mainWindow", "Recognize Moving", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("mainWindow", "Saccades From/Number:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        SaccadesFrom->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        SaccadesFrom->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        SaccadesFrom->setText(QApplication::translate("mainWindow", "1", 0, QApplication::UnicodeUTF8));
        RecognizeFlash->setText(QApplication::translate("mainWindow", "Recognize Flashed", 0, QApplication::UnicodeUTF8));
        TestSelectedFolder->setText(QApplication::translate("mainWindow", "Select && Test Folder", 0, QApplication::UnicodeUTF8));
        SaccadesTo->setText(QApplication::translate("mainWindow", "4", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("mainWindow", "Maximum Saccades:", 0, QApplication::UnicodeUTF8));
        VisualiseTrainingSaccades->setText(QApplication::translate("mainWindow", "VisualizeTraining\n"
"Saccades", 0, QApplication::UnicodeUTF8));
        zoomOut->setText(QApplication::translate("mainWindow", "Zoom Out", 0, QApplication::UnicodeUTF8));
        AddNoise->setText(QApplication::translate("mainWindow", "Add Noise", 0, QApplication::UnicodeUTF8));
        ShiftDown->setText(QApplication::translate("mainWindow", "D", 0, QApplication::UnicodeUTF8));
        ShiftLeft->setText(QApplication::translate("mainWindow", "L", 0, QApplication::UnicodeUTF8));
        ShiftUp->setText(QApplication::translate("mainWindow", "U", 0, QApplication::UnicodeUTF8));
        ShiftRight->setText(QApplication::translate("mainWindow", "R", 0, QApplication::UnicodeUTF8));
        zoomIn->setText(QApplication::translate("mainWindow", "Zoom In Assist", 0, QApplication::UnicodeUTF8));
        Rotate->setText(QApplication::translate("mainWindow", "Rotate R90", 0, QApplication::UnicodeUTF8));
        Stop->setText(QApplication::translate("mainWindow", "Stop/Pause", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("mainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menuAbout->setTitle(QApplication::translate("mainWindow", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mainWindow: public Ui_mainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEO_H
