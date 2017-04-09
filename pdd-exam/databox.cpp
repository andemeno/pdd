#include "databox.h"
#include "programconfig.h"
#include <pdd_db.h>
#include <QTime>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <time.h>

#include <iostream>

using namespace pdd;

DataBox::DataBox(QObject *parent)
    : QObject(parent)
    , totalAnswers(0)
    , totalExtraQuestions(0)
    , timer(this)
    , remainingTaskTime(0)
    , elapsedTaskTime(0)
    , totalElapsedTaskTime(0) {

    connect( &timer, SIGNAL(timeout()), this, SLOT(onTimeoutTask()) );
    connect( this, SIGNAL(extraTask(uint)), this, SLOT(initExtraTask()) );
}


DataBox::Category::Category()
    : groupsCount(4)
    , themeBlocksCount(40)
    , questionsInThemeBlock(5)
    , loaded(false) {
}

bool DataBox::Category::load_from_sql(const std::string& dbName) {

    // Загрузка вопросов и тем из базы
    doc.reset(new pdd_db(dbName.c_str()));
    loaded = doc->load();
    if(!loaded) return false;

    groups.clear();
    groups.resize(groupsCount);
    const uint tasks_count = doc->get_tasks_count();
    for(uint gn = 0; gn < groupsCount; ++gn) {
        groups[gn].resize(themeBlocksCount);
        const uint start_number_in_task = gn*questionsInThemeBlock + 1;
        for(uint task_number = 1; task_number <= tasks_count; ++task_number) {
            groups[gn][task_number-1] = doc->get_ids_of_block(task_number, start_number_in_task, questionsInThemeBlock);
        }
    }

    return loaded;
}

bool DataBox::load() {
    return ab.load_from_sql(Config::inst().databaseName);
}

std::pair<uint, uint> DataBox::get_block_group_pair(const uint qid) const {
    std::pair<uint, uint> result(0, 0);
    for(group_array::const_iterator g = ab.groups.begin(); g != ab.groups.end(); ++g) {
        result.second++;

        for(group::const_iterator b = g->begin(); b != g->end(); ++b) {
            result.first++;
            const theme_block& block = *b;
            if(std::find(block.begin(), block.end(), qid) != block.end())
                return result;
        }

    }

    QString err_msg = QString("DataBox::get_block_pair() for unexpected question id = %1").arg(qid);
    throw std::runtime_error(err_msg.toStdString());
    return std::pair<uint, uint>(0, 0); // nether return
}


void DataBox::startTask() {
    if(totalExtraQuestions > 0) {
        remainingTaskTime = timer.interval();
        totalElapsedTaskTime += elapsedTaskTime;
        elapsedTaskTime = 0;
    } else {
        remainingTaskTime = timer.interval();
        totalElapsedTaskTime = 0;
        elapsedTaskTime = 0;
        emit taskStarted();
    }
    timer.start();
}

uint DataBox::themesCount() const {
    if(!ab.loaded) return 0;
    return ab.doc->get_themes_count();
}


QStringList DataBox::getThemes() const {
    if(!ab.loaded) return QStringList();
    return ab.doc->get_theme_names();
}


QString DataBox::getTheme(const uint themeNumber) const {
    if(!ab.loaded) return "";
    return ab.doc->get_theme_name(themeNumber+1);
}


void DataBox::initCommonTask(const uint count) {
    Category* category = &ab;
    if(!category->loaded) return;

    std::srand(time(NULL));

    uint factor = count / 20;
    if( factor == 1 )
        timer.setInterval( 20*60*1000 );
    if( factor == 2 )
        timer.setInterval( 30*60*1000 );

    task.clear();
    errors.clear();
    totalAnswers = 0;
    totalExtraQuestions = 0;

    std::vector<uint> nums;

    for( uint f = 0; f < factor; ++f ) {
        for( uint g = 0; g < category->groupsCount; ++g ) {
            uint n = std::rand() % category->themeBlocksCount;
            if(f > 0) {
                n = nums[g];
                while(n == nums[g])
                    n = std::rand() % category->themeBlocksCount;
            } else {
                nums.push_back(n);
            }

            const theme_block& block = category->groups[g][n];
            for(theme_block::const_iterator i = block.begin(); i != block.end(); ++i) {
                task.push_back(task_question(*i));
            }
        }
    }
}


void DataBox::initExtraTask() {
    std::srand(time(NULL));
    Category* category = &ab;

    totalExtraQuestions = 0;

    for(std::map<uint,uint>::iterator e = errors.begin(); e != errors.end(); ++e) {

        uint tb_n = e->first;
        while(tb_n == e->first)
            tb_n = (std::rand() % category->themeBlocksCount) + 1;
        const uint g_n = e->second;

        const theme_block& block = category->groups[g_n-1][tb_n-1];
        for(theme_block::const_iterator i = block.begin(); i != block.end(); ++i) {
            task.push_back(task_question(*i));
        }

        totalExtraQuestions += block.size();
    }

    timer.setInterval( totalExtraQuestions*60*1000 );
    startTask();
}


void DataBox::initByTheme(const uint themeNumber) {
    Category* category = &ab;
    if(!category->loaded) return;

    task.clear();
    errors.clear();
    totalAnswers = 0;
    totalExtraQuestions = 0;

    const uint quest_count = category->doc->get_questions_count(themeNumber+1);
    for(uint n = 1; n <= quest_count; ++n) {
        task.push_back(category->doc->get_question(themeNumber+1, n).get_id());
    }
}

void DataBox::initQuestionBlock(const uint th_n, const uint quest_n, const uint count) {
    Category* category = &ab;
    if(!category->loaded) return;

    task.clear();
    errors.clear();
    totalAnswers = 0;
    totalExtraQuestions = 0;
    all_answers_right = true;

    uint total_count = 0;
    uint tn = th_n + 1;
    uint qn = quest_n + 1;

    do {

        const uint quest_count = category->doc->get_questions_count(tn);
        for(uint n = qn; n <= quest_count; ++n) {
            task.push_back(category->doc->get_question(tn, n).get_id());
            ++total_count;
            if(total_count == count)
                break;
        }
        ++tn;
        qn = 1;

    } while(total_count < count);

}

uint DataBox::getTaskQuestionsCount() const {
    return task.size();
}

std::vector<uint> DataBox::getTaskQuestions() const {
    std::vector<uint> result;
    for(std::vector<task_question>::const_iterator t = task.begin(); t != task.end(); ++t) {
        result.push_back(t->qid);
    }
    return result;
}

const question& DataBox::getTaskQuestion(const uint number) const {
    if(!ab.loaded)
        throw std::runtime_error("db is not loaded");

    if(number >= task.size())
        throw std::runtime_error(QString("Incorrect question index=%1").arg(number).toStdString());

    return ab.doc->get_question(task[number].qid);
}

const question& DataBox::getQuestion(const uint id) const {
    if(!ab.loaded)
        throw std::runtime_error("db is not loaded");

    return ab.doc->get_question(id);
}

const question& DataBox::getQuestion(const uint th, const uint n) const {
    if(!ab.loaded)
        throw std::runtime_error("db is not loaded");

    return ab.doc->get_question(th, n);
}


bool DataBox::checkAnswer( const uint number ) const {
    if(number >= task.size())
        throw std::runtime_error(QString("Incorrect question index=%1").arg(number).toStdString());

    uint right_answer = ab.doc->get_question(task[number].qid).get_answer();
    int user_answer = task[number].answer;

    return int(right_answer) == user_answer;
}


bool DataBox::wrongAnswered(const uint number) const {
    if(number >= task.size())
        throw std::runtime_error(QString("Incorrect question index=%1").arg(number).toStdString());

    if(checkAnswer(number))
        return false;
    return task[number].answer > 0;
}


int DataBox::getAnswer(const uint number) const {
    if(number >= task.size())
        throw std::runtime_error(QString("Incorrect question index=%1").arg(number).toStdString());

    return task[number].answer;
}


void DataBox::setAnswer(const uint number, const uint answer) {
    if(number >= task.size())
        throw std::runtime_error(QString("Incorrect question index=%1").arg(number).toStdString());

    task[number].answer = answer;
    ++totalAnswers;

    uint right_answer = ab.doc->get_question(task[number].qid).get_answer();

    emit questionAnswered(number, task[number].qid, task[number].answer, right_answer);

    if(Config::inst().extraOff == 0) { // Обычный режим с доп вопросами

        if(answer != right_answer) { // ответ неправильный

            if(totalExtraQuestions > 0) {
                timer.stop();
                emit failTask(); // допущена ошибка при ответе на вопрос дополнительного блока
                return;
            }

            std::pair<std::map<uint, uint>::iterator, bool> result = errors.insert(get_block_group_pair(task[number].qid));

            if( !result.second ) {
                emit failTask(); // допущены две ошибки в одном тематическом блоке
                timer.stop();
                return;
            }
            if( errors.size() == 3 ) {
                emit failTask(); // допущены три ошибки
                timer.stop();
                return;
            }
        }

        if( totalAnswers == task.size() ) { // даны ответы на все вопросы
            timer.stop();

            if( totalExtraQuestions > 0 ) {
                emit successTask(); // даны правильные ответы на все вопросы дополнительного блока
                return;
            }

            if( errors.size() == 0 )
                emit successTask(); // не допущено ни одной ошибки
            else
                emit extraTask(ab.questionsInThemeBlock*errors.size()); // допущены ошибки предусматривающие доп.вопросы
        }

    } else { // extraOff > 0
        if(answer != right_answer) { // ответ неправильный

            std::pair<std::map<uint, uint>::iterator, bool> result = errors.insert(get_block_group_pair(task[number].qid));

            if( !result.second ) {
                emit failTask(); // допущены две ошибки в одном тематическом блоке
                timer.stop();
                return;
            }
            if( errors.size() == 3 ) {
                emit failTask(); // допущены три ошибки
                timer.stop();
                return;
            }
        }

        if( totalAnswers == task.size() ) { // даны ответы на все вопросы
            timer.stop();
            emit successTask();
        }
    }
}

void DataBox::setAnswerOnThemes(const uint number, const uint answer) {
    if(number >= task.size())
        throw std::runtime_error(QString("Incorrect question index=%1").arg(number).toStdString());

    task[number].answer = answer;
    ++totalAnswers;

    uint right_answer = ab.doc->get_question(task[number].qid).get_answer();
    if(right_answer != answer)
        all_answers_right = false;
    emit questionAnswered(number, task[number].qid, task[number].answer, right_answer);

    if(task.size() == totalAnswers) { // вопросы закончились
        // критериев оценки нет, поэтому если нет ни одной ошибки, то успех
        if(all_answers_right)
            emit successTask();
        else // в остальных случаях - провал
            emit failTask();
    }
}


void DataBox::onTimeoutTask() {

    timer.stop();

    if(Config::inst().extraOff == 0) {
        uint n0 = totalExtraQuestions > 0 ? task.size() - totalExtraQuestions : 0;
        for( uint n = n0; n < task.size(); ++n ) {

            if( task[n].answer == -1 ) {

                if(totalExtraQuestions > 0) {
                    emit failTask(); // не дан ответ на вопрос дополнительного блока
                    return;
                }

                std::pair<std::map<uint, uint>::iterator, bool> result = errors.insert(get_block_group_pair(task[n].qid));

                if( !result.second ) {
                    emit failTask(); // допущены две ошибки и/или пропуск в вопросах одного тематического блока
                    return;
                }
                if( errors.size() == 3 ) {
                    emit failTask(); // допущены три ошибки и/или пропуск
                    return;
                }
            }
        }

        if( totalExtraQuestions > 0 ) { // после дополнительного блока
            emit successTask();

        } else { // после основного билета

            if( errors.size() == 0 )
                emit successTask(); // не допущено ни одной ошибки
            else {
                totalAnswers = task.size();
                emit extraTask(ab.questionsInThemeBlock*errors.size());
            }
        }

    } else { // extraOff > 0

        for( uint n = 0; n < task.size(); ++n ) {
            if( task[n].answer == -1 ) {
                std::pair<std::map<uint, uint>::iterator, bool> result = errors.insert(get_block_group_pair(task[n].qid));

                if( !result.second ) {
                    emit failTask(); // допущены две ошибки и/или пропуск в вопросах одного тематического блока
                    return;
                }
                if( errors.size() == 3 ) {
                    emit failTask(); // допущены три ошибки и/или пропуск
                    return;
                }
            }
        }

        emit successTask();
    }
}

uint DataBox::getExtraCount() const {
    return totalExtraQuestions;
}

QTime DataBox::getTaskRemainingTime() {
    remainingTaskTime = timer.remainingTime();
    elapsedTaskTime = timer.interval() - remainingTaskTime;
    return QTime(0,0,0,0).addMSecs( remainingTaskTime );
}

QTime DataBox::getTaskElapsedTime() const {
    return QTime(0,0,0,0).addMSecs( totalElapsedTaskTime + elapsedTaskTime );
}

DataBox& DataBox::inst() {
    static DataBox instance;
    return instance;
}
