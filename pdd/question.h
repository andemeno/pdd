#ifndef PDD_QUESTION_H
#define PDD_QUESTION_H

#include <QString>
#include <QStringList>

namespace pdd {

/**
 * Данные вопроса
 */
class Question {
public:
    Question();
    Question(/*const uint _id, */const uint group, const uint block);

    void setNumber(const uint n);
    uint getId() const;
    uint getNumber() const;
    uint getThemeNumber() const;
    void setThemeNumber( const uint n );
    uint getGroupNumber() const;
    void setGroupNumber(const uint n);
    uint getThemeBlockNumber() const;
    void setThemeBlockNumber(const uint n);
    QString getTask() const;
    void setTask( const QString& _task, bool onlyTask = false );
    QString getComment() const;
    void setComment( const QString& _comment );
    QString getImageName() const;
    void setImageName(const QString& name);
    uint getAnswer() const;
    void setAnswer( const uint _answer );
    QStringList getAnswers() const;
    void setAnswers(const QStringList& _answers);

private:

    uint id; /// Идентификатор вопроса в ресурсном файле
    uint themeNumber; /// Номер темы, к которой относится вопрос (начиная с 0)
    uint groupNumber; /// Номер группы, к которой отностится вопрос (от 0 до 3)
    uint blockNumber; /// Номер тематического блока, к которому относится вопрос (от 0 до 159)

    QString task; /// Задание (текст вопроса)
    QStringList answers; /// Варианты ответов
    QString comment; /// Комментарий (обЪяснение) к заданию
    QString imageName; /// Имя файла с иллюстрацией
    uint answer; /// Номер правильного ответа
};

}

#endif // PDD_QUESTION_H
