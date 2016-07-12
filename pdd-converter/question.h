#ifndef QUESTION_H
#define QUESTION_H

#include <QString>
#include <QStringList>
#include <vector>

struct Question {
    uint number; /// Порядковый номер вопроса в базе
    uint themeNumber; /// Номер темы, к которой относится вопрос (начиная с 0)
    QString task; /// Задание (текст вопроса)
    QStringList answers; /// Варианты ответов
    QString comment; /// Комментарий (обЪяснение) к заданию
    QString imageName; /// Имя файла с иллюстрацией
    uint answer; /// Номер правильного ответа

    void set_task_and_answers( const QString& _task ) {
        // Разбор строки _task, которая содержит текст вопроса и варианты ответов
        QString delim; delim.append( 0x0D ); delim.append( 0x0A );
        QStringList splited = _task.split( delim, QString::SkipEmptyParts);
        task = splited.at(0);
        for( int i = 1; i < splited.size(); i++ ) {
            if( splited.at(i).size() > 2 )
                answers.append( splited.at(i) );
        }
        // костыль
        if(answers.size() == 1)
            answer = 1;
    }

    // todo comment.replace( "\n", " " );
};

typedef std::vector<Question> Questions;

struct Theme {
    QString name;
    Questions questions;
};

typedef std::vector<Theme> Themes;

#endif // QUESTION_H
