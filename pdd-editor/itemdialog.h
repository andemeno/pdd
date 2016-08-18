#ifndef ITEMDIALOG_H
#define ITEMDIALOG_H

#include <QDialog>

class QTextEdit;

class ItemDialog : public QDialog {
    Q_OBJECT
public:
    explicit ItemDialog(const QString str, QWidget *parent = 0);
    QString itemText();

private slots:
    void onOkButton();
private:

    QTextEdit* edit;
    QString text;
};

#endif // ITEMDIALOG_H
