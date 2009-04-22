INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += xml \
    script
HEADERS += constsource.h \
    flowtypefactory.h \
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
    null.h
SOURCES += nodes.cpp \
    constsource.cpp \
    flowtypefactory.cpp \
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
    null.cpp
include(qs/qs.pri)
