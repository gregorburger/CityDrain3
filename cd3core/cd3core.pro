TEMPLATE=lib
CONFIG += plugin
LIBS += -lboost_signals-mt -lboost_thread-mt
include(../build.pri)
include(cd3core.pri)
