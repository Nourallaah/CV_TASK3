/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
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
    QSpacerItem *spacerItem;
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
    QPushButton *runHarrisButton;
    QPushButton *runSiftButton;
    QSpacerItem *spacerItem1;
    QVBoxLayout *vboxLayout3;
    QLabel *label3;
    QComboBox *comboMatchMethod;
    QPushButton *runMatchButton;
    QVBoxLayout *vboxLayout4;
    QLabel *labelCornerCount;
    QLabel *labelTime;
    QHBoxLayout *contentLayout;
    QVBoxLayout *leftColumn;
    QGroupBox *groupImage1;
    QVBoxLayout *img1Layout;
    QLabel *labelImage1;
    QGroupBox *groupImage2;
    QVBoxLayout *img2Layout;
    QLabel *labelImage2;
    QGroupBox *groupMatchResult;
    QVBoxLayout *matchLayout;
    QLabel *labelMatchResult;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1400, 900);
        MainWindow->setMinimumSize(QSize(1000, 700));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setSpacing(8);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(10, 10, 10, 10);
        topBarLayout = new QHBoxLayout();
        topBarLayout->setObjectName("topBarLayout");
        removeButton = new QPushButton(centralwidget);
        removeButton->setObjectName("removeButton");
        removeButton->setMinimumSize(QSize(130, 34));
        removeButton->setMaximumSize(QSize(130, 34));
        removeButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        topBarLayout->addWidget(removeButton);

        filePathLabel = new QLabel(centralwidget);
        filePathLabel->setObjectName("filePathLabel");
        filePathLabel->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

        topBarLayout->addWidget(filePathLabel);

        spacerItem = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        topBarLayout->addItem(spacerItem);


        mainLayout->addLayout(topBarLayout);

        groupControls = new QGroupBox(centralwidget);
        groupControls->setObjectName("groupControls");
        groupControls->setMaximumSize(QSize(16777215, 120));
        controlsLayout = new QHBoxLayout(groupControls);
        controlsLayout->setSpacing(20);
        controlsLayout->setObjectName("controlsLayout");
        vboxLayout = new QVBoxLayout();
        vboxLayout->setObjectName("vboxLayout");
        label = new QLabel(groupControls);
        label->setObjectName("label");

        vboxLayout->addWidget(label);

        spinK = new QDoubleSpinBox(groupControls);
        spinK->setObjectName("spinK");
        spinK->setValue(0.040000000000000);
        spinK->setSingleStep(0.002000000000000);
        spinK->setMaximumSize(QSize(120, 16777215));

        vboxLayout->addWidget(spinK);


        controlsLayout->addLayout(vboxLayout);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setObjectName("vboxLayout1");
        label1 = new QLabel(groupControls);
        label1->setObjectName("label1");

        vboxLayout1->addWidget(label1);

        spinBlockSize = new QSpinBox(groupControls);
        spinBlockSize->setObjectName("spinBlockSize");
        spinBlockSize->setValue(3);
        spinBlockSize->setSingleStep(2);
        spinBlockSize->setMaximumSize(QSize(120, 16777215));

        vboxLayout1->addWidget(spinBlockSize);


        controlsLayout->addLayout(vboxLayout1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setObjectName("vboxLayout2");
        label2 = new QLabel(groupControls);
        label2->setObjectName("label2");

        vboxLayout2->addWidget(label2);

        spinThreshold = new QDoubleSpinBox(groupControls);
        spinThreshold->setObjectName("spinThreshold");
        spinThreshold->setValue(0.010000000000000);
        spinThreshold->setSingleStep(0.010000000000000);
        spinThreshold->setMaximumSize(QSize(120, 16777215));

        vboxLayout2->addWidget(spinThreshold);


        controlsLayout->addLayout(vboxLayout2);

        runHarrisButton = new QPushButton(groupControls);
        runHarrisButton->setObjectName("runHarrisButton");

        controlsLayout->addWidget(runHarrisButton);

        runSiftButton = new QPushButton(groupControls);
        runSiftButton->setObjectName("runSiftButton");

        controlsLayout->addWidget(runSiftButton);

        spacerItem1 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        controlsLayout->addItem(spacerItem1);

        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setObjectName("vboxLayout3");
        label3 = new QLabel(groupControls);
        label3->setObjectName("label3");

        vboxLayout3->addWidget(label3);

        comboMatchMethod = new QComboBox(groupControls);
        comboMatchMethod->addItem(QString());
        comboMatchMethod->addItem(QString());
        comboMatchMethod->setObjectName("comboMatchMethod");
        comboMatchMethod->setMaximumSize(QSize(100, 16777215));

        vboxLayout3->addWidget(comboMatchMethod);


        controlsLayout->addLayout(vboxLayout3);

        runMatchButton = new QPushButton(groupControls);
        runMatchButton->setObjectName("runMatchButton");
        runMatchButton->setMinimumSize(QSize(140, 44));

        controlsLayout->addWidget(runMatchButton);

        vboxLayout4 = new QVBoxLayout();
        vboxLayout4->setObjectName("vboxLayout4");
        labelCornerCount = new QLabel(groupControls);
        labelCornerCount->setObjectName("labelCornerCount");

        vboxLayout4->addWidget(labelCornerCount);

        labelTime = new QLabel(groupControls);
        labelTime->setObjectName("labelTime");

        vboxLayout4->addWidget(labelTime);


        controlsLayout->addLayout(vboxLayout4);


        mainLayout->addWidget(groupControls);

        contentLayout = new QHBoxLayout();
        contentLayout->setSpacing(8);
        contentLayout->setObjectName("contentLayout");
        leftColumn = new QVBoxLayout();
        leftColumn->setSpacing(8);
        leftColumn->setObjectName("leftColumn");
        groupImage1 = new QGroupBox(centralwidget);
        groupImage1->setObjectName("groupImage1");
        img1Layout = new QVBoxLayout(groupImage1);
        img1Layout->setSpacing(0);
        img1Layout->setObjectName("img1Layout");
        img1Layout->setContentsMargins(4, 4, 4, 4);
        labelImage1 = new QLabel(groupImage1);
        labelImage1->setObjectName("labelImage1");
        labelImage1->setAlignment(Qt::AlignCenter);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelImage1->sizePolicy().hasHeightForWidth());
        labelImage1->setSizePolicy(sizePolicy);
        labelImage1->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        img1Layout->addWidget(labelImage1);


        leftColumn->addWidget(groupImage1);

        groupImage2 = new QGroupBox(centralwidget);
        groupImage2->setObjectName("groupImage2");
        img2Layout = new QVBoxLayout(groupImage2);
        img2Layout->setSpacing(0);
        img2Layout->setObjectName("img2Layout");
        img2Layout->setContentsMargins(4, 4, 4, 4);
        labelImage2 = new QLabel(groupImage2);
        labelImage2->setObjectName("labelImage2");
        labelImage2->setAlignment(Qt::AlignCenter);
        sizePolicy.setHeightForWidth(labelImage2->sizePolicy().hasHeightForWidth());
        labelImage2->setSizePolicy(sizePolicy);
        labelImage2->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        img2Layout->addWidget(labelImage2);


        leftColumn->addWidget(groupImage2);

        leftColumn->setStretch(0, 1);
        leftColumn->setStretch(1, 1);

        contentLayout->addLayout(leftColumn);

        groupMatchResult = new QGroupBox(centralwidget);
        groupMatchResult->setObjectName("groupMatchResult");
        matchLayout = new QVBoxLayout(groupMatchResult);
        matchLayout->setSpacing(0);
        matchLayout->setObjectName("matchLayout");
        matchLayout->setContentsMargins(4, 4, 4, 4);
        labelMatchResult = new QLabel(groupMatchResult);
        labelMatchResult->setObjectName("labelMatchResult");
        labelMatchResult->setAlignment(Qt::AlignCenter);
        sizePolicy.setHeightForWidth(labelMatchResult->sizePolicy().hasHeightForWidth());
        labelMatchResult->setSizePolicy(sizePolicy);

        matchLayout->addWidget(labelMatchResult);


        contentLayout->addWidget(groupMatchResult);

        contentLayout->setStretch(0, 2);
        contentLayout->setStretch(1, 3);

        mainLayout->addLayout(contentLayout);

        mainLayout->setStretch(2, 1);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1400, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Feature Detection and Matching", nullptr));
        removeButton->setText(QCoreApplication::translate("MainWindow", "Clear Images", nullptr));
        filePathLabel->setText(QCoreApplication::translate("MainWindow", "No images loaded \342\200\224 double-click the image panels to upload", nullptr));
        groupControls->setTitle(QCoreApplication::translate("MainWindow", "Parameters and Controls", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Sensitivity k", nullptr));
        label1->setText(QCoreApplication::translate("MainWindow", "Block Size", nullptr));
        label2->setText(QCoreApplication::translate("MainWindow", "Threshold", nullptr));
        runHarrisButton->setText(QCoreApplication::translate("MainWindow", "Harris Detector", nullptr));
        runSiftButton->setText(QCoreApplication::translate("MainWindow", "SIFT Generator", nullptr));
        label3->setText(QCoreApplication::translate("MainWindow", "Match Metric", nullptr));
        comboMatchMethod->setItemText(0, QCoreApplication::translate("MainWindow", "SSD", nullptr));
        comboMatchMethod->setItemText(1, QCoreApplication::translate("MainWindow", "NCC", nullptr));

        runMatchButton->setText(QCoreApplication::translate("MainWindow", "Match Features", nullptr));
        labelCornerCount->setText(QCoreApplication::translate("MainWindow", "Features detected: \342\200\224", nullptr));
        labelTime->setText(QCoreApplication::translate("MainWindow", "Computation time: \342\200\224", nullptr));
        groupImage1->setTitle(QCoreApplication::translate("MainWindow", "Image 1 \342\200\224 double-click to upload", nullptr));
        labelImage1->setText(QCoreApplication::translate("MainWindow", "Double-click to upload Image 1", nullptr));
        groupImage2->setTitle(QCoreApplication::translate("MainWindow", "Image 2 \342\200\224 double-click to upload", nullptr));
        labelImage2->setText(QCoreApplication::translate("MainWindow", "Double-click to upload Image 2", nullptr));
        groupMatchResult->setTitle(QCoreApplication::translate("MainWindow", "Resulting Output", nullptr));
        labelMatchResult->setText(QCoreApplication::translate("MainWindow", "Matching feature view will appear here", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
