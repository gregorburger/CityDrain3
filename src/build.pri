debug {
	DEFINES = DEBUG
}
win32 {
	CONFIG += rtti
	DESTDIR = $$PWD/../win32
	LIBS += -L"$$PWD/../win32"
}

linux* {
	DESTDIR = $$PWD/../
	LIBS += -L$$PWD/../
	LIBS += -L"$$PWD/../3rdparty/boost_1_38_0/stage/lib/"
}
linux-g++* {
	QMAKE_CXXFLAGS += -fopenmp
	LIBS+=-lgomp
}
linux-icc* {
	QMAKE_CXXFLAGS += -openmp
	LIBS += -liomp5
}
INCLUDEPATH += $$PWD/../3rdparty/boost_1_38_0/
QT+=xml
QT-=gui
INCLUDEPATH+=$$PWD/cd3core
INCLUDEPATH+=$$PWD/cd3core/qs
INCLUDEPATH+=$$PWD/python/
OBJECTS_DIR=$$PWD/../tmp/
MOC_DIR=$$PWD/../tmp/
UI_DIR=$$PWD/../tmp/
CONFIG += no_keywords console debug
QMAKE_LFLAGS += -Wl,--allow-shlib-undefined -Wl,--no-undefined

#python stuff

