INCLUDEPATH += $$PWD

win32 {
    LIBS += -lShell32
}

HEADERS += \
    $$PWD/OSApi.h

SOURCES += \
    $$PWD/LinuxApi.cpp \
    $$PWD/MacApi.cpp \
    $$PWD/OSApi.cpp \
    $$PWD/WinApi.cpp

