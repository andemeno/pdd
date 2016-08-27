#include "question_form.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <pdd_db.h>
#include "itemdialog.h"

using namespace pdd;

question_form::question_form(QSharedPointer<pdd_db> db, const uint th_n, const uint n, QWidget *parent)
    : QWidget(parent)
    , doc(db)
    , quest(doc->get_question(th_n, n)) {

    QLabel* label = new QLabel("Здесь будет иллюстрация");
    label->setPixmap(QPixmap(QString("../pdd_resources/images/ab/%1").arg(quest.get_image_name())));
    label->setAlignment(Qt::AlignHCenter);

    table = new QTableWidget;
    table->setColumnCount(2);
    QStringList vars = quest.get_answers();
    uint vars_count = vars.size();
    table->setRowCount(10);

    table->setItem(0, 0, new QTableWidgetItem("Тема"));
    table->setItem(0, 1, new QTableWidgetItem(QString("%1").arg(quest.get_theme_number())));

    table->setItem(1, 0, new QTableWidgetItem("Номер вопроса"));
    table->setItem(1, 1, new QTableWidgetItem(QString("%1").arg(quest.get_number())));

    table->setItem(2, 0, new QTableWidgetItem("Вопрос"));
    table->setItem(2, 1, new QTableWidgetItem(quest.get_task()));

    table->setItem(3, 0, new QTableWidgetItem("Пояснение"));
    table->setItem(3, 1, new QTableWidgetItem(quest.get_comment()));

    table->setItem(4, 0, new QTableWidgetItem("Правильный ответ"));
    table->setItem(4, 1, new QTableWidgetItem(QString("%1").arg(quest.get_answer())));

    for(uint v = 0; v < 5; ++v) {
        table->setItem(5 + v, 0, new QTableWidgetItem(QString("Вариант %1").arg(v+1)));
        table->item(5 + v, 0)->setFlags(Qt::ItemIsEnabled);
        if(v < vars_count) table->setItem(5 + v, 1, new QTableWidgetItem(vars[v]));
        else table->setItem(5 + v, 1, new QTableWidgetItem);
    }

    QFont font("Consolas", 10);
    QFont font_bold("Consolas Bold", 8);
    for(int r = 0; r < table->rowCount(); ++r) {
        table->item(r, 0)->setFont(font_bold);
        table->item(r, 0)->setFlags(Qt::ItemIsEnabled);
        table->item(r, 1)->setFont(font);
        if(r != 0 && r != 1 && r != 4)
            table->item(r, 1)->setFlags(Qt::ItemIsEnabled);
    }

    table->item(1, 1)->setSelected(true);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    QPushButton* save_btn = new QPushButton("Сохранить изменения");
    connect(save_btn, SIGNAL(clicked(bool)), this, SLOT(on_save()));
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addStretch(1);
    hbox->addWidget(save_btn);

    connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(on_cell_dbl_clicked(int,int)));

    QVBoxLayout* lay = new QVBoxLayout;
    lay->addWidget(label);
    lay->addWidget(table);
    lay->addLayout(hbox);

    setLayout(lay);
}

void question_form::on_cell_dbl_clicked(const int row, const int column) {
    if(column == 0) return;
    if(row == 0 || row == 1 || row == 4) return;
    ItemDialog dialog(table->item(row, column)->text(), this);

    if(row == 2) dialog.setWindowTitle("Отредактируйте текст вопроса");
    else if(row == 3) dialog.setWindowTitle("Отредактируйте текст пояснения");
    else dialog.setWindowTitle("Отредактируйте текст варианта ответа");

    int result = dialog.exec();
    if(result == QDialog::Accepted) {
        table->item(row, column)->setText(dialog.itemText());
    }
}

void question_form::on_save() {
    quest.set_theme_number(table->item(0,1)->text().toUInt());
    quest.set_number(table->item(1,1)->text().toUInt());
    quest.set_task(table->item(2,1)->text());
    quest.set_comment(table->item(3,1)->text());
    quest.set_answer(table->item(4,1)->text().toUInt());
    QStringList answers;
    for(int r = 5; r < table->rowCount(); ++r) {
        QString str = table->item(r,1)->text();
        if(!str.isEmpty()) answers << str;
    }
    quest.set_answers(answers);

    doc->set_question(quest);
    if(doc->save_changes())
        emit question_saved();
}
