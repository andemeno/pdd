#include "selectorbar.h"
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <pdd_db.h>

using namespace pdd;

SelectorBar::SelectorBar(QSharedPointer<pdd_db> db, QWidget *parent)
    : QWidget(parent)
    , doc(db) {

    themeBox = new QComboBox;
    themeBox->addItems(doc->get_theme_names());
    themeBox->setCurrentIndex(0);

    QLabel* label = new QLabel("вопрос");
    numberBox = new QSpinBox;
    numberBox->setMinimum(1);
    QPushButton* show_btn = new QPushButton("Показать");
    QPushButton* prev_btn = new QPushButton("<<");
    QPushButton* next_btn = new QPushButton(">>");
    //QPushButton* remove_btn = new QPushButton("Удалить");

    connect(themeBox, SIGNAL(activated(int)), this, SLOT(init_question_box(int)));
    connect(show_btn, SIGNAL(clicked(bool)), this, SLOT(on_show_question()));
    connect(prev_btn, SIGNAL(clicked(bool)), this, SLOT(on_prev()));
    connect(next_btn, SIGNAL(clicked(bool)), this, SLOT(on_next()));
    //connect(remove_btn, SIGNAL(clicked(bool)), this, SLOT(on_remove()));

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addSpacing(10);
    layout->addWidget(themeBox);
    layout->addSpacing(10);
    layout->addWidget(label);
    layout->addSpacing(10);
    layout->addWidget(numberBox);
    layout->addSpacing(10);
    layout->addWidget(show_btn);
    layout->addWidget(prev_btn);
    layout->addWidget(next_btn);
    //layout->addWidget(remove_btn);

    setLayout(layout);
    init_question_box(0);
}

void SelectorBar::init_question_box(const int theme) const {
    uint count = doc->get_questions_count(theme+1);
    numberBox->setMaximum(count);
    numberBox->setValue(1);
}

void SelectorBar::on_show_question() {
    emit show_question(themeBox->currentIndex()+1, numberBox->value());
}

void SelectorBar::on_prev() {
    numberBox->setValue(numberBox->value()-1);
    on_show_question();
}

void SelectorBar::on_next() {
    numberBox->setValue(numberBox->value()+1);
    on_show_question();
}

void SelectorBar::on_remove() {
    //uint th_n = themeBox->currentIndex() + 1;
    //uint n = numberBox->value();
    //doc->remove_question(th_n, n);
    //doc->save_changes();
}
