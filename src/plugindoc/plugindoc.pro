include(../build.pri)
DESTDIR=$$PWD
QT -= gui
CONFIG += console
SOURCES += pdocmain.cpp
LIBS += -lcd3core -lboost_signals-mt -lboost_thread-mt -lpycd3
