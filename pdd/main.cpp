#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "databox.h"

int main(int argc, char *argv[]) {

    try {
        // Вызов конструктора DataBox (загрузка ресурсов).
        pdd::DataBox& box = pdd::DataBox::inst();

        QApplication a(argc, argv);
        a.setApplicationName("pdd-client");
        a.setApplicationVersion("0.1");
        a.setApplicationDisplayName("ПДД клиент");
        pdd::MainWindow w;
        w.show();
        return a.exec();

    } catch(std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return EXIT_FAILURE;
}
