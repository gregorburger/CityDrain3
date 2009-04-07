DEFINES = DEBUG
win32 {
    CONFIG += release rtti
    DESTDIR=$$PWD/win32
}

linux-g++ {
    DESTDIR=$$PWD
    CONFIG+=debug
}
OBJECTS_DIR=$$PWD/tmp/
MOC_DIR=$$PWD/tmp/
UI_DIR=$$PWD/tmp/
