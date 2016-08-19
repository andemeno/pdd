#include "answerwidget.h"

using namespace pdd;

AnswerWidget::AnswerWidget(const QString &cap, const uint n, const uint rn, QWidget *parent)
    : QLabel(cap, parent)
    , number(n)
    , rightNumber(rn)
    , selected(false)
    , confirmed(false) {

    defaultWindowColor = palette().color( QPalette::Window );
    defaultWindowTextColor = palette().color( QPalette::WindowText );

    setAutoFillBackground(true);
    setWordWrap(true);
    setTextFormat(Qt::PlainText);
    setAlignment(Qt::AlignLeft);
}


void AnswerWidget::onSelect(uint n) {

    if(n == number) {
        setBackground( QColor("lightblue") );
        setForeground( QColor("black") );
        selected = true;

    } else {
        setBackground( defaultWindowColor );
        setForeground( defaultWindowTextColor );
        selected = false;
    }

    repaint();
}

void AnswerWidget::onConfirm() {

    confirmed = true;
    repaint();
}


void AnswerWidget::onEndTask() {

    if( rightNumber == number ) {
        if( confirmed ) {
            setBackground( QColor("lightgreen") );
            setForeground( QColor("black") );
        } else {
            setBackground( QColor("green") );
            setForeground( QColor("white") );
        }

    } else {
        if( selected && confirmed ) {
            setBackground( QColor("lightblue") );
            setForeground( QColor("black") );
        } else {
            setBackground( defaultWindowColor );
            setForeground( defaultWindowTextColor );
        }
    }

    confirmed = true;
    repaint();
}


void AnswerWidget::mouseReleaseEvent(QMouseEvent*) {
    if(!confirmed && !selected)
        emit select(number);
}


void AnswerWidget::enterEvent(QEvent *) {
    if(!confirmed && !selected) {
        setBackground( QColor("lightblue") );
        setForeground( QColor("black") );
        repaint();
    }
}


void AnswerWidget::leaveEvent(QEvent *) {
    if(!confirmed && !selected) {
        setBackground( defaultWindowColor );
        setForeground( defaultWindowTextColor );
        repaint();
    }
}


void AnswerWidget::setBackground( QColor color ) {
    QPalette pal = palette();
    pal.setColor( QPalette::Window, color );
    setPalette( pal );
}


void AnswerWidget::setForeground( QColor color ) {
    QPalette pal = palette();
    pal.setColor( QPalette::WindowText, color );
    setPalette( pal );
}
