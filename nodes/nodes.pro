TEMPLATE = lib
CONFIG += dll
TARGET = nodes
QT -= gui
QT += xml
include(nodes.pri)
include(../build.pri)
include(../cd3core/cd3core.pri)
include(../loader/loader.pri)
