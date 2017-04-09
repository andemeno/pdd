#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace pdd {

class NetClient;
class HeaderWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void mouseDoubleClickEvent(QMouseEvent *);

private slots:
    void onServerConnected();
    void onServerDisconnected();
    // Регистарция задания от сервера
    void onRegisterTask(QString name, uint qcount, bool category_ab);
    void onRegisterTest(QString name, const uint theme_n, const uint quest_n, const uint qcount, const bool category_ab);
    // Оператор выбрал задание
    void onSelectTask(bool category_ab, uint qcount);
    // Оператор выбрал тренировку
    void onSelectTrainig(bool category_ab, uint themeNumber);
    void onStopTaskByServer();
    void onTaskStarted();
    void onTaskEnded();
    void onSecondElapsed();
    void about();

private:

    void setSelectorWidget();
    void setPromptWidget();
    void setTaskWidget();
    void setTrainingWidget();
    void setTestThemeBlockWidget();

    enum {
        state_wait_task, // ждем задание от сервера
        state_wait_start, // получил задание но не начал его выполнять
        state_execute_task, // отвечает
        state_view_results // смотрит результаты
    };

    int state;
    uint questionsCount;
    uint themeNum;
    uint questionNum;
    bool category;
    QTimer timer;
    NetClient* client;
    HeaderWidget* headerWidget;
    QString user_name;
};

}

#endif // MAINWINDOW_H
