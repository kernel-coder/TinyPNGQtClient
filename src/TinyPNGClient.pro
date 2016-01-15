#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T16:36:00
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TinyPNGClient
TEMPLATE = app
DESTDIR= $$PWD/Setup


SOURCES += main.cpp\
        MainWindow.cpp \
    TableWidget.cpp \
    SettingsDlg.cpp

HEADERS  += MainWindow.h \
    TableWidget.h \
    SettingsDlg.h

FORMS += \
    SettingsDlg.ui

include ($$PWD/Shared/Shared.pri)
