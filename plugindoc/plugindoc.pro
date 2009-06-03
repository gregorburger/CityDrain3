include(../build.pri)
DESTDIR=$$PWD
QT -= gui
CONFIG += console
SOURCES += pdocmain.cpp
include(../cd3core/cd3core.pri)
