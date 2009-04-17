DEFINES = DEBUG
win32 {
    CONFIG += release rtti
    DESTDIR=$$PWD/win32
}

linux* {
    DESTDIR=$$PWD
    LIBS += -L$$PWD
    CONFIG+=debug
}
linux-g++* {
        QMAKE_CXXFLAGS += -fopenmp 
        LIBS+=-lgomp
}
linux-icc* {
        QMAKE_CXXFLAGS += -openmp -openmp-lib=compat -O3 -xHOST
        LIBS += -liomp5
}
QT+=xml
QT-=gui
INCLUDEPATH+=$$PWD/cd3core
OBJECTS_DIR=$$PWD/tmp/
MOC_DIR=$$PWD/tmp/
UI_DIR=$$PWD/tmp/
CONFIG += no_keywords
