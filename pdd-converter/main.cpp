#include <QCoreApplication>
#include <pe_base.h>
#include <pe_factory.h>
#include <pe_resources.h>
#include <pe_resource_viewer.h>
#include <pe_resource_manager.h>
#include <iostream>
#include <fstream>
#include <QtSql>
#include "question.h"

using namespace pe_bliss;

int main(int argc, char *argv[]) {

    QCoreApplication app(argc, argv);
    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");

    // Подготовка SQLite базы данных
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName("pdd.db");
    if(!sdb.open()) {
       std::cout << sdb.lastError().text().toStdString() << std::endl;
       return EXIT_FAILURE;
    }

    QString category_name = "ab";
    do {
        std::string pe_main_name(category_name.toStdString() + "/text.dat");
        //pe_main_name.append(category_name.toStdString() + "/text.dat");
        std::string pe_comment_name(category_name.toStdString() + "/comment.dat");
        //pe_comment_name.append(category_name.toStdString() + "/comment.dat");

        // Открываем файл
        std::ifstream pe_main(pe_main_name.c_str(), std::ios::in | std::ios::binary);
        if(!pe_main) {
            std::cout << "Error on open " << pe_main_name << std::endl;
            return EXIT_FAILURE;
        }

        // Открываем файл
        std::ifstream pe_comment(pe_comment_name.c_str(), std::ios::in | std::ios::binary);
        if(!pe_comment){
            std::cout << "Error on open " << pe_comment_name << std::endl;
            return EXIT_FAILURE;
        }

        Themes themes;

        try {
            //Создаем экземпляр PE или PE+ класса с помощью фабрики
            pe_base image( pe_factory::create_pe(pe_main) );
            //Проверим, есть ли ресурсы у файла
            if(!image.has_resources()) {
                std::cout << pe_main_name << " does not have resources" << std::endl;
                return EXIT_FAILURE;
            }
            //Получаем корневую директорию ресурсов
            const resource_directory root(get_resources(image));
            pe_resource_viewer res(root);

            //Создаем экземпляр PE или PE+ класса с помощью фабрики
            pe_base imageComment( pe_factory::create_pe(pe_comment) );
            //Проверим, есть ли ресурсы у файла
            if(!imageComment.has_resources()) {
                std::cout << pe_comment_name << " does not have resources" << std::endl;
                return EXIT_FAILURE;
            }
            //Получаем корневую директорию ресурсов
            const resource_directory rootComment(get_resources(imageComment));
            pe_resource_viewer resComment(rootComment);

            // Правильные ответы по номерам вопросов
            std::map<uint, uint> ans;
            {
                resource_data_info info("", 0);
                if(res.get_language_count(L"OTV", 1018) > 1)
                    info = res.get_resource_data_by_id( 0, L"OTV", 1018 );
                else
                    info = res.get_resource_data_by_id( 1049, L"OTV", 1018 );

                std::string raw( info.get_data() );
                for(unsigned int i = 4; i < raw.size(); i+=2 ) {
                    uint16_t answer = raw[i] + (raw[i+1]<<8);
                    unsigned int n = i/2 - 2;
                    ans.insert( std::make_pair( n, answer ));
                }
            }

            // Вопросы по темам (номер вопроса - номер темы)
            {
                // Количество тем
                resource_data_info info = res.get_resource_data_by_id(1049, L"COUNTTEM", 1021);
                std::string raw( info.get_data() );
                uint totalThemes = raw[0] + (raw[1] << 8);

                themes.resize(totalThemes);

                for( uint t = 0; t < totalThemes; ++t ) {
                    resource_data_info info("", 0);
                    if( res.get_language_count(L"POTEMAM", 1022+t) > 1 )
                        info = res.get_resource_data_by_id( 0, L"POTEMAM", 1022+t );
                    else
                        info = res.get_resource_data_by_id( 1049, L"POTEMAM", 1022+t );

                    std::string raw( info.get_data() );
                    uint32_t questionsInTheme = (raw.size() - 4)/6; // количество вопросов в теме
                    themes[t].questions.resize(questionsInTheme);

                    for( unsigned int i = 0; i < questionsInTheme; ++i ) {
                        uint16_t n = 20*(raw[4 + i*6] - 1) + raw[6 + i*6] - 1;
                        Question& question = themes[t].questions[i];
                        question.themeNumber = t;
                        question.number = n+1;
                        question.imageName = QString("jpg%1.jpg").arg(n+1);
                        uint qid = n + 217;

                        // правильный ответ
                        std::map<uint,uint>::iterator a = ans.find(n);
                        if(a != ans.end())
                            question.answer = a->second;

                        // задание и варианты ответов
                        {
                            resource_data_info info( "", 0 );
                            if( res.get_language_count( L"TEXT", qid ) > 1 )
                                info = res.get_resource_data_by_id( 0, L"TEXT", qid );
                            else
                                info = res.get_resource_data_by_id( 1049, L"TEXT", qid );

                            std::string raw( info.get_data() );
                            question.set_task_and_answers(codec->toUnicode(raw.c_str()));
                        }

                        // комментарий
                        {
                            resource_data_info info( "", 0 );
                            if( resComment.get_language_count( L"TEXT", qid ) > 1 )
                                info = resComment.get_resource_data_by_id( 0, L"TEXT", qid );
                            else
                                info = resComment.get_resource_data_by_id( 1049, L"TEXT", qid );

                            std::string raw( info.get_data() );
                            question.comment = codec->toUnicode(raw.c_str()).replace("\n", " ");
                        }
                    }

                    { // Названия тем
                        resource_data_info info = res.get_resource_data_by_id(1049, L"THEME_NAME", 1050 + t);
                        themes[t].name = codec->toUnicode(info.get_data().c_str());
                    }
                }
            }

        } catch( const pe_exception& e ) { //Если возникла ошибка
            std::cout << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        // Данные из PE файлов загружены. Формируем таблицы новой базы данных

        for(uint t = 0; t < themes.size(); ++t) {
            QSqlQuery addTheme;
            addTheme.prepare(QString("INSERT INTO themes_%1"\
                             "(theme_num, name, quest_count)"\
                             " VALUES"\
                             "(:theme_num, :name, :quest_count);").arg(category_name));

            addTheme.bindValue(":theme_num", t+1);
            addTheme.bindValue(":name", themes[t].name);
            addTheme.bindValue(":quest_count", themes[t].questions.size());

            if(!addTheme.exec()) {
                std::cout << addTheme.lastError().text().toStdString() << std::endl;
            }
        }

        uint i = 1;
        for(uint t = 0; t < themes.size(); ++t) {

            uint in_theme_num = 1;
            Questions& questions = themes[t].questions;
            for(Questions::iterator q = questions.begin(); q != questions.end(); ++q) {
                QSqlQuery addQuestion;
                addQuestion.prepare(QString("INSERT INTO questions_%1"\
                                    "(number, task, vars_count, right_ans, comment, ans_v1, ans_v2, ans_v3, ans_v4, ans_v5, image, theme_num, in_theme_num)"
                                    "VALUES"\
                                    "(:number, :task, :vars_count, :right_ans, :comment, :ans_v1, :ans_v2, :ans_v3, :ans_v4, :ans_v5, :image, :theme_num, :in_theme_num);").arg(category_name));

                addQuestion.bindValue(":number", i++);
                addQuestion.bindValue(":task", q->task);
                QStringList& answers = q->answers;
                uint vars_count = answers.size();
                addQuestion.bindValue(":vars_count", vars_count);
                addQuestion.bindValue(":right_ans", q->answer);
                addQuestion.bindValue(":comment", q->comment);

                for(uint v = 0; v < vars_count; ++v) {
                    addQuestion.bindValue(QString(":ans_v%1").arg(v+1), answers[v]);
                }

                for(uint k = vars_count; k < 6; ++k) {
                    addQuestion.bindValue(QString(":ans_v%1").arg(k+1), "");
                }

                addQuestion.bindValue(":image", q->imageName);
                addQuestion.bindValue(":theme_num", q->themeNumber+1);
                addQuestion.bindValue(":in_theme_num", in_theme_num++);

                if(!addQuestion.exec()) {
                    std::cout << addQuestion.lastError().text().toStdString() << std::endl;
                } else {
                    std::cout << category_name.toStdString() << " add question " << i-1 << std::endl;
                }
            }
        }

        if(category_name == "ab") category_name = "cd";
        else category_name = "";

    } while(category_name != "");

    std::cout << "Complited." << std::endl;
    return app.exec();
}
