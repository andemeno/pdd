#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>

class QStackedWidget;
class QStackedLayout;
class QVBoxLayout;

namespace pdd {

class QuestionsEnumWidget;
class SmallQuestionEnumWidget;

class TaskWidget : public QWidget {
    Q_OBJECT
public:
    explicit TaskWidget(QWidget *parent = 0);

public slots:
    void onPrevQuestion(uint n);
    void onNextQuestion(uint n);
    void onEndTask();
    void showEnum();
    void onExtraTask();
    void onShowQuestionWidget(int n);

private:
    void makeGUI();

    QVBoxLayout* mainLayout;
    QStackedLayout* stacked;
    QuestionsEnumWidget* enumWidget;
    SmallQuestionEnumWidget* smallEnumWidget;
    bool taskEnded;
};

}

#endif // TASKWIDGET_H
