#include "question.h"

using namespace pdd;

Question::Question()
    : id(0)
    , themeNumber(0)
    , groupNumber(0)
    , blockNumber(0)
    , answer(0) {}


Question::Question(const uint group, const uint block)
    : id(0)
    , themeNumber(0)
    , groupNumber(group)
    , blockNumber(block)
    , answer(0) {}


void Question::setNumber(const uint n) {
    id = n + 217;
    imageName = QString("jpg%1.jpg").arg(n+1);
}

uint Question::getId() const {
    return id;
}


uint Question::getNumber() const {
    return id - 217;
}


uint Question::getThemeNumber() const {
    return themeNumber;
}

void Question::setThemeNumber( const uint n ) {
    themeNumber = n;
}


uint Question::getGroupNumber() const {
    return groupNumber;
}

void Question::setGroupNumber(const uint n) {
    groupNumber = n;
}

uint Question::getThemeBlockNumber() const {
    return blockNumber;
}


void Question::setThemeBlockNumber(const uint n) {
    blockNumber = n;
}

QString Question::getTask() const {
    return task;
}


void Question::setTask(const QString& _task, bool onlyTask/*=false*/) {

    if(onlyTask) {
        task = _task;
    } else {
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
}


QString Question::getComment() const {
    return comment;
}


void Question::setComment( const QString& _comment ) {
    comment = _comment;
    comment.replace( "\n", " " );
}


QString Question::getImageName() const {
    return imageName;
}

void Question::setImageName(const QString& name) {
    imageName = name;
}


uint Question::getAnswer() const {
    return answer;
}


void Question::setAnswer( const uint _answer ) {
    answer = _answer;
}


QStringList Question::getAnswers() const {
    return answers;
}

void Question::setAnswers(const QStringList& _answers) {
    answers = _answers;
}
