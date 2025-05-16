QT += core gui widgets network sql

CONFIG += c++11

# 包含目录
INCLUDEPATH += ../common

# 源文件
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    loginwindow.cpp \
    auth_manager.cpp \
    network_client.cpp \
    sm3_utils.cpp

# 头文件
HEADERS += \
    mainwindow.h \
    loginwindow.h \
    auth_manager.h \
    network_client.h \
    sm3_utils.h \
    ../common/protocol.h

# UI 文件
FORMS += \
    mainwindow.ui \
    loginwindow.ui

