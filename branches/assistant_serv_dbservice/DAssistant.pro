# -------------------------------------------------
# Project created by QtCreator 2009-03-02T23:24:47
# -------------------------------------------------
QT += network \
    sql
TARGET = DAssistant
TEMPLATE = app
SOURCES += main.cpp \
    dbservice.cpp \
    mainwindow.cpp \
    serverdaemon.cpp \
    daemonservice.cpp \
    dialog.cpp \
    marksdialog.cpp \
    xlsreader.cpp \
    reportcreator.cpp
HEADERS += mainwindow.h \
    dbservice.h \
    protocol.h \
    serverdaemon.h \
    daemonservice.h \
    dialog.h \
    marksdialog.h \
    xlsreader.h \
    reportcreator.h
FORMS += mainwindow.ui \
    dialog.ui \
    marksdialog.ui
win32:LIBS += -lwsock32
