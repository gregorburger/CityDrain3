include(../build.pri)
TEMPLATE=app
TARGET=cd3
SOURCES+=main.cpp
LIBS += -lboost_program_options-mt -lboost_signals-mt -lboost_thread-mt -lcd3core
#include(../cd3core/cd3core.pri)

