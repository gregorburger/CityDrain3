TEMPLATE=app
TARGET=cd3
DESTDIR=..
SOURCES+=main.cpp
LIBS += -lcd3core -lboost_program_options-mt -lboost_signals-mt -lboost_thread-mt
include(../build.pri)
#include(../cd3core/cd3core.pri)

