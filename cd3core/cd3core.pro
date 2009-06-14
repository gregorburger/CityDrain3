include(../build.pri)
TEMPLATE=lib
CONFIG += staticlib
LIBS += -lboost_signals-mt -lboost_thread-mt
QT += script
include(cd3core.pri)
