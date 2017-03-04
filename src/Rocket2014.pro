#-------------------------------------------------
#
# Project created by QtCreator 2016-07-02T23:32:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Rocket2014
TEMPLATE = app

DEFINES += _CRT_SECURE_NO_WARNINGS

# z80ex stuff
DEFINES += WORDS_LITTLE_ENDIAN
DEFINES += "Z80EX_VERSION_STR=custom"
DEFINES += "Z80EX_API_REVISION=1"
DEFINES += "Z80EX_VERSION_MAJOR=1"
DEFINES += "Z80EX_VERSION_MINOR=19"
DEFINES += "Z80EX_RELEASE_TYPE=custom"

INCLUDEPATH += ../contrib/z80ex-1.1.19/include

SOURCES += main.cpp\
        mainwindow.cpp \
        textblitter.cpp \
        consoleview.cpp \
        ansiconsoleview.cpp \
        disasmview.cpp \
        registerwindow.cpp \
        z80systemthread.cpp \
        ../contrib/z80ex-1.1.19/z80ex.c \
        ../contrib/z80ex-1.1.19/z80ex_dasm.c \
        z80system.cpp \
        z80systembase.cpp \
        z80cpubase.cpp

HEADERS  += mainwindow.h \
            textblitter.h \
            consoleview.h \
            ansiconsoleview.h \
            disasmmodel.h \
            disasmview.h \
            registerwindow.h \
            z80systemthread.h \
            ../contrib/z80ex-1.1.19/include/z80ex.h \
            ../contrib/z80ex-1.1.19/include/z80ex_dasm.h \
            z80system.h \
            z80systembase.h \
            z80cpubase.h

FORMS    += mainwindow.ui

