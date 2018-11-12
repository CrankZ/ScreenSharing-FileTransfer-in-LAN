#-------------------------------------------------
#
# Project created by QtCreator 2017-08-30T18:15:23
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$PWD/QuaZIP/include
LIBS += -L$$PWD/QuaZIP/lib -lquazip -L$$PWD/zlib128-dll/ -lzlib1

TARGET = SlaveClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        widget.cpp \
    showscreen.cpp \
    showdownload.cpp \
    checkin.cpp \
    examtest.cpp \
    screensharing.cpp \
    receiver.cpp \
    sender.cpp \
    singleapplication.cpp \
    prohibitor.cpp

HEADERS  += widget.h \
    showscreen.h \
    showdownload.h \
    checkin.h \
    justenum.h \
    examtest.h \
    examinfo.h \
    screensharing.h \
    receiver.h \
    sender.h \
    singleapplication.h \
    stuinfo.h \
    prohibitor.h

FORMS    += widget.ui \
    showscreen.ui \
    showdownload.ui \
    checkin.ui \
    examtest.ui

LIBS += -luser32 -lshell32 -lgdi32
RC_FILE = rc.rc


RESOURCES += \
    resource.qrc
