#-------------------------------------------------
#
# Project created by QtCreator 2011-04-28T15:09:06
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = OniToAvi
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += /usr/include/nite
INCLUDEPATH += /usr/include/ni
INCLUDEPATH += /usr/include/opencv
INCLUDEPATH += /home/vinch/qtsdk-2010.05/qt/include/Qt
#INCLUDEPATH += /home/david/qtsdk-2010.05/qt/include/Qt

LIBS += /home/vinch/kinect/OpenNI/Platform/Linux-x86/Bin/Release/libOpenNI.so
LIBS += /home/vinch/kinect/NITE/Nite-1.3.0.17/Bin/libXnVNite.so
#LIBS += /home/david/kinect/OpenNI/Platform/Linux-x86/Bin/Release/libOpenNI.so
#LIBS += /home/david/kinect/Nite-1.3.0.17/Bin/libXnVNite.so


LIBS += /usr/lib/libglut.so
LIBS += /usr/lib/libcv.so
LIBS += /usr/lib/libcvaux.so
LIBS += /usr/lib/libcxcore.so
LIBS += /usr/lib/libhighgui.so
LIBS += /usr/lib/libhighgui.so

TEMPLATE = app


SOURCES += main.cpp