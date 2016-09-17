#include "mainwindow.h"
#include "databox.h"
#include "programconfig.h"
#include "netclient.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QDockWidget>
#include <QLabel>
#include <QPixmap>
#include "selectorwidget.h"
#include "headerwidget.h"
#include <QTime>
#include "taskwidget.h"
#include "trainingwidget.h"
#include <stdexcept>
#include <QMessageBox>
#include <QAction>

using namespace pdd;

const QString version_info("Версия программы 1.3 от 17-09-16");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , state(state_wait_task)
    , questionsCount(0)
    , themeNum(0)
    , category(true)
    , timer(this) {

    /* Получение параметров командной строки */

    QCommandLineOption fontIncOption("f");
    fontIncOption.setValueName("fontincvalue");
    fontIncOption.setDescription("Количество точек, на которое будет увеличен или уменьшен размер шрифта.");
    fontIncOption.setDefaultValue("3");
    QCommandLineOption addrOption("a");
    addrOption.setDescription("Задает сетевой адрес и номер порта сервера. По умолчанию 127.0.0.1");
    addrOption.setValueName("serveraddr");
    addrOption.setDefaultValue("127.0.0.1");
    QCommandLineOption armNumberOption("n");
    armNumberOption.setValueName("armnumber");
    armNumberOption.setDefaultValue("1");
    armNumberOption.setDescription("Номер рабочего места");

    QCommandLineOption cheatOption("cheat");

    QCommandLineOption extraOff("extraoff");
    QCommandLineOption viewOnlyWrong("onlywrong");
    QCommandLineOption useStaticPrompt("use-static-prompt");
    QCommandLineOption useTextPrompt("use-text-prompt");

    QCommandLineParser parser;
    parser.addOption(fontIncOption);
    parser.addOption(addrOption);
    parser.addOption(armNumberOption);
    parser.addOption(cheatOption);
    parser.addOption(extraOff);
    parser.addOption(viewOnlyWrong);
    parser.addOption(useStaticPrompt);
    parser.addOption(useTextPrompt);
    parser.process(*qApp);
    int fontIncValue = parser.value(fontIncOption).toInt();
    Config::inst().localMode = !parser.isSet(addrOption);
    Config::inst().armNumber = parser.value(armNumberOption).toUInt();
    Config::inst().cheat = parser.isSet(cheatOption);
    Config::inst().extraOff = parser.isSet(extraOff);
    Config::inst().viewOnlyWrong = parser.isSet(viewOnlyWrong);
    Config::inst().usePromptFromFile = !parser.isSet(useStaticPrompt);
    Config::inst().use_prompt_picture = !parser.isSet(useTextPrompt);

    //qDebug() << "Версия протокола " << Config::inst().protocolVersion;
    //qDebug() << "Extra off " << Config::inst().extraOff;
    //qDebug() << "use-common-db: " << int(Config::inst().useSqliteDatabase);

    QAction* about_action = new QAction(QString("About"), this);
    about_action->setShortcut(QKeySequence(Qt::Key_F1));
    connect(about_action, SIGNAL(triggered(bool)), this, SLOT(about()));
    addAction(about_action);

    // Загрузка вопросов
    if(!pdd::DataBox::inst().load())
        throw std::runtime_error("Ошибка при загрузке базы данных");
    pdd::DataBox& box = pdd::DataBox::inst();

    if(!Config::inst().localMode) {
        QString serverAddr = parser.value(addrOption);
        uint serverPort = Config::inst().serverPort;
        client = new NetClient(serverAddr, serverPort, this);
        connect(client, SIGNAL(connected()), this, SLOT(onServerConnected()));
        connect(client, SIGNAL(disconnected()), this, SLOT(onServerDisconnected()));
        connect(client, SIGNAL(registerTask(QString,uint,bool)), this, SLOT(onRegisterTask(QString,uint,bool)));
        connect(client, SIGNAL(stopTask()), this, SLOT(onStopTaskByServer()));
    }

    /* Настройка размера шрифта */
    QFont font = QApplication::font();
    int sz = font.pointSize();
    font.setPointSize( sz + fontIncValue );
    setFont( font );

    /* Инициализация окна приложения */

    QDockWidget* dock = new QDockWidget;
    dock->setFloating(false);
    dock->setAllowedAreas(Qt::TopDockWidgetArea);
    headerWidget = new HeaderWidget;
    dock->setWidget(headerWidget);
    dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock->setTitleBarWidget(new QLabel(""));
    addDockWidget(Qt::TopDockWidgetArea, dock);

    connect(&timer, SIGNAL(timeout()), this, SLOT(onSecondElapsed()));
    connect(&box, SIGNAL(taskStarted()), this, SLOT(onTaskStarted()));
    connect(&box, SIGNAL(successTask()), this, SLOT(onTaskEnded()));
    connect(&box, SIGNAL(failTask()), this, SLOT(onTaskEnded()));

    setWindowState(Qt::WindowFullScreen);
    setWindowTitle("ПДД");

    if(Config::inst().localMode) {
        headerWidget->setMainText("программа самоконтроля");
    } else {
        headerWidget->setArmNumber(Config::inst().armNumber);
        headerWidget->setMainText("нет связи с сервером, ожидайте...");
    }
    setSelectorWidget();
}

MainWindow::~MainWindow() {}


void MainWindow::mouseDoubleClickEvent(QMouseEvent *) {
    if(state == state_wait_start) {

        if(questionsCount > 0)
            setTaskWidget();

    } else if(state == state_view_results) {
        if(Config::inst().localMode)
            setSelectorWidget();
    }
}

void MainWindow::onServerConnected() {
    headerWidget->setMainText("связь с сервером установлена, ожидайте...");
    setSelectorWidget();
    state = state_wait_task;
}

void MainWindow::onServerDisconnected() {
    timer.stop();
    headerWidget->setMainText("нет связи с сервером, ожидайте...");
    setSelectorWidget();
    state = state_wait_task;
}

void MainWindow::onRegisterTask(QString name, uint qcount, bool category_ab) {
    questionsCount = qcount;
    category = category_ab;
    themeNum = 0;

    QString headerText("экзамен на право управления транспортным средством. категория в/у: ");
    headerText += category_ab ? "В" : "СD";
    headerText += "\n";
    headerText += name;
    headerWidget->setMainText(headerText);
    setPromptWidget();
}

void MainWindow::onSelectTask(bool category_ab, uint qcount) {
    questionsCount = qcount;
    category = category_ab;
    themeNum = 0;

    QString headerText("экзамен на право управления транспортным средством. категория в/у: ");
    headerText += category_ab ? "В" : "СD";
    headerWidget->setMainText(headerText);
    setPromptWidget();
}

void MainWindow::onSelectTrainig(bool category_ab, uint themeNumber) {
    themeNum = themeNumber;
    category = category_ab;
    questionsCount = 0;

    QString headerText =
      QString("ознакомление с вопросами по теме\n%1").
        arg(DataBox::inst().getTheme(themeNumber));
    headerWidget->setMainText(headerText);
    setTrainingWidget();
}

void MainWindow::onStopTaskByServer() {
    timer.stop();
    headerWidget->setMainText("связь с сервером установлена, ожидайте...");
    setSelectorWidget();
}

void MainWindow::onTaskStarted() {
    timer.start(1000);
    QTime t = DataBox::inst().getTaskRemainingTime();
    headerWidget->setTimerText(QString("Оставшееся время\n%1").arg( t.toString("mm:ss")));
}

void MainWindow::onTaskEnded() {
    timer.stop();
    QTime t = DataBox::inst().getTaskElapsedTime();
    headerWidget->setTimerText(QString("Затраченное время\n%1").arg( t.toString("mm:ss")));
    state = state_view_results;
}

void MainWindow::onSecondElapsed() {
    QTime t = DataBox::inst().getTaskRemainingTime();
    headerWidget->setTimerText(QString("Оставшееся время\n%1").arg( t.toString("mm:ss")));
}

void MainWindow::setSelectorWidget() {
    headerWidget->setTimerText("");
    if(Config::inst().localMode) {
        headerWidget->setMainText("программа самоконтроля");
        SelectorWidget* w = new SelectorWidget;
        connect(w, SIGNAL(startTask(bool,uint)), this, SLOT(onSelectTask(bool,uint)) );
        connect(w, SIGNAL(startTraining(bool,uint)), this, SLOT(onSelectTrainig(bool,uint)));
        setCentralWidget(w);
    } else {
        QLabel* w = new QLabel(QString("%1").arg(Config::inst().armNumber));
        w->setAlignment(Qt::AlignCenter);
        QFont font = w->font();
        font.setPointSize(200);
        w->setFont( font );
        QPalette pal = w->palette();
        pal.setColor(QPalette::WindowText, QColor("#0055ee"));
        w->setPalette(pal);
        setCentralWidget(w);
    }
    state = state_wait_task;
}

void MainWindow::setPromptWidget() {

    QLabel* w = 0;
    if(!Config::inst().use_prompt_picture) {
        w = new QLabel(Config::inst().largePromptText().c_str());
    } else {
        w = new QLabel;
        w->setPixmap(QPixmap(Config::inst().prompt_picture_file.c_str()));
    }
    w->setAlignment(Qt::AlignCenter);
    setCentralWidget(w);
    state = state_wait_start;
}

void MainWindow::setTaskWidget() {
    DataBox::inst().initCommonTask(questionsCount);
    TaskWidget* w = new TaskWidget;
    setCentralWidget(w);
    DataBox::inst().startTask();
    state = state_execute_task;
}

void MainWindow::setTrainingWidget() {
    DataBox::inst().initByTheme(themeNum);
    TrainingWidget* w = new TrainingWidget;
    setCentralWidget(w);
    state = state_view_results;
}

void MainWindow::about() {
    QMessageBox::about(this, tr("О pdd-exam"), version_info);
}
