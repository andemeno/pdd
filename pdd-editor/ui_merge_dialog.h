/********************************************************************************
** Form generated from reading UI file 'merge_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MERGE_DIALOG_H
#define UI_MERGE_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_merge_dialog
{
public:
    QTableWidget *tableWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *merge_dialog)
    {
        if (merge_dialog->objectName().isEmpty())
            merge_dialog->setObjectName(QStringLiteral("merge_dialog"));
        merge_dialog->resize(625, 423);
        tableWidget = new QTableWidget(merge_dialog);
        if (tableWidget->columnCount() < 3)
            tableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setGeometry(QRect(10, 50, 611, 361));
        buttonBox = new QDialogButtonBox(merge_dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(450, 10, 165, 18));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(merge_dialog);

        QMetaObject::connectSlotsByName(merge_dialog);
    } // setupUi

    void retranslateUi(QDialog *merge_dialog)
    {
        merge_dialog->setWindowTitle(QApplication::translate("merge_dialog", "Dialog", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("merge_dialog", "\320\242\320\265\320\274\320\260", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("merge_dialog", "\320\236\321\202", 0));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("merge_dialog", "\320\224\320\276", 0));
    } // retranslateUi

};

namespace Ui {
    class merge_dialog: public Ui_merge_dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MERGE_DIALOG_H
