include(../build.pri)
DESTDIR=$$PWD
QT -= gui
CONFIG += console
SOURCES += pdocmain.cpp
LIBS += -lboost_signals-mt -lboost_thread-mt
include(../cd3core/cd3core.pri)
