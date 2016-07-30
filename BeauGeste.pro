TARGET = beaugeste

TEMPLATE = app
CONFIG += console c11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    core.c \
    config.c

LIBS += -linput \
    -ludev \
    -lm \
    -lchibi-scheme

HEADERS += \
    core.h \
    macros.h \
    config.h

DISTFILES += \
    beaugeste_definitions.scm
