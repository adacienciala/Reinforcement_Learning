/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_nope;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_01;
    QLabel *label_09;
    QLabel *label_06;
    QLabel *label_07;
    QLabel *label_08;
    QLabel *label_05;
    QLabel *label_03;
    QLabel *label_04;
    QLabel *label_02;
    QScrollBar *horizontalScrollBar;
    QLabel *labelText;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(646, 437);
        MainWindow->setAutoFillBackground(true);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setAutoFillBackground(true);
        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(60, 20, 521, 311));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setVerticalSpacing(6);
        gridLayout->setContentsMargins(5, 5, 5, 5);
        label_nope = new QLabel(gridLayoutWidget);
        label_nope->setObjectName(QString::fromUtf8("label_nope"));
        label_nope->setAutoFillBackground(true);
        label_nope->setFrameShape(QFrame::Box);
        label_nope->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_nope, 1, 1, 1, 1);

        label_10 = new QLabel(gridLayoutWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setAutoFillBackground(true);
        label_10->setFrameShape(QFrame::Box);
        label_10->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_10, 1, 3, 1, 1);

        label_11 = new QLabel(gridLayoutWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setAutoFillBackground(true);
        label_11->setFrameShape(QFrame::Box);
        label_11->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_11, 2, 3, 1, 1);

        label_01 = new QLabel(gridLayoutWidget);
        label_01->setObjectName(QString::fromUtf8("label_01"));
        label_01->setAutoFillBackground(true);
        label_01->setFrameShape(QFrame::Box);
        label_01->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_01, 0, 0, 1, 1);

        label_09 = new QLabel(gridLayoutWidget);
        label_09->setObjectName(QString::fromUtf8("label_09"));
        label_09->setAutoFillBackground(true);
        label_09->setFrameShape(QFrame::Box);
        label_09->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_09, 0, 3, 1, 1);

        label_06 = new QLabel(gridLayoutWidget);
        label_06->setObjectName(QString::fromUtf8("label_06"));
        label_06->setAutoFillBackground(true);
        label_06->setFrameShape(QFrame::Box);
        label_06->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_06, 0, 2, 1, 1);

        label_07 = new QLabel(gridLayoutWidget);
        label_07->setObjectName(QString::fromUtf8("label_07"));
        label_07->setAutoFillBackground(true);
        label_07->setFrameShape(QFrame::Box);
        label_07->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_07, 1, 2, 1, 1);

        label_08 = new QLabel(gridLayoutWidget);
        label_08->setObjectName(QString::fromUtf8("label_08"));
        label_08->setAutoFillBackground(true);
        label_08->setFrameShape(QFrame::Box);
        label_08->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_08, 2, 2, 1, 1);

        label_05 = new QLabel(gridLayoutWidget);
        label_05->setObjectName(QString::fromUtf8("label_05"));
        label_05->setAutoFillBackground(true);
        label_05->setFrameShape(QFrame::Box);
        label_05->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_05, 2, 1, 1, 1);

        label_03 = new QLabel(gridLayoutWidget);
        label_03->setObjectName(QString::fromUtf8("label_03"));
        label_03->setAutoFillBackground(true);
        label_03->setFrameShape(QFrame::Box);
        label_03->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_03, 2, 0, 1, 1);

        label_04 = new QLabel(gridLayoutWidget);
        label_04->setObjectName(QString::fromUtf8("label_04"));
        label_04->setAutoFillBackground(true);
        label_04->setFrameShape(QFrame::Box);
        label_04->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_04, 0, 1, 1, 1);

        label_02 = new QLabel(gridLayoutWidget);
        label_02->setObjectName(QString::fromUtf8("label_02"));
        label_02->setAutoFillBackground(true);
        label_02->setFrameShape(QFrame::Box);
        label_02->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_02, 1, 0, 1, 1);

        horizontalScrollBar = new QScrollBar(centralwidget);
        horizontalScrollBar->setObjectName(QString::fromUtf8("horizontalScrollBar"));
        horizontalScrollBar->setGeometry(QRect(60, 395, 521, 21));
        horizontalScrollBar->setAutoFillBackground(true);
        horizontalScrollBar->setMaximum(100);
        horizontalScrollBar->setOrientation(Qt::Horizontal);
        horizontalScrollBar->setInvertedAppearance(false);
        horizontalScrollBar->setInvertedControls(true);
        labelText = new QLabel(centralwidget);
        labelText->setObjectName(QString::fromUtf8("labelText"));
        labelText->setGeometry(QRect(60, 335, 521, 41));
        labelText->setAutoFillBackground(true);
        labelText->setFrameShape(QFrame::Box);
        labelText->setAlignment(Qt::AlignCenter);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label_nope->setText(QString());
        label_10->setText(QString());
        label_11->setText(QString());
        label_01->setText(QString());
        label_09->setText(QString());
        label_06->setText(QString());
        label_07->setText(QString());
        label_08->setText(QString());
        label_05->setText(QString());
        label_03->setText(QString());
        label_04->setText(QString());
        label_02->setText(QString());
        labelText->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
