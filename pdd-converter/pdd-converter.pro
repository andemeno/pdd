QT += core sql
QT -= gui

CONFIG += c++11

TARGET = pdd-converter
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

# Библиотека для чтения файлов формата PE
win32: LIBS += -L$$PWD/../libs/ -lpe_lib
INCLUDEPATH += $$PWD/../pe_lib
DEPENDPATH += $$PWD/../pe_lib

HEADERS += \
    question.h
