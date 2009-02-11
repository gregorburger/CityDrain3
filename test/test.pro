TEMPLATE=app
TARGET=cd3
DESTDIR=..
QT+=xml
QT-=gui
SOURCES+=main.cpp
include(../build.pri)
include(../cd3core/cd3core.pri)
include(../loader/loader.pri)

