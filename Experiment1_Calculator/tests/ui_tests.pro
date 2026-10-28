QT += core gui widgets testlib

CONFIG += console testcase c++17
CONFIG -= app_bundle

TARGET = tst_calculatormainwindow
TEMPLATE = app

INCLUDEPATH += ..

SOURCES += \
    tst_calculatormainwindow.cpp \
    ../calculatormainwindow.cpp \
    ../calculatorcore.cpp

HEADERS += \
    ../calculatormainwindow.h \
    ../calculatorcore.h

FORMS += \
    ../calculator.ui

RESOURCES += \
    ../resources.qrc
