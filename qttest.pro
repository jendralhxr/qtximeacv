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

unix{
include(/usr/local/qwt/features/qwt.prf)
#opencv
INCLUDEPATH += /usr/include/opencv2
LIBS += `pkg-config opencv --libs`
#ximea
INCLUDEPATH += /usr/include/m3api
LIBS += /usr/lib/libm3api.so
}

win32{
#opencv
INCLUDEPATH += C:/opencv/build/include
#DEPENDPATH += C:/opencv/build/include
CONFIG(release, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_core2413
else:CONFIG(debug, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_core2413d
CONFIG(release, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_highui2413
else:CONFIG(debug, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_highui2413d
CONFIG(release, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_imgproc2413
else:CONFIG(debug, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_imgproc2413d

#ximea
INCLUDEPATH += C:/XIMEA/API
CONFIG(release, debug|release): LIBS += -LC:/XIMEA/API/x64/ -lm3apiX64
CONFIG(release, debug|release): LIBS += -LC:/XIMEA/API/x64/ -lxiapi64
#DEPENDPATH += C:/XIMEA/API

#qwt
INCLUDEPATH += C:/Qwt/include
CONFIG(release, debug|release): LIBS += -LC:/Qwt/lib/ -lqwt
else:CONFIG(debug, debug|release): LIBS += -LC:/Qwt/lib/ -lqwtd
#DEPENDPATH += C:/Qwt/include
}

SOURCES += main.cpp\
    capturethread.cpp \
    captureplot.cpp \
    renderimage.cpp

HEADERS += \
    capturethread.h \
    captureplot.h \
    renderimage.h




