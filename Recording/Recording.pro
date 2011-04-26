#-------------------------------------------------
#
# Project created by QtCreator 2011-04-05T22:03:35
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Recording
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/include/nite
INCLUDEPATH += /usr/include/ni
LIBS += /home/vinch/kinect/OpenNI/Platform/Linux-x86/Bin/Release/libOpenNI.so
LIBS += /home/vinch/kinect/NITE/Nite-1.3.0.17/Bin/libXnVNite.so


SOURCES += main.cpp
