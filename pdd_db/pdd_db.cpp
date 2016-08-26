#include "pdd_db.h"
#include <cassert>
#include <QDebug>
#include <QtSql>
#include <algorithm>
#include <stdexcept>

using namespace pdd;

pdd_db::pdd_db(const QString& db)
    : db_name(db) {}


bool pdd_db::load() {

    // Установка соединения с базой данных
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName(db_name);
    if(!sdb.open()) {
       qDebug() << db_name << " " << sdb.lastError().text();
       return false;
    }

    { // Чтение таблицы themes
        QSqlQuery selectThemes("SELECT * FROM themes_ab");
        if(!selectThemes.exec()) {
            qDebug() << selectThemes.lastError().text();
            return false;
        }
        while(selectThemes.next()) {
            themes << selectThemes.value(1).toString();
            questions_by_themes.push_back(std::vector<uint>(selectThemes.value(2).toUInt()));
        }
    }

    { // Чтение таблицы questions
        QSqlQuery sq("SELECT * FROM questions_ab");
        if(!sq.exec()) {
            qDebug() << sq.lastError().text();
            return false;
        }

        while(sq.next()) {
            question quest(sq.value(0).toUInt());
            quest.set_task(sq.value(1).toString());
            uint vars_count = sq.value(2).toUInt();
            quest.set_answer(sq.value(3).toUInt());
            quest.set_comment(sq.value(4).toString());
            for(uint a = 0; a < vars_count; ++a) {
                quest.add_answer(sq.value(a+5).toString());
            }
            quest.set_theme_number(sq.value(11).toUInt());
            quest.set_number(sq.value(12).toUInt());
            questions.insert(std::make_pair(quest.get_id(), quest));
            questions_by_themes[quest.get_theme_number()-1][quest.get_number()-1] = quest.get_id();
        }
    }

    { // Чтение таблицы quests_by_task_ab
        QSqlQuery sq("SELECT * FROM quests_by_task_ab");
        if(!sq.exec()) {
            qDebug() << sq.lastError().text();
            return false;
        }

        questions_by_tasks.resize(get_tasks_count()); // количество билетов (официальная версия ГИБДД)
        for(std::vector<std::vector<uint> >::iterator q = questions_by_tasks.begin(); q != questions_by_tasks.end(); ++q) {
            q->resize(get_questions_in_task_count()); // количество вопросов в билете (официальная версия ГИБДД)
        }
        while(sq.next()) {
            uint task_number = sq.value(0).toUInt();
            uint in_task_number = sq.value(1).toUInt();
            uint theme_number = sq.value(2).toUInt();
            uint in_theme_number = sq.value(3).toUInt();
            questions_by_tasks[task_number-1][in_task_number-1] = questions_by_themes[theme_number-1][in_theme_number-1];
        }
    }

    sdb.removeDatabase(db_name);

    return true;
}

uint pdd_db::get_themes_count() const {
    return themes.size();
}

QStringList pdd_db::get_theme_names() const {
    return themes;
}

QString pdd_db::get_theme_name(const uint n) const {
    assert(n > 0);
    assert(int(n) < themes.size());
    return themes[n-1];
}

uint pdd_db::get_questions_count(const uint theme_n) const {
    assert(theme_n > 0);
    return questions_by_themes[theme_n-1].size();
}

uint pdd_db::get_tasks_count() const {
    return 40;
}

uint pdd_db::get_questions_in_task_count() const {
    return 20;
}

const question& pdd_db::get_question(const uint id) const {
    assert(id > 0);
    QuestionsMap::const_iterator q = questions.find(id);
    if(q == questions.end())
        throw std::runtime_error("get_question(): unknownd id");
    return q->second;
}

question& pdd_db::get_question(const uint id) {
    assert(id > 0);
    QuestionsMap::iterator q = questions.find(id);
    if(q == questions.end())
        throw std::runtime_error("get_question(): unknownd id");
    return q->second;
}

const question& pdd_db::get_question(const uint theme_n, const uint n) const {
    assert(theme_n > 0);
    assert(theme_n <= questions_by_themes.size());
    assert(n > 0);
    assert(n <= questions_by_themes[theme_n].size());
    const uint id = questions_by_themes[theme_n-1][n-1];
    return get_question(id);
}

question& pdd_db::get_question(const uint theme_n, const uint n)  {
    assert(theme_n > 0);
    assert(n > 0);
    const uint id = questions_by_themes[theme_n-1][n-1];
    return get_question(id);
}

std::vector<uint> pdd_db::get_ids_of_block(const uint task_number, const uint start_number_in_task, const uint block_sz/* = 5*/) const {
    assert(task_number <= questions_by_tasks.size());
    assert(start_number_in_task + block_sz - 1 <= questions_by_tasks[task_number-1].size());

    std::vector<uint> result;
    const uint last_number_in_task = start_number_in_task + block_sz - 1;
    for(uint n = start_number_in_task; n <= last_number_in_task; ++n) {
        result.push_back(questions_by_tasks[task_number-1][n-1]);
    }

    return result;
}

void pdd_db::set_question(const question& q) {
    question& to_change = get_question(q.get_id());
    uint new_number = q.get_number();
    uint old_number = to_change.get_number();
    if(new_number != old_number) { // изменился номер вопроса в теме
        // необходимо поменять местами id вопросов в таблице questions_by_themes
        uint th_n = q.get_theme_number();
        uint id = questions_by_themes[th_n-1][new_number-1];
        question& q = get_question(id);
        q.set_number(old_number);
        changed_quests.push_back(id);
        std::swap(
            questions_by_themes[th_n-1][new_number-1],
            questions_by_themes[th_n-1][old_number-1]
        );
    }

    to_change = q;
    changed_quests.push_back(q.get_id());
}

void pdd_db::remove_question(const uint id) {
    question& q = get_question(id);
    const uint th_n = q.get_theme_number();
    questions.erase(id);
    std::vector<uint>& in_theme = questions_by_themes[th_n-1];
    in_theme.erase(std::remove(in_theme.begin(), in_theme.end(), id), in_theme.end());
    removed_quests.push_back(id);
}

void pdd_db::remove_question(const uint th_n, const uint n) {
    remove_question(questions_by_themes[th_n-1][n-1]);
}

bool pdd_db::save_changes() {
    // Установка соединения с базой данных
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName(db_name);
    if(!sdb.open()) {
       qDebug() << db_name << " " << sdb.lastError().text();
       return false;
    }

    // удаление removed_quests
    for(std::vector<uint>::iterator i = removed_quests.begin(); i != removed_quests.end(); ++i) {
        QString str_query = QString("DELETE FROM questions_ab WHERE rowid=%1;").arg(*i);
        QSqlQuery delete_query(str_query);
        if(!delete_query.exec())
            return false;
    }
    removed_quests.clear();

    // обновление changed_quests
    for(std::vector<uint>::iterator i = changed_quests.begin(); i != changed_quests.end(); ++i) {

        question& q = get_question(*i);
        QStringList answers = q.get_answers();
        uint vars_count = answers.size();
        for(uint v = vars_count; v < 5; ++v) {
            answers << "";
        }

        QStringList queries;
        queries << QString("UPDATE questions_ab SET task=\"%1\" WHERE id=%2;").arg(q.get_task()).arg(*i);
        queries << QString("UPDATE questions_ab SET vars_count=%1 WHERE id=%2;").arg(vars_count).arg(*i);
        queries << QString("UPDATE questions_ab SET right_ans=%1 WHERE id=%2;").arg(q.get_answer()).arg(*i);
        queries << QString("UPDATE questions_ab SET comment=\"%1\" WHERE id=%2;").arg(q.get_comment()).arg(*i);
        queries << QString("UPDATE questions_ab SET ans_v1=\"%1\" WHERE id=%2;").arg(answers[0]).arg(*i);
        queries << QString("UPDATE questions_ab SET ans_v2=\"%1\" WHERE id=%2;").arg(answers[1]).arg(*i);
        queries << QString("UPDATE questions_ab SET ans_v3=\"%1\" WHERE id=%2;").arg(answers[2]).arg(*i);
        queries << QString("UPDATE questions_ab SET ans_v4=\"%1\" WHERE id=%2;").arg(answers[3]).arg(*i);
        queries << QString("UPDATE questions_ab SET ans_v5=\"%1\" WHERE id=%2;").arg(answers[4]).arg(*i);
        //queries << QString("UPDATE questions_ab SET image=\"%1\" WHERE id=%2;").arg(q.get_image_name()).arg(*i);
        queries << QString("UPDATE questions_ab SET theme_num=%1 WHERE id=%2;").arg(q.get_theme_number()).arg(*i);
        queries << QString("UPDATE questions_ab SET in_theme_num=%1 WHERE id=%2;").arg(q.get_number()).arg(*i);

        foreach (QString str_query, queries) {
            QSqlQuery update_query;
            update_query.prepare(str_query);
            if(!update_query.exec()) {
                qDebug() << update_query.executedQuery() << ": " << update_query.lastError().text();
                return false;
            }
        }

    }
    changed_quests.clear();

    sdb.removeDatabase(db_name);

    // todo добавление added_quests
    //for(std::vector<uint>::iterator i = added_quests.begin(); i != added_quests.end(); ++i) {

    //}
    //added_quests.clear();

    return true;
}
