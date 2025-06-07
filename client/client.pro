QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = client
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    protocol.h

FORMS += \
    mainwindow.ui

