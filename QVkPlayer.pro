#-------------------------------------------------
#
# Project created by QtCreator 2013-06-21T21:07:18
#
#-------------------------------------------------

QT       += core gui multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += webkitwidgets multimedia multimediawidgets

TARGET = QVkPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    vkauth.cpp \
    musiccontrol.cpp

HEADERS  += mainwindow.h \
    vkauth.h \
    musiccontrol.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc