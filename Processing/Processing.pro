#-------------------------------------------------
#
# Project created by QtCreator 2011-04-12T14:13:54
#
#-------------------------------------------------

QT       += core
QT       += xml
QT       -= gui

TARGET = Processing
CONFIG   += console
CONFIG   -= app_bundle


INCLUDEPATH += /usr/include/nite
INCLUDEPATH += /usr/include/ni
INCLUDEPATH += /usr/include/opencv

INCLUDEPATH += ~/qtsdk-2010.05/qt/include/Qt

LIBS += ~/kinect/OpenNI/Platform/Linux-x86/Bin/Release/libOpenNI.so
LIBS += ~/kinect/Nite-1.3.0.17/Bin/libXnVNite.so

LIBS += /usr/lib/libglut.so
LIBS += /usr/lib/libcv.so
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
    event.cpp \
    generators.cpp \
    processor.cpp \
    tinystr.cpp \
    tinyxml.cpp \
    tinyxmlerror.cpp \
    tinyxmlparser.cpp

OTHER_FILES +=

HEADERS += \
    signal_catch.h \
    sequence.h \
    SceneDrawer.h \
    movingObject.h \
    kbhit.h \
    frame.h \
    event.h \
    defs.h \
    generators.h \
    processor.h \
    tinystr.h \
    tinyxml.h
