QT += testlib
QT -= gui

CONFIG += console testcase c++17
CONFIG -= app_bundle

TARGET = tst_calculatorcore
TEMPLATE = app

INCLUDEPATH += ..

SOURCES += \
    tst_calculatorcore.cpp \
    ../calculatorcore.cpp

HEADERS += \
    ../calculatorcore.h
