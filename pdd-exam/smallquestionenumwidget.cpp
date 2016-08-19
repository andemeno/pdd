#include "smallquestionenumwidget.h"
#include "databox.h"
#include <QLabel>
#include <QGridLayout>
#include <QApplication>
#include <QDesktopWidget>

using namespace pdd;

SmallQuestionEnumWidget::SmallQuestionEnumWidget(QWidget *parent)
    : QWidget(parent)
    , taskEnded(false)
    , selectedNumber(-1) {

    grid = new QGridLayout;
    grid->setHorizontalSpacing( 5 );

    lastRow = 0;
    lastColumn = 0;
    uint count = DataBox::inst().getQuestions().size();
    for( uint n = 0; n < count; ++n ) {

        QLabel* label = new QLabel("   ");
        label->setAutoFillBackground(true);
        QPalette labelPalette = label->palette();
        label->setFrameStyle( QFrame::Panel | QFrame::Sunken );
        label->setLineWidth(1);
        label->setMidLineWidth(0);
        labelPalette.setColor( QPalette::Window, QColor("lightgrey") );
        label->setPalette( labelPalette );

        grid->addWidget( label, lastRow, lastColumn );
        lastColumn++;
        if(lastColumn > 19) {
            lastRow++;
            lastColumn = 0;
        }
        labels.append(label);
    }

    setLayout(grid);
}

QSize SmallQuestionEnumWidget::sizeHint() {
    uint w = qApp->desktop()->screen()->size().width();
    return QSize(w, 50);
}


void SmallQuestionEnumWidget::onSelectQuestion(const int number) {
    if(taskEnded) return;

    if(number != selectedNumber) {
        QLabel* label = labels[number]; // новый выбранный вопрос
        QPalette labelPalette = label->palette();
        label->setFrameStyle(QFrame::Panel|QFrame::Raised);
        label->setLineWidth(2);
        label->setMidLineWidth(0);

        bool questionAnswered = (answered.find(number) != answered.end());
        if(questionAnswered)
            labelPalette.setColor( QPalette::Window, QColor("darkgrey") );
        else
            labelPalette.setColor( QPalette::Window, QColor("yellow") );
        label->setPalette( labelPalette );

        if(selectedNumber != -1) {
            QLabel* label = labels[selectedNumber]; // прежний выбранный вопрос
            QPalette labelPalette = label->palette();
            label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            label->setLineWidth(1);
            label->setMidLineWidth(0);

            bool questionAnswered = (answered.find(selectedNumber) != answered.end());
            if(questionAnswered)
                labelPalette.setColor( QPalette::Window, QColor("grey") );
            else
                labelPalette.setColor( QPalette::Window, QColor("lightgrey") );
            label->setPalette( labelPalette );
        }
        selectedNumber = number;
    }

    show();
}


void SmallQuestionEnumWidget::onAnswerQuestion(const uint number) {
    answered.insert(number);
    hide();
}


void SmallQuestionEnumWidget::onExtraTask() {
    uint extraCount = DataBox::inst().getExtraCount();
    //uint firstExtNumber = DataBox::inst().getQuestions().size() - extraCount;
    for( uint n = 0; n < extraCount; ++n ) {
        QLabel* label = new QLabel("   ");
        label->setAutoFillBackground(true);
        QPalette labelPalette = label->palette();
        label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        label->setLineWidth(1);
        label->setMidLineWidth(0);
        labelPalette.setColor( QPalette::Window, QColor("lightgrey") );
        label->setPalette( labelPalette );
        grid->addWidget( label, lastRow, lastColumn );
        lastColumn++;
        if(lastColumn > 19) {
            lastRow++;
            lastColumn = 0;
        }
        labels.append(label);
    }
}


void SmallQuestionEnumWidget::onEndTask() {
    taskEnded = true;
    hide();
}
