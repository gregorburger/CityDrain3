INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += xml \
    script
HEADERS += constsource.h \
    fileout.h \
    mixer.h \
    defaultsimulation.h \
    vardtsimulation.h \
    sewer.h \
    rainread.h \
    testnode.h \
    cso.h \
    splitter.h \
    catchmentcss.h \
    parallelsimulation.h \
    pipelinedsimulation.h \
    orderedpipesimulation.h \
    null.h \
    bufferednodeconnection.h \
    cyclenodeend.h \
    cyclenodestart.h \
    pumpingstation.h \
    arrayparamtest.h \
    pumpingstation.h
SOURCES += nodes.cpp \
    constsource.cpp \
    fileout.cpp \
    mixer.cpp \
    defaultsimulation.cpp \
    vardtsimulation.cpp \
    sewer.cpp \
    rainread.cpp \
    testnode.cpp \
    cso.cpp \
    splitter.cpp \
    catchmentcss.cpp \
    parallelsimulation.cpp \
    pipelinedsimulation.cpp \
    orderedpipesimulation.cpp \
    null.cpp \
    bufferednodeconnection.cpp \
    cyclenodeend.cpp \
    cyclenodestart.cpp \
    pumpingstation.cpp \
    arrayparamtest.cpp \
    pumpingstation.cpp
include(qs/qs.pri)
