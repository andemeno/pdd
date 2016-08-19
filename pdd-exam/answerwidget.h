#ifndef ANSWERWIDGET_H
#define ANSWERWIDGET_H

#include <QLabel>

namespace pdd {

class AnswerWidget : public QLabel {
    Q_OBJECT
public:
    explicit AnswerWidget(const QString& cap, const uint n, const uint rn, QWidget *parent = 0);

signals:
    void select(uint n);

public slots:
    void onSelect(uint n);
    void onConfirm();
    void onEndTask();

protected:
    void mouseReleaseEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    void setBackground( QColor color );
    void setForeground( QColor color );
    uint number;
    uint rightNumber;
    bool selected;
    bool confirmed;
    QColor defaultWindowColor;
    QColor defaultWindowTextColor;
};

}

#endif // ANSWERWIDGET_H
