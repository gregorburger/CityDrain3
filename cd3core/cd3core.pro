TEMPLATE=lib
CONFIG += plugin
LIBS += -lboost_signals-mt -lboost_thread-mt
QT += script
include(../build.pri)
include(cd3core.pri)
