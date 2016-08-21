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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    , work_dir("../pdd_resources/")
    , doc(work_dir + "pdd.db") {
    ui->setupUi(this);

    // todo название базы спрашивать у опертора и передавать в командной строке
    if(!doc.load()) {
        statusBar()->showMessage(QString("Ошибка при загрузке %1%2").arg(work_dir).arg("pdd.db"));
    }

    selector = new SelectorBar(&doc);
    connect(selector, SIGNAL(show_question(uint,uint)), this, SLOT(show_question(uint,uint)));

    QToolBar* toolbar = new QToolBar;
    toolbar->addWidget(selector);
    addToolBar(toolbar);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::show_question(const uint theme, const uint n) {
    pdd::question_form* form = new pdd::question_form(&doc, theme, n);
    connect(form, SIGNAL(question_saved()), selector, SLOT(on_show_question()));
    setCentralWidget(form);
}


void MainWindow::rename_images() {
    uint themes = doc.get_themes_count();
    for(uint t = 1; t <= themes; ++t){
        uint qcount = doc.get_questions_count(t);
        for(uint n = 1; n <= qcount; ++n) {
            const pdd::question& quest = doc.get_question(t, n);
            if(quest.get_image_name().isEmpty())
                continue;
            QString file_name = QString("%1images/ab/%2").arg(work_dir).arg(quest.get_image_name());
            QString new_name = QString("%1images/ab/%2-%3.jpg").arg(work_dir).arg(t).arg(n);
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
            doc.set_question(q);
        }
        msg += QString("тема %1 вопросы %2-%3\n").arg(thn).arg(from).arg(to);
    }

    QMessageBox msgBox;
    msgBox.setText(QString("Выполнено слияние данных:\n%1").arg(msg));

    if(!doc.save_changes()) { // сохранение
        msg = QString("Ошибка при слиянии данных из %1").arg(db_name);
    } else {
        msg = QString("Выполнено слияние данных из %1").arg(db_name);
    }

    msgBox.exec();
    statusBar()->showMessage(msg);
    qDebug() << msg;
}

