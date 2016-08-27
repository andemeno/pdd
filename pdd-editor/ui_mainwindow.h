/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_merge;
    QAction *rename_action;
    QAction *choose_bd_action;
    QWidget *centralWidget;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1200, 800);
        action_merge = new QAction(MainWindow);
        action_merge->setObjectName(QStringLiteral("action_merge"));
        rename_action = new QAction(MainWindow);
        rename_action->setObjectName(QStringLiteral("rename_action"));
        choose_bd_action = new QAction(MainWindow);
        choose_bd_action->setObjectName(QStringLiteral("choose_bd_action"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1200, 19));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menu->addAction(action_merge);
        menu->addAction(rename_action);
        menu_2->addAction(choose_bd_action);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        action_merge->setText(QApplication::translate("MainWindow", "\320\241\320\273\320\270\321\217\320\275\320\270\320\265...", 0));
        rename_action->setText(QApplication::translate("MainWindow", "\320\237\320\265\321\200\320\265\320\270\320\274\320\265\320\275\320\276\320\262\320\260\320\275\320\270\320\265...", 0));
        choose_bd_action->setText(QApplication::translate("MainWindow", "\320\243\320\272\320\260\320\267\320\260\321\202\321\214 \320\277\321\203\321\202\321\214 \320\272 \320\221\320\224...", 0));
        menu->setTitle(QApplication::translate("MainWindow", "\320\224\320\265\320\271\321\201\321\202\320\262\320\270\321\217", 0));
        menu_2->setTitle(QApplication::translate("MainWindow", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
