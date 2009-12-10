INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += pythonnodefactory.h \
    module.h \
    pymodel.h
SOURCES += pythonnodefactory.cpp \
    module.cpp \
    pymodel.cpp

#python stuff
QMAKE_CXXFLAGS += $$system(python-config --cflags)
