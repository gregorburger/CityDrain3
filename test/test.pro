TEMPLATE=app
TARGET=cd3
DESTDIR=..
SOURCES+=main.cpp
QMAKE_LFLAGS_DEBUG += -rdynamic
LIBS += -lcd3core
include(../build.pri)
#include(../cd3core/cd3core.pri)

