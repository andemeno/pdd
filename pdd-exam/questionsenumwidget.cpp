#include "questionsenumwidget.h"
#include "smallquestionwidget.h"
#include "programconfig.h"
#include "databox.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

using namespace pdd;

QuestionsEnumWidget::QuestionsEnumWidget(QWidget *parent)
    : QWidget(parent) {

    makeGUI();

    DataBox& box = DataBox::inst();
    connect( &box, SIGNAL(extraTask(uint)), this, SLOT(onExtraTask()) );
}

void QuestionsEnumWidget::onExtraTask() {
    uint count = DataBox::inst().getExtraCount();

    const DataBox::questions_array& questions = DataBox::inst().getQuestions();
    uint firstExtNumber = questions.size() - count;
    int row = gridLayout->rowCount();
    int column = 0;
    for( uint i = firstExtNumber; i < questions.size(); ++i ) {
        SmallQuestionWidget* w = new SmallQuestionWidget(i);
        connect(w, SIGNAL(selectQuestion(int)), this, SIGNAL(selectQuestion(int)));
        connect(this, SIGNAL(answerQuestion(uint,uint)), w, SLOT(onAnswerQuestion(uint,uint)));
        gridLayout->addWidget( w, row, column );
        column++;
        if( column > 4 ) {
            column = 0; row++;
        }
    }
}


void QuestionsEnumWidget::makeGUI() {

    QVBoxLayout* mainLayout = new QVBoxLayout;

//    QHBoxLayout* rhbox = new QHBoxLayout;
//    resultLabel = new QLabel;
//    resultLabel->hide();
//    rhbox->addWidget( resultLabel, 0, Qt::AlignCenter );
//    rhbox->setSizeConstraint( QLayout::SetFixedSize );
//    mainLayout->addLayout(rhbox, 0);

    gridLayout = new QGridLayout;
    const DataBox::questions_array& questions = DataBox::inst().getQuestions();
    int row = 0;
    int column = 0;
    for( uint i = 0; i < questions.size(); ++i ) {
        SmallQuestionWidget* w = new SmallQuestionWidget(i);
        connect(w, SIGNAL(selectQuestion(int)), this, SIGNAL(selectQuestion(int)));
        connect(this, SIGNAL(answerQuestion(uint,uint)), w, SLOT(onAnswerQuestion(uint,uint)));
        gridLayout->addWidget( w, row, column );
        column++;
        if( column > 4 ) {
            column = 0; row++;
        }
    }

    QHBoxLayout* ghbox = new QHBoxLayout;
    ghbox->addLayout(gridLayout, 0);
    mainLayout->addLayout(ghbox, 1);

    setLayout(mainLayout);
}
