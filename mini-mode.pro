QT       += widgets multimedia

QMAKE_CXXFLAGS += -std=c++11

TARGET = mini-mode
TEMPLATE = lib

target.path = C:\dev\Madame-Miam-Miam-Music-Player\build\MiamPlayer\release\plugins
INSTALLS += target

DEFINES += MINIMODE_LIBRARY

SOURCES += minimode.cpp

HEADERS += minimode.h\
    basicplugininterface.h \
    mediaplayer.h \
    mediaplayerplugininterface.h \
    miamcore_global.h

CONFIG  += plugin
CONFIG(debug, debug|release) {
    LIBS += -debug -lMiamCore
}

CONFIG(release, debug|release) {
    LIBS += -Lrelease -lMiamCore
}

RESOURCES +=
