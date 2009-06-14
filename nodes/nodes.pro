TEMPLATE = lib
CONFIG += plugin
TARGET = nodes
LIBS += -lcd3core -lboost_thread-mt -lboost_signals
include(nodes.pri)
include(../build.pri)
