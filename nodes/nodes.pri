INCLUDEPATH +=  $$PWD
DEPENDPATH  +=  $$PWD

QT += xml

HEADERS += constsource.h \
	flowtypefactory.h \
	fileout.h
SOURCES += nodes.cpp \
	constsource.cpp \
	flowtypefactory.cpp \
	fileout.cpp
