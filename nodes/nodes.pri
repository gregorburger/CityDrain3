INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
VEIGHT = /home/gregor/download/v8
INCLUDEPATH += $$VEIGHT/include
LIBS += $$VEIGHT/libv8_g.a
QT += xml script
HEADERS += constsource.h \
    flowtypefactory.h \
    fileout.h \
    mixer.h \
    defaultsimulation.h \
    vardtsimulation.h \
    sewer.h \
    jsnode.h \
    jsbindings.h \
    v8cppwrapper.h \
    rainread.h
SOURCES += nodes.cpp \
    constsource.cpp \
    flowtypefactory.cpp \
    fileout.cpp \
    mixer.cpp \
    defaultsimulation.cpp \
    vardtsimulation.cpp \
    sewer.cpp \
    jsnode.cpp \
    jsbindings.cpp \
    rainread.cpp
include(qs/qs.pri)
