/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topBarLayout;
    QPushButton *removeButton;
    QLabel *filePathLabel;
    QHBoxLayout *imagePanelsLayout;
    QGroupBox *groupOriginal;
    QVBoxLayout *origBoxLayout;
    QLabel *labelOriginal;
    QGroupBox *groupResult;
    QVBoxLayout *resultBoxLayout;
    QLabel *labelResult;
    QGroupBox *groupControls;
    QHBoxLayout *controlsLayout;
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QDoubleSpinBox *spinK;
    QVBoxLayout *vboxLayout1;
    QLabel *label1;
    QSpinBox *spinBlockSize;
    QVBoxLayout *vboxLayout2;
    QLabel *label2;
    QDoubleSpinBox *spinThreshold;
    QSpacerItem *hSpacer;
    QPushButton *runHarrisButton;
    QSpacerItem *hSpacer2;
    QVBoxLayout *vboxLayout3;
    QLabel *labelCornerCount;
    QLabel *labelTime;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1150, 720);
        MainWindow->setMinimumSize(QSize(900, 600));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setSpacing(10);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(12, 12, 12, 12);
        topBarLayout = new QHBoxLayout();
        topBarLayout->setSpacing(10);
        topBarLayout->setObjectName("topBarLayout");
        removeButton = new QPushButton(centralwidget);
        removeButton->setObjectName("removeButton");
        removeButton->setMinimumSize(QSize(140, 36));
        removeButton->setEnabled(false);
        removeButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        topBarLayout->addWidget(removeButton);

        filePathLabel = new QLabel(centralwidget);
        filePathLabel->setObjectName("filePathLabel");
        filePathLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

        topBarLayout->addWidget(filePathLabel);


        mainLayout->addLayout(topBarLayout);

        imagePanelsLayout = new QHBoxLayout();
        imagePanelsLayout->setSpacing(10);
        imagePanelsLayout->setObjectName("imagePanelsLayout");
        groupOriginal = new QGroupBox(centralwidget);
        groupOriginal->setObjectName("groupOriginal");
        origBoxLayout = new QVBoxLayout(groupOriginal);
        origBoxLayout->setSpacing(2);
        origBoxLayout->setObjectName("origBoxLayout");
        origBoxLayout->setContentsMargins(4, 4, 4, 4);
        labelOriginal = new QLabel(groupOriginal);
        labelOriginal->setObjectName("labelOriginal");
        labelOriginal->setAlignment(Qt::AlignCenter);
        labelOriginal->setMinimumSize(QSize(460, 460));
        labelOriginal->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        origBoxLayout->addWidget(labelOriginal);


        imagePanelsLayout->addWidget(groupOriginal);

        groupResult = new QGroupBox(centralwidget);
        groupResult->setObjectName("groupResult");
        resultBoxLayout = new QVBoxLayout(groupResult);
        resultBoxLayout->setSpacing(2);
        resultBoxLayout->setObjectName("resultBoxLayout");
        resultBoxLayout->setContentsMargins(4, 4, 4, 4);
        labelResult = new QLabel(groupResult);
        labelResult->setObjectName("labelResult");
        labelResult->setAlignment(Qt::AlignCenter);
        labelResult->setMinimumSize(QSize(460, 460));

        resultBoxLayout->addWidget(labelResult);


        imagePanelsLayout->addWidget(groupResult);


        mainLayout->addLayout(imagePanelsLayout);

        groupControls = new QGroupBox(centralwidget);
        groupControls->setObjectName("groupControls");
        controlsLayout = new QHBoxLayout(groupControls);
        controlsLayout->setSpacing(24);
        controlsLayout->setObjectName("controlsLayout");
        controlsLayout->setContentsMargins(16, 10, 16, 10);
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(4);
        vboxLayout->setObjectName("vboxLayout");
        label = new QLabel(groupControls);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignCenter);

        vboxLayout->addWidget(label);

        spinK = new QDoubleSpinBox(groupControls);
        spinK->setObjectName("spinK");
        spinK->setMinimumSize(QSize(120, 34));
        spinK->setAlignment(Qt::AlignCenter);
        spinK->setDecimals(3);
        spinK->setMinimum(0.040000000000000);
        spinK->setMaximum(0.060000000000000);
        spinK->setSingleStep(0.002000000000000);
        spinK->setValue(0.040000000000000);

        vboxLayout->addWidget(spinK);


        controlsLayout->addLayout(vboxLayout);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(4);
        vboxLayout1->setObjectName("vboxLayout1");
        label1 = new QLabel(groupControls);
        label1->setObjectName("label1");
        label1->setAlignment(Qt::AlignCenter);

        vboxLayout1->addWidget(label1);

        spinBlockSize = new QSpinBox(groupControls);
        spinBlockSize->setObjectName("spinBlockSize");
        spinBlockSize->setMinimumSize(QSize(120, 34));
        spinBlockSize->setAlignment(Qt::AlignCenter);
        spinBlockSize->setMinimum(3);
        spinBlockSize->setMaximum(7);
        spinBlockSize->setSingleStep(2);
        spinBlockSize->setValue(3);

        vboxLayout1->addWidget(spinBlockSize);


        controlsLayout->addLayout(vboxLayout1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setSpacing(4);
        vboxLayout2->setObjectName("vboxLayout2");
        label2 = new QLabel(groupControls);
        label2->setObjectName("label2");
        label2->setAlignment(Qt::AlignCenter);

        vboxLayout2->addWidget(label2);

        spinThreshold = new QDoubleSpinBox(groupControls);
        spinThreshold->setObjectName("spinThreshold");
        spinThreshold->setMinimumSize(QSize(120, 34));
        spinThreshold->setAlignment(Qt::AlignCenter);
        spinThreshold->setDecimals(3);
        spinThreshold->setMinimum(0.010000000000000);
        spinThreshold->setMaximum(0.200000000000000);
        spinThreshold->setSingleStep(0.010000000000000);
        spinThreshold->setValue(0.010000000000000);

        vboxLayout2->addWidget(spinThreshold);


        controlsLayout->addLayout(vboxLayout2);

        hSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        controlsLayout->addItem(hSpacer);

        runHarrisButton = new QPushButton(groupControls);
        runHarrisButton->setObjectName("runHarrisButton");
        runHarrisButton->setMinimumSize(QSize(190, 44));
        runHarrisButton->setEnabled(false);
        runHarrisButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        controlsLayout->addWidget(runHarrisButton);

        hSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        controlsLayout->addItem(hSpacer2);

        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setSpacing(6);
        vboxLayout3->setObjectName("vboxLayout3");
        labelCornerCount = new QLabel(groupControls);
        labelCornerCount->setObjectName("labelCornerCount");

        vboxLayout3->addWidget(labelCornerCount);

        labelTime = new QLabel(groupControls);
        labelTime->setObjectName("labelTime");

        vboxLayout3->addWidget(labelTime);


        controlsLayout->addLayout(vboxLayout3);


        mainLayout->addWidget(groupControls);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1150, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Harris Corner Detector", nullptr));
        removeButton->setText(QCoreApplication::translate("MainWindow", "Remove Image", nullptr));
        filePathLabel->setText(QCoreApplication::translate("MainWindow", "No image loaded \342\200\224 double-click the image panel to upload", nullptr));
        groupOriginal->setTitle(QCoreApplication::translate("MainWindow", "Original Image \342\200\224 double-click to upload", nullptr));
        labelOriginal->setText(QCoreApplication::translate("MainWindow", "Double-click here to upload an image", nullptr));
        groupResult->setTitle(QCoreApplication::translate("MainWindow", "Result \342\200\224 Harris Corners", nullptr));
        labelResult->setText(QCoreApplication::translate("MainWindow", "Run the detector to see results here", nullptr));
        groupControls->setTitle(QCoreApplication::translate("MainWindow", "Harris Parameters and Controls", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Sensitivity  k", nullptr));
        label1->setText(QCoreApplication::translate("MainWindow", "Block Size", nullptr));
        label2->setText(QCoreApplication::translate("MainWindow", "Threshold", nullptr));
        runHarrisButton->setText(QCoreApplication::translate("MainWindow", "Run Harris Detector", nullptr));
        labelCornerCount->setText(QCoreApplication::translate("MainWindow", "Corners detected: \342\200\224", nullptr));
        labelTime->setText(QCoreApplication::translate("MainWindow", "Computation time: \342\200\224", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
