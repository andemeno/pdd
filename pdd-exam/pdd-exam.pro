#-------------------------------------------------
#
# Project created by QtCreator 2016-08-19T11:57:38
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pdd-exam
TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../pdd_db/release/ -lpdd_db
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../pdd_db/debug/ -lpdd_db

INCLUDEPATH += $$PWD/../pdd_db
DEPENDPATH += $$PWD/../pdd_db

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../pdd_db/release/libpdd_db.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../pdd_db/debug/libpdd_db.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../pdd_db/release/pdd_db.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../pdd_db/debug/pdd_db.lib


SOURCES += main.cpp\
        mainwindow.cpp \
    trainingwidget.cpp \
    trainingquestionwidget.cpp \
    taskwidget.cpp \
    smallquestionwidget.cpp \
    smallquestionenumwidget.cpp \
    selectorwidget.cpp \
    questionwidget.cpp \
    questionsenumwidget.cpp \
    programconfig.cpp \
    netclient.cpp \
    headerwidget.cpp \
    databox.cpp \
    answerwidget.cpp

HEADERS  += mainwindow.h \
    trainingwidget.h \
    trainingquestionwidget.h \
    taskwidget.h \
    smallquestionwidget.h \
    smallquestionenumwidget.h \
    selectorwidget.h \
    questionwidget.h \
    questionsenumwidget.h \
    programconfig.h \
    netclient.h \
    headerwidget.h \
    databox.h \
    answerwidget.h
