#-------------------------------------------------
#
# Project created by QtCreator 2011-09-27T03:20:23
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = qLhaCmd
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

INCLUDEPATH += ../qLhALib
LIBS += -lqLhA

CONFIG (debug, debug|release) 
{
    LIBS += -L../qLhA-build-desktop/debug
} 
CONFIG (release, debug|release) 
{
    LIBS += -L../qLhA-build-desktop/release
}
