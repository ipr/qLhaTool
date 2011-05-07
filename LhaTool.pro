#-------------------------------------------------
#
# Project created by QtCreator 2011-05-01T18:41:42
#
#-------------------------------------------------

QT       += core gui

TARGET = LhaTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../qLhA
LIBS += -lqLhA

CONFIG (debug, debug|release) 
{
    LIBS += -L../qLhA-build-desktop/debug
} 
CONFIG (release, debug|release) 
{
    LIBS += -L../qLhA-build-desktop/release
}
