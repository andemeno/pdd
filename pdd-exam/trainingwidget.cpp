#include "trainingwidget.h"
#include "databox.h"
#include "trainingquestionwidget.h"
#include <QGridLayout>
#include <QStackedLayout>
#include <QPushButton>
#include <QButtonGroup>

using namespace pdd;

TrainingWidget::TrainingWidget(QWidget *parent)
    : QWidget(parent) {
    makeGUI();
}

void TrainingWidget::onPrevQuestion(uint n) {
    if(n == 0) n = stacked->count() - 2;
    else --n;
    stacked->setCurrentIndex(n);
}

void TrainingWidget::onNextQuestion(uint n) {
    if(n == uint(stacked->count() - 2)) n = 0;
    else ++n;
    stacked->setCurrentIndex(n);
}

void TrainingWidget::showEnum() {
    stacked->setCurrentIndex(stacked->count()-1);
}

void TrainingWidget::makeGUI() {

    EnumLabelWidget* enumWidget = new EnumLabelWidget;
    stacked = new QStackedLayout;
    const uint task_questions_count = DataBox::inst().getTaskQuestionsCount();
    //const DataBox::questions_array& questions = DataBox::inst().getQuestions();
    for( uint i = 0; i < task_questions_count; ++i ) {
        TrainingQuestionWidget* w = new TrainingQuestionWidget(i);
        connect( w, SIGNAL(prevQuestion(uint)), this, SLOT(onPrevQuestion(uint)) );
        connect( w, SIGNAL(nextQuestion(uint)), this, SLOT(onNextQuestion(uint)) );
        connect( w, SIGNAL(showEnum()), this, SLOT(showEnum()) );
        connect( w, SIGNAL(answerRight(uint)), enumWidget, SLOT(onAnswerRight(uint)) );
        connect( w, SIGNAL(answerWrong(uint)), enumWidget, SLOT(onAnswerWrong(uint)) );
        stacked->addWidget(w);
    }
    stacked->addWidget(enumWidget);
    connect(enumWidget, SIGNAL(selectQuestion(int)), stacked, SLOT(setCurrentIndex(int)));
    stacked->setCurrentWidget(enumWidget);
    setLayout(stacked);
}

EnumLabelWidget::EnumLabelWidget(QWidget *parent/* = 0*/)
    :QWidget(parent) {
    QGridLayout* grid = new QGridLayout;
    group = new QButtonGroup;
    uint count = DataBox::inst().getTaskQuestionsCount();//DataBox::inst().getQuestions().size();
    int row = 0; int column = 0;
    for(uint n=0; n<count; ++n) {
        QPushButton* btn = new QPushButton(QString("%1").arg(n+1));
        group->addButton(btn, n);
        grid->addWidget(btn, row, column);
        column++;
        if(column>9) {
            row++; column = 0;
        }
    }
    connect(group, SIGNAL(buttonClicked(int)), this, SIGNAL(selectQuestion(int)));
    setLayout(grid);
}

void EnumLabelWidget::onAnswerWrong(uint n) {
    QPalette pal = group->button(n)->palette();

    group->button(n)->setAutoFillBackground(true);
    pal.setColor( QPalette::Button, QColor("red") );
    pal.setColor( QPalette::ButtonText, QColor("red") );

    group->button(n)->setPalette(pal);
}

void EnumLabelWidget::onAnswerRight(uint n) {
    QPalette pal = group->button(n)->palette();

    group->button(n)->setAutoFillBackground(true);
    pal.setColor( QPalette::Button, QColor("green") );
    pal.setColor( QPalette::ButtonText, QColor("green") );

    group->button(n)->setPalette(pal);
}
