TEMPLATE = lib
TARGET = nodes
CONFIG += debug
QT -= gui
QT += xml

include(nodes.pri)
include(../build.pri)
include(../cd3core/cd3core.pri)
include(../loader/loader.pri)
