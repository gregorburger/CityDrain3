INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
CONFIG += link_pkgconfig \
    no_keywords

LIBS += -lboost_signals \
    -lboost_filesystem 

HEADERS += calculationunit.h \
    flow.h \
    model.h \
    nodeconnection.h \
    nodefactory.h \
    node.h \
    noderegistry.h \
    simulation.h \
    mapbasedmodel.h \
    cd3typeinfo.h \
    controller.h \
    cd3assert.h \
    simulationfactory.h \
    simulationregistry.h \
    flowbuffer.h \
    cd3utils.h \
    stateserializer.h \
    modelserializer.h \
    flowfuns.h
SOURCES += calculationunit.cpp \
    flow.cpp \
    nodeconnection.cpp \
    node.cpp \
    noderegistry.cpp \
    mapbasedmodel.cpp \
    simulationregistry.cpp \
    flowbuffer.cpp \
    simulation.cpp \
    stateserializer.cpp \
    modelserializer.cpp \
    flowfuns.cpp
