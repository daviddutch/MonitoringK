#-------------------------------------------------
#
# Project created by QtCreator 2011-04-12T14:13:54
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Processing
CONFIG   += console
CONFIG   -= app_bundle


INCLUDEPATH += /usr/include/nite
INCLUDEPATH += /usr/include/ni
INCLUDEPATH += /usr/include/opencv

LIBS += /home/david/kinect/OpenNI/Platform/Linux-x86/Bin/Release/libOpenNI.so
LIBS += /home/david/kinect/Nite-1.3.0.17/Bin/libXnVNite.so
LIBS += /usr/lib/libglut.so

LIBS += /usr/lib/libcv.so
LIBS += /usr/lib/libcvaux.so
LIBS += /usr/lib/libcxcore.so
LIBS += /usr/lib/libhighgui.so
LIBS += /usr/lib/libhighgui.so

TEMPLATE = app


SOURCES += main.cpp \
    signal_catch.cpp \
    sequence.cpp \
    SceneDrawer.cpp \
    movingObject.cpp \
    kbhit.cpp \
    frame.cpp \
    event.cpp

OTHER_FILES +=

HEADERS += \
    signal_catch.h \
    sequence.h \
    SceneDrawer.h \
    movingObject.h \
    kbhit.h \
    frame.h \
    event.h \
    defs.h
