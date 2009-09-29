INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += pythonnodefactory.h \
    module.h
SOURCES += pythonnodefactory.cpp \
	module.cpp

#python stuff
QMAKE_CXXFLAGS += $$system(python-config --cflags)
