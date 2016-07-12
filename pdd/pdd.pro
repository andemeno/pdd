#-------------------------------------------------
#
# Project created by QtCreator 2015-10-30T21:23:33
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pdd
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    quazip/JlCompress.cpp \
    quazip/qioapi.cpp \
    quazip/quaadler32.cpp \
    quazip/quacrc32.cpp \
    quazip/quagzipfile.cpp \
    quazip/quaziodevice.cpp \
    quazip/quazip.cpp \
    quazip/quazipdir.cpp \
    quazip/quazipfile.cpp \
    quazip/quazipfileinfo.cpp \
    quazip/quazipnewinfo.cpp \
    quazip/unzip.c \
    quazip/zip.c \
    databox.cpp \
    question.cpp \
    selectorwidget.cpp \
    taskwidget.cpp \
    questionwidget.cpp \
    questionsenumwidget.cpp \
    smallquestionwidget.cpp \
    answerwidget.cpp \
    smallquestionenumwidget.cpp \
    trainingwidget.cpp \
    trainingquestionwidget.cpp \
    netclient.cpp \
    headerwidget.cpp \
    programconfig.cpp

HEADERS  += mainwindow.h \
    quazip/crypt.h \
    quazip/ioapi.h \
    quazip/JlCompress.h \
    quazip/quaadler32.h \
    quazip/quachecksum32.h \
    quazip/quacrc32.h \
    quazip/quagzipfile.h \
    quazip/quaziodevice.h \
    quazip/quazip.h \
    quazip/quazip_global.h \
    quazip/quazipdir.h \
    quazip/quazipfile.h \
    quazip/quazipfileinfo.h \
    quazip/quazipnewinfo.h \
    quazip/unzip.h \
    quazip/zip.h \
    databox.h \
    question.h \
    programconfig.h \
    selectorwidget.h \
    taskwidget.h \
    questionwidget.h \
    questionsenumwidget.h \
    smallquestionwidget.h \
    answerwidget.h \
    smallquestionenumwidget.h \
    trainingwidget.h \
    trainingquestionwidget.h \
    netclient.h \
    headerwidget.h

# using zlib
unix|win32: LIBS += -LC:/Qt/Tools/mingw492_32/i686-w64-mingw32/lib/ -lz

INCLUDEPATH += C:/Qt/Tools/mingw492_32/i686-w64-mingw32/include
DEPENDPATH += C:/Qt/Tools/mingw492_32/i686-w64-mingw32/include

win32:!win32-g++: PRE_TARGETDEPS += C:/Qt/Tools/mingw492_32/i686-w64-mingw32/lib/z.lib
else:unix|win32-g++: PRE_TARGETDEPS += C:/Qt/Tools/mingw492_32/i686-w64-mingw32/lib/libz.a

DEFINES += QUAZIP_STATIC

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/ -lpe_lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/ -lpe_lib
else:unix: LIBS += -L$$PWD/../build-pe_lib-Desktop_Qt_5_5_1_MinGW_32bit-Debug/ -lpe_lib

INCLUDEPATH += $$PWD/../pe_lib
DEPENDPATH += $$PWD/../pe_lib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libs/libpe_lib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libs/libpe_lib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libs/pe_lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libs/pe_lib.lib
else:unix: PRE_TARGETDEPS += $$PWD/../libs/libpe_lib.a
