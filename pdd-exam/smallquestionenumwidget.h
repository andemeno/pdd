#ifndef SMALLQUESTIONENUMWIDGET_H
#define SMALLQUESTIONENUMWIDGET_H

#include <set>
#include <QWidget>

class QLabel;
class QGridLayout;

namespace pdd {

class SmallQuestionEnumWidget : public QWidget {
    Q_OBJECT
public:
    explicit SmallQuestionEnumWidget(QWidget *parent = 0);
    QSize sizeHint();

public slots:

    void onSelectQuestion(const int number);
    void onAnswerQuestion(const uint number);
    void onExtraTask();
    void onEndTask();

private:
    QGridLayout* grid;
    QList<QLabel*> labels;
    std::set<uint> answered; // множество номеров вопросов, на которое даны ответы
    bool taskEnded; // признак завершенного экзамена
    int selectedNumber; // Номер выбранного вопроса
    int lastRow;
    int lastColumn;
};

}

#endif // SMALLQUESTIONENUMWIDGET_H
