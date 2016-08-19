#include "smallquestionwidget.h"
#include "databox.h"
#include <QLabel>
#include <QPainter>

using namespace pdd;

SmallQuestionWidget::SmallQuestionWidget(const uint num, QWidget *parent)
    : QWidget(parent)
    , number( num )
    , userAnswer(-1)
    , rightAnswer( DataBox::inst().getQuestion(number).getAnswer() )
    , name(DataBox::inst().getQuestion(number).getTask().left(20))
    , image( DataBox::inst().getImage(number) )
    , color( "lightgrey" )
    , resultColor( "white" )
    , taskEnded( false ) {

    name.append( "..." );

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
    painter.drawPixmap( imageRect, image );

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

