#include "questionwidget.h"
#include "databox.h"
#include "question.h"
#include "programconfig.h"
#include "answerwidget.h"
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QKeyEvent>

using namespace pdd;

QuestionWidget::QuestionWidget(const uint n, QWidget *parent)
    : QWidget(parent)
    , number( n )
    , choise(-1)
    , rightAnswer(-1) {

    makeGUI();

    connect(confirmButton, SIGNAL(clicked(bool)), this, SLOT(onConfirmButton()));
    connect(prevButton, SIGNAL(clicked(bool)), this, SLOT(onPrevButton()));
    connect(nextButton, SIGNAL(clicked(bool)), this, SLOT(onNextButton()));
    connect(helpButton, SIGNAL(clicked(bool)), this, SLOT(onHelpButton()));
    connect(enumButton, SIGNAL(clicked(bool)), this, SIGNAL(showEnum()));

    DataBox& box = DataBox::inst();
    connect(&box, SIGNAL(failTask()), this, SLOT(onEndTask()));
    connect(&box, SIGNAL(successTask()), this, SLOT(onEndTask()));

    setFocusPolicy(Qt::StrongFocus);
}


void QuestionWidget::onNextButton() {
    emit nextQuestion(number);
}


void QuestionWidget::onPrevButton() {
    emit prevQuestion(number);
}


void QuestionWidget::onSelectAnswer(uint n) {
    choise = n;
    for(int i = 0; i < answers.size(); ++i) {
        answers[i]->onSelect(n);
    }
    confirmButton->setEnabled(true);
}


void QuestionWidget::onConfirmButton() {
    // добавляем к комментарию сообщение с номером ответа экзаменуемого
    QString comment = helpLabel->text();
    comment.append( QString(". Ваш ответ - %1.").arg(choise) );
    helpLabel->setText( comment );

    for(int i = 0; i < answers.size(); ++i) {
        answers[i]->onConfirm();
    }
    confirmButton->setDisabled(true);

    emit answerQuestion(number, choise);
    emit showEnum();

    DataBox::inst().setAnswer(number, choise);
}


void QuestionWidget::onEndTask() {
    for(int i = 0; i < answers.size(); ++i) {
        answers[i]->onEndTask();
    }
    confirmButton->setDisabled(true);
    helpLabel->show();

    if( choise > 0 ) {
        bool right = (uint(choise) == rightAnswer);
        QPalette pal = resultLabel->palette();
        pal.setColor( QPalette::Window, right ? QColor("green") : QColor("red") );
        pal.setColor( QPalette::WindowText, QColor("white") );
        resultLabel->setAutoFillBackground(true);
        resultLabel->setPalette( pal );
        resultLabel->setText( right ? "ВЫ ДАЛИ ПРАВИЛЬНЫЙ ОТВЕТ" : "ВЫ ДАЛИ НЕПРАВИЛЬНЫЙ ОТВЕТ" );
    } else {
        resultLabel->setText( "ВЫ НЕ ДАЛИ ОТВЕТ" );
    }
    resultLabel->show();

    enumButton->setText("К РЕЗУЛЬТАТАМ");
}


void QuestionWidget::onHelpButton() {
    if(helpLabel->isHidden())
        helpLabel->show();
    else
        helpLabel->hide();
}


void QuestionWidget::keyPressEvent(QKeyEvent* event) {

    if(Config::inst().cheat) {
        if(event->key() == Qt::Key_Space) {
            onSelectAnswer(DataBox::inst().getTaskQuestion(number).get_answer());
            onConfirmButton();
        }
    }

    QWidget::keyPressEvent(event);
}


void QuestionWidget::makeGUI() {
    const pdd::question& quest = DataBox::inst().getTaskQuestion(number);
    rightAnswer = quest.get_answer();

    QVBoxLayout* layout = new QVBoxLayout;

    // Иллюстрация
    QLabel* pixmapLabel = new QLabel;
    QString image_name = QString("%1%2").arg(Config::inst().abPathToImages.c_str()).arg(quest.get_image_name());
    pixmapLabel->setPixmap( QPixmap(image_name));
    QHBoxLayout* pixmapLayout = new QHBoxLayout;
    pixmapLayout->addWidget(pixmapLabel, 0, Qt::AlignHCenter|Qt::AlignTop);
    layout->addLayout( pixmapLayout );

    resultLabel = new QLabel();
    resultLabel->hide();
    QHBoxLayout* rhbox = new QHBoxLayout;
    rhbox->addWidget( resultLabel, 0, Qt::AlignCenter );
    layout->addLayout( rhbox );

    // Вопрос
    QLabel* taskLabel = new QLabel(quest.get_task());
    QHBoxLayout* taskLayout = new QHBoxLayout;
    taskLayout->addWidget( taskLabel );
    taskLabel->setWordWrap( true );
    layout->addLayout( taskLayout );

    // Варианты ответов
    QGridLayout* grid = new QGridLayout;
    grid->setColumnStretch( 1, 1 );
    QStringList answerStrs = quest.get_answers();
    for( int i = 0; i < answerStrs.size(); ++i ) {
        AnswerWidget* w = new AnswerWidget(answerStrs[i], i+1, rightAnswer);
        connect(w, SIGNAL(select(uint)), this, SLOT(onSelectAnswer(uint)));
        answers.append( w );
        grid->addWidget( new QLabel(""), i, 0, Qt::AlignTop );
        grid->addWidget( w, i, 1, Qt::AlignTop );
    }
    layout->addLayout( grid );

    // Кнопка подтверждения выбранного варианта ответа
    confirmButton = new QPushButton("ОТВЕТИТЬ");
    confirmButton->setDisabled(true);
    QHBoxLayout* confirmLayout = new QHBoxLayout;
    confirmLayout->addWidget( confirmButton, 0, Qt::AlignTop|Qt::AlignLeft );
    confirmLayout->addStretch( 1 );
    layout->addLayout( confirmLayout );
    layout->addSpacing( 10 );

    // Пояснение
    QHBoxLayout* helpLayout = new QHBoxLayout;
    QString comment(quest.get_comment());
    comment.append(QString("\nПравильный ответ - %1").arg(quest.get_answer()));
    helpLabel = new QLabel(comment);
    helpLabel->setWordWrap(true);
    helpLabel->setFrameStyle( QFrame::Panel|QFrame::Sunken );
    helpLabel->setLineWidth(2);
    helpLabel->hide();
    helpLayout->addWidget( helpLabel );
    layout->addLayout(helpLayout);

    // Кнопки навигации

    QHBoxLayout* footer = new QHBoxLayout;
    footer->addStretch( 1 );
    // Кнопка вызова пояснения (только в локальном режиме)
    helpButton = new QPushButton("ПОМОЩЬ");
    footer->addWidget(helpButton, 0, Qt::AlignRight);
    if(!Config::inst().localMode) {
        helpButton->hide();
    }
    // Кнопка перехода к предыдущему вопросу
    prevButton = new QPushButton("НАЗАД");
    footer->addWidget(prevButton, 0, Qt::AlignRight);
    // Кнопка перехода к перечню вопросов
    enumButton = new QPushButton("К ПЕРЕЧНЮ");
    footer->addWidget(enumButton, 0, Qt::AlignRight);
    // Кнопка перехода к следующему вопросу
    nextButton = new QPushButton("ДАЛЬШЕ");
    footer->addWidget(nextButton, 0, Qt::AlignRight);
    layout->addLayout(footer);

    setLayout(layout);
}
