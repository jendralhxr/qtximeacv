#-------------------------------------------------
#
# Project created by QtCreator 2016-12-05T14:13:53
#
#-------------------------------------------------

QT       += core gui
CONFIG += qwt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qttest
TEMPLATE = app

include(/usr/local/qwt/features/qwt.prf)

#opencv
INCLUDEPATH += /usr/include/opencv2
LIBS += `pkg-config opencv --libs`

#ximea
INCLUDEPATH += /usr/include/m3api
LIBS += /usr/lib/libm3api.so

SOURCES += main.cpp\
    capturethread.cpp \
    captureplot.cpp \
    renderimage.cpp

HEADERS += \
    capturethread.h \
    captureplot.h \
    renderimage.h
