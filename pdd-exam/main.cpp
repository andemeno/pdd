#include "mainwindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]) {

    try {
        QApplication a(argc, argv);
        a.setApplicationName("pdd-exam");
        a.setApplicationVersion("1.4");
        a.setApplicationDisplayName("ПДД клиент");
        pdd::MainWindow w;
        w.show();
        return a.exec();

    } catch(std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return EXIT_FAILURE;
}
