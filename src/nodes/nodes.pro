TEMPLATE = lib
CONFIG += plugin
TARGET = nodes
LIBS += -lcd3core -lboost_thread-mt -lboost_signals-mt
include(nodes.pri)
include(../build.pri)
