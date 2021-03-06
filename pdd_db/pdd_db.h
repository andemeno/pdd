#ifndef PDD_DB_H
#define PDD_DB_H

#include <QStringList>
#include <vector>
#include <map>
#include "question.h"

namespace pdd {

class pdd_db {
public:
    pdd_db(const QString& db);

    bool load();

    uint get_themes_count() const;
    QStringList get_theme_names() const;
    QString get_theme_name(const uint n) const;
    uint get_questions_count(const uint theme_n) const;

    uint get_tasks_count() const;
    uint get_questions_in_task_count() const;

    const question& get_question(const uint id) const;
    question& get_question(const uint id);
    const question& get_question(const uint theme_n, const uint n) const;
    question& get_question(const uint theme_n, const uint n);
    std::vector<uint> get_ids_of_block(const uint task_number, const uint start_number_in_task, const uint block_sz = 5) const;
    void set_question(const question& q);
    void remove_question(const uint id);
    void remove_question(const uint th_n, const uint n);

    bool save_changes();
private:

    typedef std::map<uint, question> QuestionsMap;
    QString db_name;

    QStringList themes;
    QuestionsMap questions;
    std::vector<std::vector<uint> > questions_by_themes;
    std::vector<std::vector<uint> > questions_by_tasks;
    std::vector<uint> changed_quests;
    std::vector<uint> removed_quests;
    std::vector<uint> added_quests;
};

}

#endif // PDD_DB_H
