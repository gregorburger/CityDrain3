include(../build.pri)
TEMPLATE=lib
CONFIG += plugin
LIBS += -lboost_signals-mt -lboost_thread-mt
QT += script
include(cd3core.pri)
