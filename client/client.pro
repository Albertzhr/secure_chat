QT += core gui network widgets

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    network_client.cpp

HEADERS += \
    mainwindow.h \
    network_client.h \
    ../common/protocol.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += ../common

