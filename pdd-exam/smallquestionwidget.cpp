#include "smallquestionwidget.h"
#include "databox.h"
#include <QLabel>
#include <QPainter>
#include "programconfig.h"

using namespace pdd;

SmallQuestionWidget::SmallQuestionWidget(const uint num, const bool is_themes_test, QWidget *parent)
    : QWidget(parent)
    , number( num )
    , themes_test(is_themes_test)
    , userAnswer(-1)
    , rightAnswer( DataBox::inst().getTaskQuestion(number).get_answer() )
    , name(DataBox::inst().getTaskQuestion(number).get_task().left(20))
    , image(QPixmap(QString("%1%2").arg(Config::inst().abPathToImages.c_str()).arg(DataBox::inst().getTaskQuestion(number).get_image_name())))
    , color( "lightgrey" )
    , resultColor( "white" )
    , taskEnded( false ) {

    if(themes_test) {
        name = QString("%1.%2").arg(
                    DataBox::inst().getTaskQuestion(number).get_theme_number()).arg(
                    DataBox::inst().getTaskQuestion(number).get_number());
    } else {
        name.append( "..." );
    }

    DataBox& box = DataBox::inst();
    connect( &box, SIGNAL(failTask()), this, SLOT(onEndTask()) );
    connect( &box, SIGNAL(successTask()), this, SLOT(onEndTask()) );
}


void SmallQuestionWidget::onAnswerQuestion(uint n, uint answer) {
    if( n == number ) {
        userAnswer = answer;
        setColor("grey");
    }
}


void SmallQuestionWidget::onEndTask() {
    taskEnded = true;

    if( rightAnswer == userAnswer ) {
        resultColor = QColor("green");
        result = "ОТВЕТ ПРАВИЛЬНЫЙ";
    } else if(userAnswer > 0) {
        resultColor = QColor("red");
        result = "ОТВЕТ НЕПРАВИЛЬНЫЙ";
    } else {
        resultColor = QColor("white");
        result = "НЕТ ОТВЕТА";
    }
    setColor("lightgrey");
}


void SmallQuestionWidget::setColor( const QColor& c ) {
    color = c;
    repaint();
}


void SmallQuestionWidget::mouseReleaseEvent( QMouseEvent* ) {
    emit selectQuestion( number );
}


void SmallQuestionWidget::paintEvent( QPaintEvent* ) {

    uint swidth = width()-1;
    uint sheight = height()-1;

    QPainter painter(this);
    QRect rect(QRect(0, 0, swidth, sheight));
    painter.fillRect( rect, color );
    painter.drawRect( rect );

    uint offset_h = 0.1 * sheight;
    uint offset_w = 0.1 * swidth;
    uint text_spacing = painter.fontMetrics().lineSpacing()+2*offset_h;

    QRect imageRect( offset_w, offset_h, swidth-2*offset_w, height()-text_spacing-offset_h);
    if(!themes_test) {
        painter.drawPixmap( imageRect, image );
    }

    QRect nameRect( 0, sheight-text_spacing, swidth, text_spacing );
    painter.drawText( nameRect, Qt::AlignCenter, name );

    if( taskEnded ) {
        QRect resultRect = imageRect;
        painter.setBackgroundMode( Qt::OpaqueMode );
        painter.setBackground( resultColor );
        painter.drawText( resultRect, Qt::AlignCenter, result );
    }
}


void SmallQuestionWidget::enterEvent( QEvent* ) {

    if(!taskEnded)
        setColor( userAnswer>0 ? "grey" : "yellow" );
    else
        setColor( "yellow" );
}


void SmallQuestionWidget::leaveEvent( QEvent* ) {

    if(!taskEnded)
        setColor( userAnswer>0 ? "grey" : "lightgray" );
    else
        setColor( "lightgray" );
}

