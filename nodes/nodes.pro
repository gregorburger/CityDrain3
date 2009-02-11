TEMPLATE = lib
TARGET = nodes
QT -= gui
QT += xml
HEADERS = constsource.h \
    flowtypefactory.h \
    fileout.h
SOURCES = nodes.cpp \
    constsource.cpp \
    flowtypefactory.cpp \
    fileout.cpp
include(../build.pri)
include(../cd3core/cd3core.pri)
include(../loader/loader.pri)
