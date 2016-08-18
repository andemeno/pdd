#include "merge_dialog.h"
#include "ui_merge_dialog.h"
#include <pdd_db.h>
#include <QtWidgets>

merge_dialog::merge_dialog(const pdd::pdd_db &doc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::merge_dialog) {
    ui->setupUi(this);

    ui->tableWidget->setRowCount(doc.get_themes_count());

    for(int r = 0; r < ui->tableWidget->rowCount(); ++r) {
        QCheckBox* check_box = new QCheckBox;
        themes.push_back(check_box);

        QSpinBox* spin_from = new QSpinBox;
        spin_from->setRange(1, doc.get_questions_count(r+1));
        spin_from->setValue(spin_from->minimum());

        QSpinBox* spin_to = new QSpinBox;
        spin_to->setRange(1, doc.get_questions_count(r+1));
        spin_to->setValue(spin_to->maximum());

        ui->tableWidget->setCellWidget(r, 0, check_box);
        ui->tableWidget->setCellWidget(r, 1, spin_from);
        ui->tableWidget->setCellWidget(r, 2, spin_to);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

}

merge_dialog::~merge_dialog() {
    delete ui;
}

std::map<uint, std::pair<uint, uint> > merge_dialog::get_data_to_merge() {
    return to_merge;
}

void merge_dialog::on_buttonBox_accepted() {
    for(uint r = 0; r < themes.size(); ++r) {
        if(themes[r]->isChecked()) {
            int from = ((QSpinBox*)(ui->tableWidget->cellWidget(r, 1)))->value();
            int to = ((QSpinBox*)(ui->tableWidget->cellWidget(r, 2)))->value();
            if(from > to) {
                qDebug() << "incorrect question range: from"<< from << "to" << to;
                continue;
            }
            qDebug() << "to merge: theme" << r+1 << "from" << from << "to" << to;

            to_merge.insert(std::make_pair(r+1, std::make_pair(from, to)));
        }
    }

    accept();
}

void merge_dialog::on_buttonBox_rejected() {
    reject();
}
