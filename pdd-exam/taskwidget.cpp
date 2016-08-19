#include "taskwidget.h"
#include <QStackedWidget>
#include <QStackedLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "databox.h"
#include "questionwidget.h"
#include "questionsenumwidget.h"
#include "smallquestionenumwidget.h"
#include "programconfig.h"

using namespace pdd;

TaskWidget::TaskWidget(QWidget *parent)
    : QWidget(parent)
    , taskEnded(false) {

    makeGUI();

    DataBox& box = DataBox::inst();
    connect(&box, SIGNAL(failTask()), this, SLOT(onEndTask()));
    connect(&box, SIGNAL(successTask()), this, SLOT(onEndTask()));
    connect(&box, SIGNAL(failTask()), smallEnumWidget, SLOT(onEndTask()));
    connect(&box, SIGNAL(successTask()), smallEnumWidget, SLOT(onEndTask()));
    connect(&box, SIGNAL(extraTask(uint)), this, SLOT(onExtraTask()));
}


void TaskWidget::onPrevQuestion(uint n) {

    if(taskEnded && Config::inst().viewOnlyWrong) {
        bool wrong = false;
        while(!wrong) {
            if(n == 0)
                n = stacked->count() - 2;
            else --n;
            wrong = DataBox::inst().wrongAnswered(n);

        }
    } else {
        if(n == 0)
            n = stacked->count() - 2;
        else --n;

    }

    onShowQuestionWidget(n);
    smallEnumWidget->onSelectQuestion(n);
}


void TaskWidget::onNextQuestion(uint n) {
    if(taskEnded && Config::inst().viewOnlyWrong) {
        bool wrong = false;
        while(!wrong) {
            if(n == uint(stacked->count() - 2))
                n = 0;
            else ++n;
            wrong = DataBox::inst().wrongAnswered(n);
        }
    } else {
        if(n == uint(stacked->count() - 2))
            n = 0;
        else ++n;
    }

    onShowQuestionWidget(n);
    smallEnumWidget->onSelectQuestion(n);
}


void TaskWidget::onEndTask() {
    taskEnded = true;
    showEnum();
}

void TaskWidget::showEnum() {
    stacked->setCurrentIndex(stacked->count()-1);
    smallEnumWidget->hide();
}


void TaskWidget::onExtraTask() {
    uint count = DataBox::inst().getExtraCount();

    const DataBox::questions_array& questions = DataBox::inst().getQuestions();
    uint firstExtNumber = questions.size() - count;
    for( uint i = firstExtNumber; i < questions.size(); ++i ) {
        QuestionWidget* w = new QuestionWidget(i);
        connect( w, SIGNAL(prevQuestion(uint)), this, SLOT(onPrevQuestion(uint)) );
        connect( w, SIGNAL(nextQuestion(uint)), this, SLOT(onNextQuestion(uint)) );
        connect( w, SIGNAL(answerQuestion(uint,uint)), enumWidget, SIGNAL(answerQuestion(uint,uint)) );
        connect( w, SIGNAL(answerQuestion(uint,uint)), smallEnumWidget, SLOT(onAnswerQuestion(uint)) );
        connect( w, SIGNAL(showEnum()), this, SLOT(showEnum()) );
        stacked->insertWidget(i, w);
    }
    smallEnumWidget->onExtraTask();
    showEnum();
}


void TaskWidget::onShowQuestionWidget(int n) {
    if(taskEnded && Config::inst().viewOnlyWrong) {
        if(!DataBox::inst().wrongAnswered(n))
            return;
    }

    stacked->setCurrentIndex(n);
}


void TaskWidget::makeGUI() {
    mainLayout = new QVBoxLayout;

    smallEnumWidget = new SmallQuestionEnumWidget;
    QHBoxLayout* sehbox = new QHBoxLayout;
    sehbox->addWidget( smallEnumWidget, 0, Qt::AlignLeft );
    mainLayout->addLayout( sehbox );

    stacked = new QStackedLayout;
    enumWidget = new QuestionsEnumWidget;
    connect(enumWidget, SIGNAL(selectQuestion(int)), this, SLOT(onShowQuestionWidget(int)));
    connect(enumWidget, SIGNAL(selectQuestion(int)), smallEnumWidget, SLOT(onSelectQuestion(int)));

    const DataBox::questions_array& questions = DataBox::inst().getQuestions();
    for( uint i = 0; i < questions.size(); ++i ) {
        QuestionWidget* w = new QuestionWidget(i);
        connect( w, SIGNAL(prevQuestion(uint)), this, SLOT(onPrevQuestion(uint)) );
        connect( w, SIGNAL(nextQuestion(uint)), this, SLOT(onNextQuestion(uint)) );
        connect( w, SIGNAL(answerQuestion(uint,uint)), enumWidget, SIGNAL(answerQuestion(uint,uint)) );
        connect( w, SIGNAL(answerQuestion(uint,uint)), smallEnumWidget, SLOT(onAnswerQuestion(uint)) );
        connect( w, SIGNAL(showEnum()), this, SLOT(showEnum()) );
        stacked->addWidget(w);
    }
    stacked->addWidget(enumWidget);

    QHBoxLayout* shbox = new QHBoxLayout;
    shbox->addLayout(stacked);
    mainLayout->addLayout(shbox);
    setLayout(mainLayout);

    showEnum();
}
