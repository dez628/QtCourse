QT += core gui widgets

CONFIG += c++17

TARGET = Experiment1_Calculator
TEMPLATE = app

SOURCES += \
    main.cpp \
    calculatormainwindow.cpp \
    calculatorcore.cpp

HEADERS += \
    calculatormainwindow.h \
    calculatorcore.h

FORMS += \
    calculator.ui
