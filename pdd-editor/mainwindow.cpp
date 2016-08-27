#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "question_form.h"
#include "question.h"
#include "selectorbar.h"
#include "merge_dialog.h"
#include <QToolBar>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSql>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    , work_dir("..\\work-resources")
    , doc(new pdd::pdd_db(work_dir + "\\pdd.db")) {

    ui->setupUi(this);

    if(!doc->load()) {
        statusBar()->showMessage(QString("Ошибка при загрузке %1/%2").arg(work_dir).arg("pdd.db"));
    } else {
        statusBar()->showMessage(QString("Используется БД %1/%2").arg(work_dir).arg("pdd.db"));
        setWindowTitle(QString("Редактирование %1/%2").arg(work_dir).arg("pdd.db"));
    }

    selector = new SelectorBar(doc);
    connect(selector, SIGNAL(show_question(uint,uint)), this, SLOT(show_question(uint,uint)));

    QToolBar* toolbar = new QToolBar;
    toolbar->addWidget(selector);
    addToolBar(toolbar);
    show_question(1, 1);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::show_question(const uint theme, const uint n) {
    pdd::question_form* form = new pdd::question_form(doc, theme, n);
    connect(form, SIGNAL(question_saved()), selector, SLOT(on_show_question()));
    setCentralWidget(form);
}


void MainWindow::rename_images_1() {
    uint themes = doc->get_themes_count();
    for(uint t = 1; t <= themes; ++t) {
        uint qcount = doc->get_questions_count(t);
        for(uint n = 1; n <= qcount; ++n) {
            const pdd::question& quest = doc->get_question(t, n);
            if(quest.get_image_name().isEmpty())
                continue;
            QString file_name = QString("%1/images/ab/%2").arg(work_dir).arg(quest.get_image_name());
            QString new_name = QString("%1/images/ab/%2-%3.jpg").arg(work_dir).arg(t).arg(n);
            QFile file(file_name);
            if(!file.open(QIODevice::ReadWrite)) {
                qDebug() << "error on open" << file_name;
                return;
            }
            if(!file.rename(new_name)) {
                qDebug() << "error on rename" << file_name << "to" << new_name;
                return;
            }
        }
    }
}

void MainWindow::rename_images_2(const QString &path_to_images) {

    QFile table_file(work_dir + "/table.json");
    if(!table_file.open(QIODevice::ReadOnly)) {
        qDebug() << "error on open file" << table_file.fileName();
        return;
    }

    QByteArray data = table_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(data);
    QJsonObject json = json_doc.object();
    for(uint task = 1; task <= 40; ++task) {
        QJsonValue value = json[QString("T%1").arg(task)];
        if(value.isUndefined())
            continue;
        // Вопросы билета с номером task
        QJsonObject json_task = value.toObject();
        for(uint n = 1; n <= 20; ++n) {

            QJsonValue qv = json_task[QString("%1").arg(n)];
            if(qv.isUndefined()) continue;
            QJsonObject json_quest = qv.toObject();
            int theme_n = json_quest["t"].toString().toInt();
            int quest_n = json_quest["n"].toString().toInt();

            QString old_name = QString("%1%2-%3.jpg").arg(path_to_images).arg(task).arg(n);
            QString new_name = QString("%1%2-%3.jpg").arg(path_to_images).arg(theme_n).arg(quest_n);
            QFile image_file(old_name);
            if(!image_file.open(QIODevice::ReadWrite)) {
                qDebug() << "error on open" << old_name;
                continue;
            }
            if(!image_file.rename(new_name)) {
                qDebug() << "error on rename" << old_name << "to" << new_name;
                continue;
            }
        }

    }
}

void MainWindow::init_table_quests_by_task() {

    // Установка соединения с базой данных
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName(work_dir + "/pdd.db");
    if(!sdb.open()) {
       qDebug() << sdb.databaseName() << " " << sdb.lastError().text();
       return;
    }

    QFile table_file(work_dir + "/table.json");
    if(!table_file.open(QIODevice::ReadOnly)) {
        qDebug() << "error on open file" << table_file.fileName();
        return;
    }

    QByteArray data = table_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(data);
    QJsonObject json = json_doc.object();
    for(uint task = 1; task <= 40; ++task) {
        QJsonValue value = json[QString("T%1").arg(task)];
        if(value.isUndefined())
            continue;
        // Вопросы билета с номером task
        QJsonObject json_task = value.toObject();
        for(uint n = 1; n <= 20; ++n) {

            QJsonValue qv = json_task[QString("%1").arg(n)];
            if(qv.isUndefined()) continue;
            QJsonObject json_quest = qv.toObject();
            int theme_n = json_quest["t"].toString().toInt();
            int quest_n = json_quest["n"].toString().toInt();

            QString str_query = QString(
                    "insert into quests_by_task_ab (task_number, in_task_number, theme_number, in_theme_number) values (%1, %2, %3, %4)"
                    ).arg(task).arg(n).arg(theme_n).arg(quest_n);
            QSqlQuery query;
            query.prepare(str_query);
            if(!query.exec()) {
                qDebug() << query.executedQuery() << ": " << query.lastError().text();
                continue;
            }
        }
    }
}

void MainWindow::on_action_merge_triggered() {
    QString db_name = QFileDialog::getOpenFileName(this, "Выберете базу данных", work_dir, "*.db");
    if(db_name.isEmpty()) return;

    merge_dialog dialog(doc, this);
    QString title = db_name.mid(db_name.lastIndexOf('/') + 1);
    dialog.setWindowTitle(QString("Вопросы из базы %1").arg(title));
    if(!dialog.exec()) return;

    data_to_merge to_merge = dialog.get_data_to_merge();
    merge_questions(db_name, to_merge);
}

void MainWindow::merge_questions(const QString& db_name, const data_to_merge& to_merge) {

    /* Загрузка заданных вопросов из db_name и передача их в doc */

    pdd::pdd_db changed_db(db_name);
    if(!changed_db.load()) {
        statusBar()->showMessage("Ошибка при загрузке базы данных " + db_name);
        return;
    }

    QString msg;
    for(data_to_merge::const_iterator m = to_merge.begin(); m != to_merge.end(); ++m) {
        const uint thn = m->first;
        const uint from = m->second.first;
        const uint to = m->second.second;
        for(uint n = from; n <= to; ++n) {
            const pdd::question& q = changed_db.get_question(thn, n);
            doc->set_question(q);
        }
        msg += QString("тема %1 вопросы %2-%3\n").arg(thn).arg(from).arg(to);
    }

    QMessageBox msgBox;
    msgBox.setText(QString("Выполнено слияние данных:\n%1").arg(msg));

    if(!doc->save_changes()) { // сохранение
        msg = QString("Ошибка при слиянии данных из %1").arg(db_name);
    } else {
        msg = QString("Выполнено слияние данных из %1").arg(db_name);
    }

    msgBox.exec();
    statusBar()->showMessage(msg);
    qDebug() << msg;
}


void MainWindow::on_rename_action_triggered() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setDirectory(work_dir);
    if(dialog.exec()) {
        QStringList paths = dialog.selectedFiles();
        if(!paths.isEmpty()) {
            rename_images_2(paths[0]);
        }
    }
}

void MainWindow::on_choose_bd_action_triggered() {
    QFileDialog dialog(this);
    dialog.setWindowTitle("Укажите расположение с рабочей БД");
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setDirectory(work_dir);
    if(dialog.exec()) {
        QStringList paths = dialog.selectedFiles();
        if(!paths.isEmpty()) {
            QString new_work_dir = paths[0];
            QSharedPointer<pdd::pdd_db> new_doc(new pdd::pdd_db(new_work_dir + "/pdd.db"));
            if(!new_doc->load())
                statusBar()->showMessage(QString("Ошибка при загрузке %1/%2").arg(new_work_dir).arg("pdd.db"));
            else {
                work_dir = new_work_dir;
                doc = new_doc;
                statusBar()->showMessage(QString("Используется БД %1/%2").arg(work_dir).arg("pdd.db"));
                setWindowTitle(QString("Редактирование %1/%2").arg(work_dir).arg("pdd.db"));
                show_question(1, 1);
            }
        }
    }
}
