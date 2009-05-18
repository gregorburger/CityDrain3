DEFINES = DEBUG
win32 {
	CONFIG += rtti
    DESTDIR = $$PWD/win32
    LIBS += -L$$PWD/win32
}

linux* {
    DESTDIR = $$PWD
    LIBS += -L$$PWD
}
linux-g++* {
        QMAKE_CXXFLAGS += -fopenmp 
        LIBS+=-lgomp
}
linux-icc* {
        QMAKE_CXXFLAGS += -openmp
        LIBS += -liomp5
}
INCLUDEPATH += $$PWD/3rdparty/boost_1_38_0/
LIBS += -L$$PWD/3rdparty/boost_1_38_0/stage/lib/
QT+=xml
QT-=gui
INCLUDEPATH+=$$PWD/cd3core
OBJECTS_DIR=$$PWD/tmp/
MOC_DIR=$$PWD/tmp/
UI_DIR=$$PWD/tmp/
CONFIG += no_keywords console debug
