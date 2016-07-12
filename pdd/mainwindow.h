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
    // Оператор выбрал задание
    void onSelectTask(bool category_ab, uint qcount);
    // Оператор выбрал тренировку
    void onSelectTrainig(bool category_ab, uint themeNumber);
    void onStopTaskByServer();
    void onTaskStarted();
    void onTaskEnded();
    void onSecondElapsed();

private:

    void setSelectorWidget();
    void setPromptWidget();
    void setTaskWidget();
    void setTrainingWidget();

    enum {
        state_wait_task,
        state_wait_start,
        state_execute_task,
        state_view_results
    };

    int state;
    uint questionsCount;
    uint themeNum;
    bool category;
    QTimer timer;
    NetClient* client;
    HeaderWidget* headerWidget;
};

}

#endif // MAINWINDOW_H
