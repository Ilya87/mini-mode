QT      += widgets multimedia

TARGET   = $$qtLibraryTarget(mini-mode)
TEMPLATE = lib

MiamPlayerBuildDirectory = C:\dev\Madame-Miam-Miam-Music-Player\build\MiamPlayer

DEFINES += MIAM_PLUGIN

CONFIG  += c++11
CONFIG(debug, debug|release) {
    target.path = $$MiamPlayerBuildDirectory\debug\plugins
    LIBS += -Ldebug -lMiamCore
}

CONFIG(release, debug|release) {
    target.path = $$MiamPlayerBuildDirectory\release\plugins
    LIBS += -Lrelease -lMiamCore
}

INSTALLS += target

HEADERS += basicplugininterface.h \
    mediaplayer.h \
    mediaplayerplugininterface.h \
    miamcore_global.h \
    minimode.h \
    settings.h

SOURCES += minimode.cpp

RESOURCES += \
    resources.qrc

FORMS += \
    config.ui
