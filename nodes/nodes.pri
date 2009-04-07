INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += xml \
    script
QMAKE_CXXFLAGS += -fopenmp
LIBS += -lgomp
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
    parallelsimulation.h
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
    parallelsimulation.cpp
include(qs/qs.pri)
