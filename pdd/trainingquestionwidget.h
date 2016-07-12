#ifndef TRAININGQUESTIONWIDGET_H
#define TRAININGQUESTIONWIDGET_H

#include <QWidget>

class QLabel;
class QPushButton;

namespace pdd {

class AnswerWidget;

class TrainingQuestionWidget : public QWidget {
    Q_OBJECT
public:
    explicit TrainingQuestionWidget(const uint n, QWidget *parent = 0);

signals:
    void nextQuestion(uint n);
    void prevQuestion(uint n);
    void showEnum();
    void answerRight(uint n);
    void answerWrong(uint n);

public slots:
    void onSelectAnswer(const uint n);
    void onConfirmButton();
    void onNextButton();
    void onPrevButton();
    void onHelpButton();
private:

    void makeGUI();
    uint number;
    uint rightAnswer;
    uint choise;
    QList<AnswerWidget*> answers; /// Группа кнопок с вариантами ответов
    QPushButton* confirmButton; /// Кнопка подтверждения выбранного варианта ответа
    QLabel* helpLabel;
    QPushButton* helpButton; /// Кнопка вывода пояснения к вопросу
    QPushButton* prevButton; /// Кнопка для перехода к предыдущему вопросу
    QPushButton* enumButton; /// Кнопка для перехода к перечню  вопросов
    QPushButton* nextButton; /// Кнопка для перехода к следующему вопросу
    QLabel* resultLabel;

};

}

#endif // TRAININGQUESTIONWIDGET_H
