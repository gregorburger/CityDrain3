include(../build.pri)
TEMPLATE=lib
win32* {
	CONFIG += dll
	message("as dll")
}
linux* {
	CONFIG += staticlib
	message("as static lib")
}
LIBS += -lboost_signals-mt -lboost_thread-mt
QT += script
include(cd3core.pri)
