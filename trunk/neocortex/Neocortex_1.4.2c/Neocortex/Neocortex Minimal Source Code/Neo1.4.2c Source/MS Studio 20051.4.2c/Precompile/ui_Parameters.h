/********************************************************************************
** Form generated from reading UI file 'Parameters.ui'
**
** Created: Wed Jul 14 18:01:14 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARAMETERS_H
#define UI_PARAMETERS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_Parameters
{
public:
    QLabel *label_2;
    QSpinBox *RegionCount;
    QLabel *label_3;
    QLabel *ForgetThresholdLegend;
    QLabel *label_4;
    QLabel *label_5;
    QPushButton *OK;
    QLabel *label_6;
    QLabel *Side_0;
    QLabel *LabSide_0;
    QLabel *LabSide_2;
    QLabel *LabSide_1;
    QLabel *LabSide_3;
    QLabel *Side_1;
    QLabel *Side_2;
    QLabel *Side_3;
    QLabel *label_7;
    QDoubleSpinBox *ForgetThreshold_0;
    QDoubleSpinBox *ForgetThreshold_1;
    QDoubleSpinBox *ForgetThreshold_2;
    QDoubleSpinBox *ForgetThreshold_3;
    QSpinBox *SideCompression_0;
    QSpinBox *SideCompression_1;
    QSpinBox *SideCompression_2;
    QSpinBox *SideCompression_3;
    QLabel *SideCompression_Hippo;
    QLabel *label_10;
    QLabel *Side_4;
    QCheckBox *DeletionByPercentage;
    QLabel *label;
    QLabel *label_9;
    QSpinBox *MaxMemory_0;
    QSpinBox *MaxMemory_1;
    QSpinBox *MaxMemory_2;
    QSpinBox *MaxMemory_3;
    QCheckBox *DoNotForget;
    QLabel *label_12;
    QLabel *label_13;
    QPushButton *Revert;
    QLineEdit *SecondPassSaccadesLimit;
    QLineEdit *FirstPassSaccadesLimit;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_18;
    QLabel *LabMaxMemory_0;
    QLabel *label_19;
    QLabel *LabMaxMemory_1;
    QLabel *LabMaxMemory_2;
    QLabel *LabMaxMemory_3;
    QLabel *label_20;
    QSpinBox *LowUsageThreshold_0;
    QSpinBox *LowUsageThreshold_1;
    QSpinBox *LowUsageThreshold_2;
    QSpinBox *LowUsageThreshold_3;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_8;
    QLabel *label_23;
    QLabel *label_24;
    QLabel *label_25;

    void setupUi(QDialog *Parameters)
    {
        if (Parameters->objectName().isEmpty())
            Parameters->setObjectName(QString::fromUtf8("Parameters"));
        Parameters->setWindowModality(Qt::ApplicationModal);
        Parameters->resize(433, 541);
        label_2 = new QLabel(Parameters);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 31, 106, 16));
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        label_2->setFont(font);
        RegionCount = new QSpinBox(Parameters);
        RegionCount->setObjectName(QString::fromUtf8("RegionCount"));
        RegionCount->setGeometry(QRect(122, 33, 46, 22));
        RegionCount->setMinimum(2);
        RegionCount->setMaximum(4);
        label_3 = new QLabel(Parameters);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(11, 84, 101, 20));
        ForgetThresholdLegend = new QLabel(Parameters);
        ForgetThresholdLegend->setObjectName(QString::fromUtf8("ForgetThresholdLegend"));
        ForgetThresholdLegend->setGeometry(QRect(11, 237, 101, 30));
        ForgetThresholdLegend->setWordWrap(true);
        label_4 = new QLabel(Parameters);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(122, 63, 311, 16));
        label_5 = new QLabel(Parameters);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(12, 61, 81, 20));
        OK = new QPushButton(Parameters);
        OK->setObjectName(QString::fromUtf8("OK"));
        OK->setGeometry(QRect(350, 510, 75, 25));
        label_6 = new QLabel(Parameters);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(11, 136, 33, 16));
        Side_0 = new QLabel(Parameters);
        Side_0->setObjectName(QString::fromUtf8("Side_0"));
        Side_0->setGeometry(QRect(80, 155, 16, 16));
        LabSide_0 = new QLabel(Parameters);
        LabSide_0->setObjectName(QString::fromUtf8("LabSide_0"));
        LabSide_0->setGeometry(QRect(80, 136, 20, 16));
        LabSide_2 = new QLabel(Parameters);
        LabSide_2->setObjectName(QString::fromUtf8("LabSide_2"));
        LabSide_2->setGeometry(QRect(218, 136, 16, 16));
        LabSide_1 = new QLabel(Parameters);
        LabSide_1->setObjectName(QString::fromUtf8("LabSide_1"));
        LabSide_1->setGeometry(QRect(148, 136, 16, 16));
        LabSide_3 = new QLabel(Parameters);
        LabSide_3->setObjectName(QString::fromUtf8("LabSide_3"));
        LabSide_3->setGeometry(QRect(288, 136, 16, 16));
        Side_1 = new QLabel(Parameters);
        Side_1->setObjectName(QString::fromUtf8("Side_1"));
        Side_1->setGeometry(QRect(148, 156, 16, 16));
        Side_2 = new QLabel(Parameters);
        Side_2->setObjectName(QString::fromUtf8("Side_2"));
        Side_2->setGeometry(QRect(218, 156, 16, 16));
        Side_3 = new QLabel(Parameters);
        Side_3->setObjectName(QString::fromUtf8("Side_3"));
        Side_3->setGeometry(QRect(288, 156, 16, 16));
        label_7 = new QLabel(Parameters);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(12, 155, 31, 16));
        ForgetThreshold_0 = new QDoubleSpinBox(Parameters);
        ForgetThreshold_0->setObjectName(QString::fromUtf8("ForgetThreshold_0"));
        ForgetThreshold_0->setGeometry(QRect(121, 243, 63, 20));
        ForgetThreshold_0->setSingleStep(0.1);
        ForgetThreshold_1 = new QDoubleSpinBox(Parameters);
        ForgetThreshold_1->setObjectName(QString::fromUtf8("ForgetThreshold_1"));
        ForgetThreshold_1->setGeometry(QRect(191, 243, 63, 20));
        ForgetThreshold_1->setSingleStep(0.1);
        ForgetThreshold_2 = new QDoubleSpinBox(Parameters);
        ForgetThreshold_2->setObjectName(QString::fromUtf8("ForgetThreshold_2"));
        ForgetThreshold_2->setGeometry(QRect(261, 243, 62, 20));
        ForgetThreshold_2->setSingleStep(0.1);
        ForgetThreshold_3 = new QDoubleSpinBox(Parameters);
        ForgetThreshold_3->setObjectName(QString::fromUtf8("ForgetThreshold_3"));
        ForgetThreshold_3->setGeometry(QRect(331, 243, 63, 20));
        ForgetThreshold_3->setSingleStep(1);
        SideCompression_0 = new QSpinBox(Parameters);
        SideCompression_0->setObjectName(QString::fromUtf8("SideCompression_0"));
        SideCompression_0->setGeometry(QRect(121, 86, 46, 20));
        SideCompression_0->setMinimum(1);
        SideCompression_0->setMaximum(99);
        SideCompression_1 = new QSpinBox(Parameters);
        SideCompression_1->setObjectName(QString::fromUtf8("SideCompression_1"));
        SideCompression_1->setGeometry(QRect(191, 85, 46, 20));
        SideCompression_1->setMinimum(1);
        SideCompression_2 = new QSpinBox(Parameters);
        SideCompression_2->setObjectName(QString::fromUtf8("SideCompression_2"));
        SideCompression_2->setGeometry(QRect(261, 84, 46, 20));
        SideCompression_2->setMinimum(1);
        SideCompression_3 = new QSpinBox(Parameters);
        SideCompression_3->setObjectName(QString::fromUtf8("SideCompression_3"));
        SideCompression_3->setGeometry(QRect(330, 85, 46, 20));
        SideCompression_3->setMinimum(1);
        SideCompression_Hippo = new QLabel(Parameters);
        SideCompression_Hippo->setObjectName(QString::fromUtf8("SideCompression_Hippo"));
        SideCompression_Hippo->setGeometry(QRect(396, 86, 16, 16));
        label_10 = new QLabel(Parameters);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(312, 136, 63, 16));
        Side_4 = new QLabel(Parameters);
        Side_4->setObjectName(QString::fromUtf8("Side_4"));
        Side_4->setGeometry(QRect(349, 156, 31, 16));
        DeletionByPercentage = new QCheckBox(Parameters);
        DeletionByPercentage->setObjectName(QString::fromUtf8("DeletionByPercentage"));
        DeletionByPercentage->setGeometry(QRect(10, 214, 361, 19));
        label = new QLabel(Parameters);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(11, 190, 145, 16));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        label_9 = new QLabel(Parameters);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(11, 354, 101, 29));
        label_9->setFont(font);
        label_9->setWordWrap(true);
        MaxMemory_0 = new QSpinBox(Parameters);
        MaxMemory_0->setObjectName(QString::fromUtf8("MaxMemory_0"));
        MaxMemory_0->setGeometry(QRect(121, 358, 61, 18));
        MaxMemory_0->setMaximum(30000);
        MaxMemory_0->setSingleStep(1000);
        MaxMemory_0->setValue(30000);
        MaxMemory_1 = new QSpinBox(Parameters);
        MaxMemory_1->setObjectName(QString::fromUtf8("MaxMemory_1"));
        MaxMemory_1->setGeometry(QRect(191, 358, 61, 18));
        MaxMemory_1->setMaximum(30000);
        MaxMemory_1->setSingleStep(1000);
        MaxMemory_1->setValue(30000);
        MaxMemory_2 = new QSpinBox(Parameters);
        MaxMemory_2->setObjectName(QString::fromUtf8("MaxMemory_2"));
        MaxMemory_2->setGeometry(QRect(261, 358, 61, 18));
        MaxMemory_2->setMaximum(30000);
        MaxMemory_2->setSingleStep(1000);
        MaxMemory_2->setValue(30000);
        MaxMemory_3 = new QSpinBox(Parameters);
        MaxMemory_3->setObjectName(QString::fromUtf8("MaxMemory_3"));
        MaxMemory_3->setGeometry(QRect(331, 358, 61, 18));
        MaxMemory_3->setMaximum(30000);
        MaxMemory_3->setSingleStep(1000);
        MaxMemory_3->setValue(30000);
        DoNotForget = new QCheckBox(Parameters);
        DoNotForget->setObjectName(QString::fromUtf8("DoNotForget"));
        DoNotForget->setGeometry(QRect(11, 300, 21, 17));
        label_12 = new QLabel(Parameters);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(40, 299, 371, 30));
        label_12->setWordWrap(true);
        label_13 = new QLabel(Parameters);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(11, 280, 131, 21));
        label_13->setFont(font1);
        Revert = new QPushButton(Parameters);
        Revert->setObjectName(QString::fromUtf8("Revert"));
        Revert->setGeometry(QRect(268, 510, 75, 25));
        SecondPassSaccadesLimit = new QLineEdit(Parameters);
        SecondPassSaccadesLimit->setObjectName(QString::fromUtf8("SecondPassSaccadesLimit"));
        SecondPassSaccadesLimit->setGeometry(QRect(340, 480, 41, 20));
        SecondPassSaccadesLimit->setCursorPosition(4);
        FirstPassSaccadesLimit = new QLineEdit(Parameters);
        FirstPassSaccadesLimit->setObjectName(QString::fromUtf8("FirstPassSaccadesLimit"));
        FirstPassSaccadesLimit->setGeometry(QRect(140, 480, 41, 20));
        FirstPassSaccadesLimit->setCursorPosition(4);
        label_14 = new QLabel(Parameters);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(10, 457, 102, 14));
        label_14->setFont(font1);
        label_15 = new QLabel(Parameters);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(10, 480, 117, 14));
        label_16 = new QLabel(Parameters);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(200, 480, 128, 14));
        label_18 = new QLabel(Parameters);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(11, 119, 171, 16));
        LabMaxMemory_0 = new QLabel(Parameters);
        LabMaxMemory_0->setObjectName(QString::fromUtf8("LabMaxMemory_0"));
        LabMaxMemory_0->setGeometry(QRect(140, 336, 20, 16));
        label_19 = new QLabel(Parameters);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setGeometry(QRect(55, 336, 33, 16));
        LabMaxMemory_1 = new QLabel(Parameters);
        LabMaxMemory_1->setObjectName(QString::fromUtf8("LabMaxMemory_1"));
        LabMaxMemory_1->setGeometry(QRect(210, 336, 20, 16));
        LabMaxMemory_2 = new QLabel(Parameters);
        LabMaxMemory_2->setObjectName(QString::fromUtf8("LabMaxMemory_2"));
        LabMaxMemory_2->setGeometry(QRect(280, 336, 20, 16));
        LabMaxMemory_3 = new QLabel(Parameters);
        LabMaxMemory_3->setObjectName(QString::fromUtf8("LabMaxMemory_3"));
        LabMaxMemory_3->setGeometry(QRect(350, 336, 20, 16));
        label_20 = new QLabel(Parameters);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setGeometry(QRect(10, 417, 102, 16));
        LowUsageThreshold_0 = new QSpinBox(Parameters);
        LowUsageThreshold_0->setObjectName(QString::fromUtf8("LowUsageThreshold_0"));
        LowUsageThreshold_0->setGeometry(QRect(120, 417, 46, 22));
        LowUsageThreshold_0->setMaximum(99);
        LowUsageThreshold_0->setValue(2);
        LowUsageThreshold_1 = new QSpinBox(Parameters);
        LowUsageThreshold_1->setObjectName(QString::fromUtf8("LowUsageThreshold_1"));
        LowUsageThreshold_1->setGeometry(QRect(190, 418, 46, 21));
        LowUsageThreshold_1->setMaximum(99);
        LowUsageThreshold_1->setValue(2);
        LowUsageThreshold_2 = new QSpinBox(Parameters);
        LowUsageThreshold_2->setObjectName(QString::fromUtf8("LowUsageThreshold_2"));
        LowUsageThreshold_2->setGeometry(QRect(260, 417, 46, 22));
        LowUsageThreshold_2->setMaximum(99);
        LowUsageThreshold_2->setValue(2);
        LowUsageThreshold_3 = new QSpinBox(Parameters);
        LowUsageThreshold_3->setObjectName(QString::fromUtf8("LowUsageThreshold_3"));
        LowUsageThreshold_3->setGeometry(QRect(330, 417, 46, 22));
        LowUsageThreshold_3->setMaximum(99);
        LowUsageThreshold_3->setValue(2);
        label_21 = new QLabel(Parameters);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setGeometry(QRect(10, 396, 381, 16));
        label_22 = new QLabel(Parameters);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setGeometry(QRect(4, 12, 181, 16));
        label_22->setFont(font1);
        label_8 = new QLabel(Parameters);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(160, 190, 16, 16));
        label_8->setFont(font1);
        label_8->setFrameShape(QFrame::Box);
        label_8->setFrameShadow(QFrame::Plain);
        label_8->setLineWidth(1);
        label_8->setMidLineWidth(1);
        label_8->setTextFormat(Qt::PlainText);
        label_8->setScaledContents(false);
        label_8->setAlignment(Qt::AlignCenter);
        label_23 = new QLabel(Parameters);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setGeometry(QRect(180, 12, 16, 16));
        label_23->setFont(font1);
        label_23->setFrameShape(QFrame::Box);
        label_23->setFrameShadow(QFrame::Plain);
        label_23->setLineWidth(1);
        label_23->setMidLineWidth(1);
        label_23->setTextFormat(Qt::PlainText);
        label_23->setScaledContents(false);
        label_23->setAlignment(Qt::AlignCenter);
        label_24 = new QLabel(Parameters);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        label_24->setGeometry(QRect(139, 282, 16, 16));
        label_24->setFont(font1);
        label_24->setFrameShape(QFrame::Box);
        label_24->setFrameShadow(QFrame::Plain);
        label_24->setLineWidth(1);
        label_24->setMidLineWidth(1);
        label_24->setTextFormat(Qt::PlainText);
        label_24->setScaledContents(false);
        label_24->setAlignment(Qt::AlignCenter);
        label_25 = new QLabel(Parameters);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setGeometry(QRect(119, 456, 16, 16));
        label_25->setFont(font1);
        label_25->setFrameShape(QFrame::Box);
        label_25->setFrameShadow(QFrame::Plain);
        label_25->setLineWidth(1);
        label_25->setMidLineWidth(1);
        label_25->setTextFormat(Qt::PlainText);
        label_25->setScaledContents(false);
        label_25->setAlignment(Qt::AlignCenter);
        QWidget::setTabOrder(RegionCount, SideCompression_0);
        QWidget::setTabOrder(SideCompression_0, SideCompression_1);
        QWidget::setTabOrder(SideCompression_1, SideCompression_2);
        QWidget::setTabOrder(SideCompression_2, SideCompression_3);
        QWidget::setTabOrder(SideCompression_3, DeletionByPercentage);
        QWidget::setTabOrder(DeletionByPercentage, ForgetThreshold_0);
        QWidget::setTabOrder(ForgetThreshold_0, ForgetThreshold_1);
        QWidget::setTabOrder(ForgetThreshold_1, ForgetThreshold_2);
        QWidget::setTabOrder(ForgetThreshold_2, ForgetThreshold_3);
        QWidget::setTabOrder(ForgetThreshold_3, DoNotForget);
        QWidget::setTabOrder(DoNotForget, MaxMemory_0);
        QWidget::setTabOrder(MaxMemory_0, MaxMemory_1);
        QWidget::setTabOrder(MaxMemory_1, MaxMemory_2);
        QWidget::setTabOrder(MaxMemory_2, MaxMemory_3);
        QWidget::setTabOrder(MaxMemory_3, Revert);
        QWidget::setTabOrder(Revert, OK);

        retranslateUi(Parameters);

        QMetaObject::connectSlotsByName(Parameters);
    } // setupUi

    void retranslateUi(QDialog *Parameters)
    {
        Parameters->setWindowTitle(QApplication::translate("Parameters", "Neocortex Parameters", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Parameters->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        Parameters->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        Parameters->setWhatsThis(QApplication::translate("Parameters", "Side Compression spinners will adjust themselves to values that give integral region sides.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        label_2->setText(QApplication::translate("Parameters", "Number of Regions", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        RegionCount->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("Parameters", "Side compression", 0, QApplication::UnicodeUTF8));
        ForgetThresholdLegend->setText(QApplication::translate("Parameters", "Delete memories below threshold", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Parameters", "Eye-0               0-1                 1-2                  2- 3           3 - Hippo", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Parameters", "Child-Parent", 0, QApplication::UnicodeUTF8));
        OK->setText(QApplication::translate("Parameters", "OK", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Parameters", "Region", 0, QApplication::UnicodeUTF8));
        Side_0->setText(QApplication::translate("Parameters", "0", 0, QApplication::UnicodeUTF8));
        LabSide_0->setText(QApplication::translate("Parameters", "0", 0, QApplication::UnicodeUTF8));
        LabSide_2->setText(QApplication::translate("Parameters", "2", 0, QApplication::UnicodeUTF8));
        LabSide_1->setText(QApplication::translate("Parameters", "1", 0, QApplication::UnicodeUTF8));
        LabSide_3->setText(QApplication::translate("Parameters", "3", 0, QApplication::UnicodeUTF8));
        Side_1->setText(QApplication::translate("Parameters", "0", 0, QApplication::UnicodeUTF8));
        Side_2->setText(QApplication::translate("Parameters", "0", 0, QApplication::UnicodeUTF8));
        Side_3->setText(QApplication::translate("Parameters", "0", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Parameters", "Side", 0, QApplication::UnicodeUTF8));
        SideCompression_Hippo->setText(QApplication::translate("Parameters", "1", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Parameters", "Hippocampus", 0, QApplication::UnicodeUTF8));
        Side_4->setText(QApplication::translate("Parameters", "1", 0, QApplication::UnicodeUTF8));
        DeletionByPercentage->setText(QApplication::translate("Parameters", "Deletion method (toggle between threshold and percentage method)", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Parameters", "Memory Deletion Method", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Parameters", "Constrain No of Memories to:", 0, QApplication::UnicodeUTF8));
        DoNotForget->setText(QString());
        label_12->setText(QApplication::translate("Parameters", "If this is checked, the memory will be constrained to the values set on the spinners below.", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("Parameters", "Constraining Memory", 0, QApplication::UnicodeUTF8));
        Revert->setText(QApplication::translate("Parameters", "Revert", 0, QApplication::UnicodeUTF8));
        SecondPassSaccadesLimit->setInputMask(QApplication::translate("Parameters", "0000; ", 0, QApplication::UnicodeUTF8));
        SecondPassSaccadesLimit->setText(QApplication::translate("Parameters", "0000", 0, QApplication::UnicodeUTF8));
        FirstPassSaccadesLimit->setInputMask(QApplication::translate("Parameters", "0000; ", 0, QApplication::UnicodeUTF8));
        FirstPassSaccadesLimit->setText(QApplication::translate("Parameters", "0000", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("Parameters", "Training Saccades", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("Parameters", "First pass saccades Limit", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("Parameters", "Second pass saccades limit", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("Parameters", "Result of Region side calculations", 0, QApplication::UnicodeUTF8));
        LabMaxMemory_0->setText(QApplication::translate("Parameters", "0", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("Parameters", "Region", 0, QApplication::UnicodeUTF8));
        LabMaxMemory_1->setText(QApplication::translate("Parameters", "1", 0, QApplication::UnicodeUTF8));
        LabMaxMemory_2->setText(QApplication::translate("Parameters", "2", 0, QApplication::UnicodeUTF8));
        LabMaxMemory_3->setText(QApplication::translate("Parameters", "3", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("Parameters", "Low Usage Threshold", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("Parameters", "The threshold at which a memory can be discarded is set on these spinners:", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("Parameters", "Setting the Number of Regions", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_8->setToolTip(QApplication::translate("Parameters", "After each region's forward pass, a selection of low-usage pattern memories is \n"
"deleted. Memories can be selected for deletion either by means of a threshold \n"
"value or by means of a percentage of the memories. Threshold is a fraction of \n"
"the average size of all memories.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_8->setText(QApplication::translate("Parameters", "?", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_23->setToolTip(QApplication::translate("Parameters", "You can set the side compression for each Region's child-parent relation.  \n"
"The dialog constrains the values such that each side measurement is an integer.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_23->setText(QApplication::translate("Parameters", "?", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_24->setToolTip(QApplication::translate("Parameters", "Instead of forgetting memories after training a Region we reduce the memory \n"
"that is available in the first place. During learning, after a Region's memory \n"
"becomes full, we just discard the least useful (e.g. the oldest and least \n"
"populous) memories and replace them with the new ones.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_24->setText(QApplication::translate("Parameters", "?", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_25->setToolTip(QApplication::translate("Parameters", "During training. the first pass for each Region learns the input patterns for that \n"
"Region and assigns a name to each.  \n"
"\n"
"\n"
"In the second pass pass each child Region learns the conditional probability of \n"
"each of its patterns given each parent pattern that occured for the same object.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_25->setText(QApplication::translate("Parameters", "?", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Parameters: public Ui_Parameters {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARAMETERS_H
