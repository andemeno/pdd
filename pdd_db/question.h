#ifndef PDD_QUESTION_H
#define PDD_QUESTION_H

#include <QString>
#include <QStringList>

namespace pdd {

/**
 * Данные вопроса
 */
class question {
public:
    question();
    question(uint qid);

    bool operator == (const question& other) const;
    bool operator == (const uint other_id) const;

    uint get_id() const;
    void set_number(const uint n);
    uint get_number() const;
    uint get_theme_number() const;
    void set_theme_number(const uint n);
    QString get_task() const;
    void set_task(const QString& _task);
    QString get_comment() const;
    void set_comment(const QString& _comment);
    void set_image_name(const QString& name);
    QString get_image_name() const;
    uint get_answer() const;
    void set_answer(const uint _answer);
    QStringList get_answers() const;
    void set_answers(const QStringList& ans);
    void add_answer(const QString& answer_text);

private:

    uint id; /// уникальный идентификатор вопроса в базе данных
    uint number; /// Номер вопроса в теме (начиная с 1)
    uint theme_number; /// Номер темы (начиная с 1)
    QString task; /// Задание (текст вопроса)
    QStringList answers; /// Варианты ответов
    QString comment; /// Комментарий (обЪяснение) к заданию
    //QString image_name; /// Имя файла с иллюстрацией
    uint answer; /// Номер правильного ответа
};

}

#endif // PDD_QUESTION_H
