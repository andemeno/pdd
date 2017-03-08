#ifndef TRAININGWIDGET_H
#define TRAININGWIDGET_H

#include <QWidget>
class QButtonGroup;
class QStackedLayout;

namespace pdd {

class TrainingWidget : public QWidget {
    Q_OBJECT
public:
    explicit TrainingWidget(QWidget *parent = 0);

signals:
    void show_help();

public slots:
    void onPrevQuestion(const uint n);
    void onNextQuestion(uint n);
    void showEnum();

private:
    void makeGUI();

    QStackedLayout* stacked;

};

class EnumLabelWidget : public QWidget {
    Q_OBJECT
public:
    explicit EnumLabelWidget(QWidget *parent = 0);
signals:
    void selectQuestion(int n);
public slots:
    void onAnswerWrong(uint n);
    void onAnswerRight(uint n);
private:
    QButtonGroup* group;
};

}

#endif // TRAININGWIDGET_H
