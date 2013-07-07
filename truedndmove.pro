#-------------------------------------------------
#
# Project created by QtCreator 2012-07-06T14:54:47
#
#-------------------------------------------------

TARGET = truedndmove
TEMPLATE = app

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): {
    DEFINES += IS_QT5
    IS_QT5 = 1

    QT += widgets
    *-clang*: cache()
}

CONFIG(release, debug|release): {
    DEFINES += QT_NO_DEBUG_OUTPUT \
               QT_NO_WARNING_OUTPUT
}


SOURCES += main.cpp\
           widget.cpp \
           tableview.cpp \
           tablemodel.cpp

HEADERS  += widget.h \
            tableview.h \
            tablemodel.h

RESOURCES += resources.qrc
