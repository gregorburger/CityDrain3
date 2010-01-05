TEMPLATE = lib
CONFIG += plugin
TARGET = pycd3
INCLUDEPATH += ../cd3core
LIBS += -lcd3core
include(../build.pri)
include(python.pri)

