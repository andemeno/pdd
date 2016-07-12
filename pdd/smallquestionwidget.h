#ifndef SMALLQUESTIONWIDGET_H
#define SMALLQUESTIONWIDGET_H

#include <QWidget>

namespace pdd {

// Панель квадратов
class SmallQuestionWidget : public QWidget {
    Q_OBJECT
public:
    explicit SmallQuestionWidget(const uint num, QWidget *parent = 0);

signals:
    void selectQuestion(int n);

public slots:
    void onAnswerQuestion(uint n, uint answer);
    void onEndTask();

protected:
    void mouseReleaseEvent( QMouseEvent* );
    void paintEvent( QPaintEvent* );
    void enterEvent( QEvent* );
    void leaveEvent( QEvent* );

private:
    void setColor( const QColor& c );
    uint number;
    int userAnswer;
    int rightAnswer;
    QString name;
    QString result;
    QPixmap image;
    QColor color;
    QColor resultColor;
    bool taskEnded;
};

}

#endif // SMALLQUESTIONWIDGET_H
