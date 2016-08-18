#-------------------------------------------------
#
# Project created by QtCreator 2016-08-10T18:34:45
#
#-------------------------------------------------

QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pdd-editor
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
        question_form.cpp \
        selectorbar.cpp \
        itemdialog.cpp \
    merge_dialog.cpp

HEADERS  += mainwindow.h \
        question_form.h \
        selectorbar.h \
        itemdialog.h \
    merge_dialog.h

FORMS    += mainwindow.ui \
    merge_dialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../pdd_db/release/ -lpdd_db
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../pdd_db/debug/ -lpdd_db

INCLUDEPATH += $$PWD/../pdd_db
DEPENDPATH += $$PWD/../pdd_db

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../pdd_db/release/libpdd_db.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../pdd_db/debug/libpdd_db.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../pdd_db/release/pdd_db.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../pdd_db/debug/pdd_db.lib
