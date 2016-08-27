#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pdd_db.h>
#include <QSharedPointer>

namespace Ui {
class MainWindow;
}

class SelectorBar;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void show_question(const uint theme, const uint n);
    void on_action_merge_triggered();
    void on_rename_action_triggered();   
    void on_choose_bd_action_triggered();

private:

    typedef std::map<uint, std::pair<uint, uint> > data_to_merge;
    void merge_questions(const QString& db_name, const data_to_merge& to_merge);
    void rename_images_1(); // переименование иллюстраций со страрыми названиями в формат <номер темы>-<номер вопроса>.jpg
    void rename_images_2(const QString& path_to_images); // переименование иллюстраций из формата <номер билета>-<номер вопроса> в формат <номер темы>-<номер вопроса>
    void init_table_quests_by_task();

    Ui::MainWindow *ui;
    QString work_dir;
    QSharedPointer<pdd::pdd_db> doc;
    SelectorBar* selector;
};

#endif // MAINWINDOW_H
