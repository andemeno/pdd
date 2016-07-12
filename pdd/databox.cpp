#include "databox.h"
#include <pe_base.h>
#include <pe_factory.h>
#include <pe_resources.h>
#include <pe_resource_viewer.h>
#include "quazip/quazip.h"
#include <quazip/quazipfile.h>
#include "programconfig.h"
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <time.h>

#include <iostream>
#include <QtSql>

using namespace pdd;
using namespace pe_bliss;

DataBox::DataBox(QObject *parent)
    : QObject(parent)
    , categoryType(category_none)
    , totalAnswers(0)
    , totalExtraQuestions(0)
    , timer(this)
    , remainingTaskTime(0)
    , elapsedTaskTime(0)
    , totalElapsedTaskTime(0) {

    if(!load())
        throw std::runtime_error( "Error on load pdd_resources." );

    connect( &timer, SIGNAL(timeout()), this, SLOT(onTimeoutTask()) );
    connect( this, SIGNAL(extraTask(uint)), this, SLOT(initExtraTask()) );
}


DataBox::Category::Category()
    : groupsCount(4)
    , themeBlocksCount(40)
    , questionsInThemeBlock(5)
    , groups( groupsCount ) {

    uint id = 217;
    uint groupNumber = 0;
    uint themeBlockNumber = 0;
    for( Groups::iterator g = groups.begin(); g != groups.end(); ++g ) {
        QuestionsGroup& group = *g;
        group.resize(themeBlocksCount);
        for( QuestionsGroup::iterator b = group.begin(); b != group.end(); ++b ) {
            Questions& block = *b;
            block.resize(questionsInThemeBlock);
            for( Questions::iterator q = block.begin(); q != block.end(); ++q ) {
                Question& question = *q;
                question = Question(groupNumber, themeBlockNumber);
                id++;
            }
            ++themeBlockNumber;
        }
        ++groupNumber;
    }
}


bool DataBox::Category::load_from_pe(const std::string& mainDll, const std::string& dllComment) {

    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");

    //Открываем файлы
    std::ifstream peMain( mainDll.c_str(), std::ios::in | std::ios::binary );
    if( !peMain )
        return false;

    //Открываем файл
    std::ifstream peComment( dllComment.c_str(), std::ios::in | std::ios::binary );
    if( !peComment )
        return false;

    try {
        //Создаем экземпляр PE или PE+ класса с помощью фабрики
        pe_base imageMain( pe_factory::create_pe(peMain) );
        //Проверим, есть ли ресурсы у файла
        if(!imageMain.has_resources())
            return false;
        //Получаем корневую директорию ресурсов
        const resource_directory rootMain(get_resources(imageMain));
        pe_resource_viewer resMain(rootMain);

        //Создаем экземпляр PE или PE+ класса с помощью фабрики
        pe_base imageComment( pe_factory::create_pe(peComment) );
        //Проверим, есть ли ресурсы у файла
        if(!imageComment.has_resources())
            return false;
        //Получаем корневую директорию ресурсов
        const resource_directory rootComment(get_resources(imageComment));
        pe_resource_viewer resComment(rootComment);

        // Правильные ответы по номерам вопросов
        std::map<uint, uint> ans;
        {
            resource_data_info info("", 0);
            if(resMain.get_language_count(L"OTV", 1018) > 1)
                info = resMain.get_resource_data_by_id( 0, L"OTV", 1018 );
            else
                info = resMain.get_resource_data_by_id( 1049, L"OTV", 1018 );

            std::string raw(info.get_data());
            for(unsigned int i = 4; i < raw.size(); i+=2 ) {
                uint16_t answer = raw[i] + (raw[i+1]<<8);
                unsigned int n = i/2 - 2;
                ans.insert( std::make_pair( n, answer ));
            }
        }

        // Вопросы по темам (номер вопроса - номер темы)
        //std::map<uint, uint> byThemes;
        {
            // Количество тем
            resource_data_info info = resMain.get_resource_data_by_id( 1049, L"COUNTTEM", 1021 );
            std::string raw( info.get_data() );
            uint totalThemes = raw[0] + (raw[1] << 8);

            uint groupNumber = 0;
            uint themeBlockNumber = 0;
            uint questionInBlockNumber = 0;
            for( uint t = 0; t < totalThemes; ++t ) {
                resource_data_info info("", 0);
                if( resMain.get_language_count(L"POTEMAM", 1022+t) > 1 )
                    info = resMain.get_resource_data_by_id( 0, L"POTEMAM", 1022+t );
                else
                    info = resMain.get_resource_data_by_id( 1049, L"POTEMAM", 1022+t );

                std::string raw( info.get_data() );
                uint32_t questionsInTheme = (raw.size() - 4)/6; // количество вопросов в теме

                for(unsigned int i = 0; i < questionsInTheme; ++i) {
                    uint16_t n = 20*(raw[4 + i*6] - 1) + raw[6 + i*6] - 1;
                    //byThemes.insert(std::make_pair(n, t));

                    Question& question = groups[groupNumber][themeBlockNumber][questionInBlockNumber];
                    question.setNumber(n);
                    question.setThemeNumber(t);

                    // правильный ответ
                    std::map<uint,uint>::iterator a = ans.find(n);
                    if(a != ans.end())
                        question.setAnswer(a->second);


                    // задание и варианты ответов
                    {
                        resource_data_info info( "", 0 );
                        if( resMain.get_language_count( L"TEXT", question.getId() ) > 1 )
                            info = resMain.get_resource_data_by_id( 0, L"TEXT", question.getId() );
                        else
                            info = resMain.get_resource_data_by_id( 1049, L"TEXT", question.getId() );

                        std::string raw( info.get_data() );
                        question.setTask( codec->toUnicode(raw.c_str()) );
                    }


                    // комментарий
                    {
                        resource_data_info info( "", 0 );
                        if( resComment.get_language_count( L"TEXT", question.getId() ) > 1 )
                            info = resComment.get_resource_data_by_id( 0, L"TEXT", question.getId() );
                        else
                            info = resComment.get_resource_data_by_id( 1049, L"TEXT", question.getId() );

                        std::string raw( info.get_data() );
                        question.setComment( codec->toUnicode(raw.c_str()) );
                    }

                    questionInBlockNumber++;
                    if(questionInBlockNumber == questionsInThemeBlock) {
                        questionInBlockNumber = 0;
                        themeBlockNumber++;
                        if(themeBlockNumber == themeBlocksCount) {
                            themeBlockNumber = 0;
                            groupNumber++;
                            if(groupNumber == groupsCount)
                                break;
                        }
                    }
                }

                {
                    resource_data_info info = resMain.get_resource_data_by_id(1049, L"THEME_NAME", 1050 + t);
                    themes.push_back( codec->toUnicode(info.get_data().c_str()) );
                }
            }
        }

    } catch( const pe_exception& e ) { //Если возникла ошибка
        return false;
    }

    return true;
}

bool DataBox::Category::load_from_sql(const std::string& dbName, const QString category_suffix) {
    groups.clear();

    // Установка соединения с базой данных
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName(dbName.c_str());
    if(!sdb.open()) {
       qDebug() << dbName.c_str() << " " << sdb.lastError().text();
       return false;
    }

    // Чтение таблицы themes
    QSqlQuery selectThemes(QString("SELECT * FROM themes_%1").arg(category_suffix));
    if(!selectThemes.exec()) {
        qDebug() << selectThemes.lastError().text();
        return false;
    }
    while(selectThemes.next()) {
        themes.push_back(selectThemes.value(1).toString());
    }

    // Чтение таблицы constants
    QSqlQuery selectConstants("SELECT * FROM constants");
    if(!selectConstants.exec()) {
        qDebug() << selectConstants.lastError().text();
        return false;
    }

    while(selectConstants.next()) {
        groupsCount = selectConstants.value(1).toUInt(); // Количество групп
        themeBlocksCount = selectConstants.value(2).toUInt(); // Количество тематических блоков в группе
        questionsInThemeBlock = selectConstants.value(3).toUInt(); // Количество вопросов в тематическом блоке
    }
    groups.resize(groupsCount);
    uint groupNumber = 0;
    uint themeBlockNumber = 0;
    for( Groups::iterator g = groups.begin(); g != groups.end(); ++g ) {
        QuestionsGroup& group = *g;
        group.resize(themeBlocksCount);
        for( QuestionsGroup::iterator b = group.begin(); b != group.end(); ++b ) {
            Questions& block = *b;
            block.resize(questionsInThemeBlock);
            for( Questions::iterator q = block.begin(); q != block.end(); ++q ) {
                Question& question = *q;
            }
            ++themeBlockNumber;
        }
        ++groupNumber;
    }

    // Чтение таблицы questions
    QSqlQuery sq(QString("SELECT * FROM questions_%1").arg(category_suffix));
    if(!sq.exec()) {
        qDebug() << sq.lastError().text();
        return false;
    }

    groupNumber = 0;
    themeBlockNumber = 0;
    uint inBlockNumber = 0;
    while(sq.next()) {
        Question& question = groups[groupNumber][themeBlockNumber][inBlockNumber];
        uint index = sq.value(0).toUInt() - 1;
        question.setNumber(index + 1);
        question.setTask(sq.value(1).toString(), true);
        uint vars_count = sq.value(2).toUInt();
        question.setAnswer(sq.value(3).toUInt());
        question.setComment(sq.value(4).toString());
        QStringList answers;
        for(uint a = 0; a < vars_count; ++a) {
            answers += sq.value(a+5).toString();
        }
        question.setAnswers(answers);
        question.setImageName(sq.value(10).toString());
        question.setThemeNumber(sq.value(11).toUInt()-1);
        question.setGroupNumber(groupNumber);
        question.setThemeBlockNumber(themeBlockNumber);

        inBlockNumber++;
        if(inBlockNumber == questionsInThemeBlock) {
            inBlockNumber = 0;
            themeBlockNumber++;
            if(themeBlockNumber == themeBlocksCount) {
                themeBlockNumber = 0;
                groupNumber++;
                if(groupNumber == groupsCount) break;
            }
        }
    }

    return true;
}


std::vector<QPixmap> DataBox::initPixmaps(Questions& quests) {

    std::vector<QPixmap> pix;

    if(Config::inst().useCompressedImages) {
        QString zipName;
        if( categoryType == category_ab ) zipName = Config::inst().abImageArchiveName.c_str();
        else if( categoryType == category_cd ) zipName = Config::inst().cdImageArchiveName.c_str();
        else throw std::runtime_error("DataBox::initPixmaps() - unknown category type");

        QuaZip archive( zipName );
        archive.open( QuaZip::mdUnzip );

        pix.resize(quests.size());

        bool haveFile = archive.goToFirstFile();
        while( haveFile ) {

            QuaZipFileInfo64 info;
            archive.getCurrentFileInfo( &info );

            ImageNameFinder finder( info.name );
            Questions::iterator q = std::find_if( quests.begin(), quests.end(), finder );
            if( q != quests.end() ) {

                uint number = q - quests.begin();

                const char* password = info.isEncrypted() ? Config::inst().unzipPassword.c_str() : 0;
                QuaZipFile file( &archive );
                if( file.open( QIODevice::ReadOnly, password ) ) {
                    QByteArray buffer = file.readAll();
                    pix[number].loadFromData(buffer);
                }
            }

            haveFile = archive.goToNextFile();
        }
    } else {
        QString path;
        if(categoryType == category_ab) path = Config::inst().abPathToImages.c_str();
        else if(categoryType == category_cd) path = Config::inst().cdPathToImages.c_str();
        for(Questions::iterator q = quests.begin(); q != quests.end(); ++q) {
            pix.push_back(QPixmap(path + q->getImageName()));
        }
    }

    return pix;
}


bool DataBox::load() {
    if(Config::inst().useSqliteDatabase) {
        if(!ab.load_from_sql(Config::inst().databaseName, "ab"))
            return false;
        //if(!cd.load_from_sql(Config::inst().databaseName, "cd"))
        //    return false;
    } else {
        if(!ab.load_from_pe(Config::inst().abDllMainName, Config::inst().abDllCommentName))
            return false;
        //if(!cd.load_from_pe(Config::inst().cdDllMainName, Config::inst().cdDllCommentName))
        //    return false;
    }
    return true;
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


const QPixmap& DataBox::getImage(const uint number) const {
    if(number >= pixmaps.size())
        throw std::runtime_error( QString("Incorrect question index=%1").arg(number).toStdString() );
    return pixmaps[number];
}


uint DataBox::themesCount( const bool categoryAB ) const {
    return categoryAB ? ab.themes.size() : cd.themes.size();
}


const QStringList &DataBox::getThemes(const bool categoryAB) const {
    return categoryAB ? ab.themes : cd.themes;
}


QString DataBox::getTheme(const uint themeNumber, const bool categoryAB) const {
    if( themeNumber >= themesCount(categoryAB) ) return "";
    return categoryAB ? ab.themes[themeNumber] : cd.themes[themeNumber];
}


void DataBox::initCommonTask(const uint count, const bool categoryAB ) {

    categoryType = categoryAB ? category_ab : category_cd;
    Category* category = categoryAB ? &ab : &cd;

    std::srand(time(NULL));

    uint factor = count / 20;
    if( factor == 1 )
        timer.setInterval( 20*60*1000 );
    if( factor == 2 )
        timer.setInterval( 30*60*1000 );

    questions.clear();
    answers.clear();
    pixmaps.clear();
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

            Questions& q = category->groups[g][n];
            questions.insert( questions.end(), q.begin(), q.end() );
        }
    }

    answers.assign( questions.size(), -1 );
    pixmaps = initPixmaps(questions);
}


void DataBox::initExtraTask() {
    std::srand(time(NULL));
    Category* category = 0;
    if( categoryType == category_ab ) category = &ab;
    else if( categoryType == category_cd ) category = &cd;
    else throw std::runtime_error("DataBox::initExtraTask() - unknown category type");

    totalExtraQuestions = 0;

    for( std::map<uint,uint>::iterator e = errors.begin(); e != errors.end(); ++e ) {

        uint n = e->first;
        while( n == e->first )
            n = std::rand() % category->themeBlocksCount;

        Questions& q = category->groups[e->second][n];
        questions.insert( questions.end(), q.begin(), q.end() );
        totalExtraQuestions += q.size();
        std::vector<QPixmap> pix = initPixmaps(q);
        pixmaps.insert(pixmaps.end(), pix.begin(), pix.end());
    }

    answers.insert( answers.end(), totalExtraQuestions, -1 );
    timer.setInterval( totalExtraQuestions*60*1000 );
    startTask();
}


void DataBox::initByTheme(const uint themeNumber, const bool categoryAB ) {

    categoryType = categoryAB ? category_ab : category_cd;
    Category* category = categoryType ? &ab : &cd;

    questions.clear();
    answers.clear();
    pixmaps.clear();
    errors.clear();
    totalAnswers = 0;
    totalExtraQuestions = 0;

    for( Groups::iterator g = category->groups.begin(); g != category->groups.end(); ++g ) {
        QuestionsGroup& group = *g;

        for( QuestionsGroup::iterator b = group.begin(); b != group.end(); ++ b ) {
            Questions& block = *b;
            for( Questions::iterator q = block.begin(); q != block.end(); ++q ) {
                Question& question = *q;
                if( question.getThemeNumber() == themeNumber )
                    questions.push_back( question );
            }
        }
    }

    pixmaps = initPixmaps(questions);
}


const DataBox::Questions &DataBox::getQuestions() const {
    return questions;
}


const Question DataBox::getQuestion( const uint number ) const {
    if( number >= questions.size() )
        throw std::runtime_error( QString("Incorrect question index=%1").arg(number).toStdString() );

    return questions[number];
}


bool DataBox::checkAnswer( const uint number ) const {
    if( number >= answers.size() )
        throw std::runtime_error( QString("Incorrect question index=%1").arg(number).toStdString() );

    return int(questions[number].getAnswer()) == answers[number];
}


bool DataBox::wrongAnswered(const uint number) const {
    if( number >= answers.size() )
        throw std::runtime_error( QString("Incorrect question index=%1").arg(number).toStdString() );

    if(checkAnswer(number))
        return false;
    return answers[number] > 0;
}


int DataBox::getAnswer(const uint number) const {
    if( number >= answers.size() )
        throw std::runtime_error( QString("Incorrect question index=%1").arg(number).toStdString() );

    return answers[number];
}


void DataBox::setAnswer( const uint number, const uint answer ) {
    if( number >= questions.size() )
        throw std::runtime_error( QString("Incorrect question index=%1").arg(number).toStdString() );

    answers[number] = answer;
    ++totalAnswers;

    emit questionAnswered(number, questions[number].getNumber(), answer, questions[number].getAnswer());

    if(Config::inst().extraOff == 0) { // Обычный режим с доп вопросами
        if(answer != questions[number].getAnswer()) { // ответ неправильный

            if(totalExtraQuestions > 0) {
                timer.stop();
                emit failTask(); // допущена ошибка при ответе на вопрос дополнительного блока
                return;
            }

            std::pair<std::map<uint, uint>::iterator, bool> result =
                    errors.insert( std::make_pair(
                                       questions[number].getThemeBlockNumber(),
                                       questions[number].getGroupNumber() ) );
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

        if( totalAnswers == questions.size() ) { // даны ответы на все вопросы
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
        if(answer != questions[number].getAnswer()) { // ответ неправильный

            std::pair<std::map<uint, uint>::iterator, bool> result =
                    errors.insert( std::make_pair(
                                       questions[number].getThemeBlockNumber(),
                                       questions[number].getGroupNumber() ) );
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

        if( totalAnswers == questions.size() ) { // даны ответы на все вопросы
            timer.stop();
            emit successTask();
        }
    }
}


void DataBox::onTimeoutTask() {

    timer.stop();

    if(Config::inst().extraOff == 0) {
        uint n0 = totalExtraQuestions > 0 ? questions.size() - totalExtraQuestions : 0;
        for( uint n = n0; n < questions.size(); ++n ) {

            if( answers[n] == -1 ) {

                if(totalExtraQuestions > 0) {
                    emit failTask(); // не дан ответ на вопрос дополнительного блока
                    return;
                }

                std::pair<std::map<uint, uint>::iterator, bool> result =
                        errors.insert( std::make_pair(
                                           questions[n].getThemeBlockNumber(),
                                           questions[n].getGroupNumber() ) );
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
                totalAnswers = questions.size();
                emit extraTask(ab.questionsInThemeBlock*errors.size());
            }
        }

    } else { // extraOff > 0

        for( uint n = 0; n < questions.size(); ++n ) {
            if( answers[n] == -1 ) {
                std::pair<std::map<uint, uint>::iterator, bool> result =
                        errors.insert( std::make_pair(
                                           questions[n].getThemeBlockNumber(),
                                           questions[n].getGroupNumber() ) );
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
