INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += flow.h \
	imodel.h \
    nodeconnection.h \
    nodefactory.h \
    node.h \
    noderegistry.h \
    simulation.h \
    mapbasedmodel.h \
    cd3typeinfo.h \
    cd3assert.h \
    simulationfactory.h \
    simulationregistry.h \
    flowbuffer.h \
    cd3utils.h \
    stateserializer.h \
    modelserializer.h \
    flowfuns.h \
    typeregistry.h \
    typefactory.h \
    cd3globals.h \
    tqueue.h \
    log.h \
	logger.h
SOURCES += flow.cpp \
    nodeconnection.cpp \
    node.cpp \
    noderegistry.cpp \
    mapbasedmodel.cpp \
    simulationregistry.cpp \
    flowbuffer.cpp \
    simulation.cpp \
    stateserializer.cpp \
    modelserializer.cpp \
    flowfuns.cpp \
    log.cpp \
	logger.cpp
