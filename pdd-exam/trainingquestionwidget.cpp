#include "trainingquestionwidget.h"
#include "databox.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <answerwidget.h>
#include "programconfig.h"

using namespace pdd;

TrainingQuestionWidget::TrainingQuestionWidget(const uint n, QWidget *parent)
    : QWidget(parent)
    , number(n)
    , rightAnswer(DataBox::inst().getTaskQuestion(number).get_answer())
    , choise(0)
    , helpLabel(0) {
    makeGUI();

    connect(confirmButton, SIGNAL(clicked(bool)), this, SLOT(onConfirmButton()));
    connect(prevButton, SIGNAL(clicked(bool)), this, SLOT(onPrevButton()));
    connect(nextButton, SIGNAL(clicked(bool)), this, SLOT(onNextButton()));
    connect(helpButton, SIGNAL(clicked(bool)), this, SLOT(onHelpButton()));
    connect(enumButton, SIGNAL(clicked(bool)), this, SIGNAL(showEnum()));
}


void TrainingQuestionWidget::onSelectAnswer(const uint n) {
    choise = n;
    for(int i = 0; i < answers.size(); ++i) {
        answers[i]->onSelect(n);
    }
    confirmButton->setEnabled(true);
}


void TrainingQuestionWidget::onConfirmButton() {
    // добавляем к комментарию сообщение с номером ответа экзаменуемого
    QString comment = helpLabel->text();
    comment.append( QString(". Ваш ответ - %1.").arg(choise) );
    helpLabel->setText( comment );

    for(int i = 0; i < answers.size(); ++i) {
        answers[i]->onConfirm();
    }
    confirmButton->setDisabled(true);

    resultLabel->setAutoFillBackground(true);
    QPalette pal = resultLabel->palette();
    if(choise == rightAnswer) {
        pal.setColor( QPalette::Window, QColor("green") );
        pal.setColor( QPalette::WindowText, QColor("white") );
        resultLabel->setText( "ВЫ ДАЛИ ПРАВИЛЬНЫЙ ОТВЕТ" );
        emit answerRight(number);

    } else {
        pal.setColor( QPalette::Window, QColor("red") );
        pal.setColor( QPalette::WindowText, QColor("white") );
        resultLabel->setText( "ВЫ ДАЛИ НЕПРАВИЛЬНЫЙ ОТВЕТ" );
        helpLabel->show();
        emit answerWrong(number);
    }

    resultLabel->setPalette(pal);
    resultLabel->show();
}


void TrainingQuestionWidget::onNextButton() {
    emit nextQuestion(number);
}

void TrainingQuestionWidget::onPrevButton() {
    emit prevQuestion(number);
}

void TrainingQuestionWidget::onHelpButton() {
    if(helpLabel->isHidden())
        helpLabel->show();
    else
        helpLabel->hide();
}


void TrainingQuestionWidget::makeGUI() {
    const pdd::question& quest = DataBox::inst().getTaskQuestion(number);

    QVBoxLayout* layout = new QVBoxLayout;

    // Номер вопроса
    QLabel* captionLabel = new QLabel(QString("Вопрос %1").arg(number+1));
    QHBoxLayout* captionLayout = new QHBoxLayout;
    captionLayout->addWidget(captionLabel, 0, Qt::AlignLeft|Qt::AlignTop);
    layout->addLayout( captionLayout );

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
        //QLabel* w = new QLabel(answerStrs[i]);
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
    // Кнопка вызова пояснения
    helpButton = new QPushButton("ПОМОЩЬ");
    footer->addWidget(helpButton, 0, Qt::AlignRight);
    // Кнопка перехода к предыдущему вопросу
    prevButton = new QPushButton("НАЗАД");
    footer->addWidget(prevButton, 0, Qt::AlignRight);
    // Кнопка перехода к перечню вопросов
    enumButton = new QPushButton("К ПЕРЕЧНЮ");
    footer->addWidget(enumButton, 0, Qt::AlignRight);
    // Кнопка перехода к следующему вопросу
    nextButton = new QPushButton("ДАЛЬШЕ");
    footer->addWidget(nextButton, 0, Qt::AlignRight);
    layout->addLayout( footer );

    setLayout(layout);
}
