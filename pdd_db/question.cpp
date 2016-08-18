#include "question.h"

using namespace pdd;

question::question()
    : id(0)
    , number(0)
    , theme_number(0)
    , answer(0) {}

question::question(uint qid)
    : id(qid)
    , number(0)
    , theme_number(0)
    , answer(0) {}

bool question::operator == (const question& other) const {
    return other.id == id;
}

bool question::operator == (const uint other_id) const {
    return other_id == id;
}

uint question::get_id() const {
    return id;
}

void question::set_number(const uint n) {
    number = n;
}

uint question::get_number() const {
    return number;
}

uint question::get_theme_number() const {
    return theme_number;
}

void question::set_theme_number( const uint n ) {
    theme_number = n;
}

QString question::get_task() const {
    return task;
}

void question::set_task(const QString& _task) {
    task = _task;
    task.replace('\"', '\'');
}

QString question::get_comment() const {
    return comment;
}

void question::set_comment( const QString& _comment ) {
    comment = _comment;
    comment.replace('\"', '\'');
}

void question::set_image_name(const QString& name) {
    //image_name = name;
}

QString question::get_image_name() const {
    //return image_name;
    return QString("%1-%2.jpg").arg(theme_number).arg(number);
}

uint question::get_answer() const {
    return answer;
}

void question::set_answer( const uint _answer ) {
    answer = _answer;
}

QStringList question::get_answers() const {
    return answers;
}

void question::set_answers(const QStringList& ans) {
    answers.clear();
    foreach(QString a, ans) {
        a.replace('\"', '\'');
        answers << a;
    }
}

void question::add_answer(const QString& answer_text) {
    answers.push_back(answer_text);
}
