#ifndef QUESTIONSENUMWIDGET_H
#define QUESTIONSENUMWIDGET_H

#include <QWidget>

class QLabel;
class QGridLayout;

namespace pdd {

// Основная паенль вопросов (сетка прямоугольников на весь экран)
class QuestionsEnumWidget : public QWidget {
    Q_OBJECT
public:
    explicit QuestionsEnumWidget(QWidget *parent = 0);

signals:
    void selectQuestion(int number);
    void answerQuestion(uint number, uint answer);

private slots:
    void onExtraTask();

private:

    void makeGUI();
    //QLabel* resultLabel;
    QGridLayout* gridLayout;
};


}

#endif // QUESTIONSENUMWIDGET_H
