#-------------------------------------------------
#
# Project created by QtCreator 2012-08-14T18:09:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = z80ex
TEMPLATE = lib

DEFINES += Z80EX_LIBRARY

SOURCES += \
    ../ptables.c \
    ../z80ex.c

HEADERS += \
    ../macros.h \
    ../typedefs.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE3B78D58
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = z80ex.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

DEFINES += Z80EX_API_REVISION
DEFINES += Z80EX_VERSION_MAJOR
DEFINES += Z80EX_VERSION_MINOR

