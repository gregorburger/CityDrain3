INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += pythonnodefactory.h \
    module.h \
    pymodel.h \
    pynode.h \
    pyflow.h
SOURCES += pythonnodefactory.cpp \
    module.cpp \
    pymodel.cpp \
    pynode.cpp \
    pyflow.cpp

# python stuff
QMAKE_CXXFLAGS += $$system(python-config --cflags)
