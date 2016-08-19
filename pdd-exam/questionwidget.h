#ifndef QUESTIONWIDGET_H
#define QUESTIONWIDGET_H

#include <QWidget>

class QPushButton;
class QLabel;

namespace pdd {

class AnswerWidget;

class QuestionWidget : public QWidget {
    Q_OBJECT
public:
    explicit QuestionWidget(const uint n, QWidget *parent = 0);

signals:
    void nextQuestion(uint nunber);
    void prevQuestion(uint number);
    void answerQuestion(uint number, uint answer);
    void showEnum();

public slots:
    void onNextButton();
    void onPrevButton();
    void onSelectAnswer(uint n);
    void onConfirmButton();
    void onEndTask();
    void onHelpButton();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    void makeGUI();

    uint number; /// Номер вопроса в билете
    int choise; /// Вариант ответа, выбранный экзаменуемым
    uint rightAnswer; /// Номер правильного ответа
    QList<AnswerWidget*> answers; /// Группа кнопок с вариантами ответов
    QPushButton* confirmButton; /// Кнопка подтверждения выбранного варианта ответа
    QLabel* helpLabel; /// Надпись-пояснение к вопросу
    QPushButton* helpButton; /// Кнопка вывода пояснения к вопросу
    QPushButton* prevButton; /// Кнопка для перехода к предыдущему вопросу
    QPushButton* enumButton; /// Кнопка для перехода к перечню  вопросов
    QPushButton* nextButton; /// Кнопка для перехода к следующему вопросу
    QLabel* resultLabel; /// Надпись с результатом ответа на вопрос

};

}

#endif // QUESTIONWIDGET_H
