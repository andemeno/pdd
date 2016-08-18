#include "itemdialog.h"
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

ItemDialog::ItemDialog(const QString str, QWidget *parent)
    : QDialog(parent)
    , text(str) {

    edit = new QTextEdit(text);
    QPushButton* okButton = new QPushButton("Применить");
    connect(okButton, SIGNAL(clicked(bool)), this, SLOT(onOkButton()));
    QPushButton* cancelButton = new QPushButton("Отмена");
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addStretch(1);
    hbox->addWidget(okButton);
    hbox->addWidget(cancelButton);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(edit);
    vbox->addLayout(hbox);

    setLayout(vbox);
}

QString ItemDialog::itemText() {
    return text;
}

void ItemDialog::onOkButton() {
    text = edit->toPlainText();
    accept();
}

