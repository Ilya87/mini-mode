QT      += widgets multimedia sql

TARGET   = $$qtLibraryTarget(mini-mode)
TEMPLATE = lib

DEFINES += MIAM_PLUGIN

CONFIG  += c++11
# TODO: how to minimize hardcoded paths?
win32 {
    MiamPlayerBuildDirectory = C:\dev\Miam-Player-build\qt5.6.0\src\MiamPlayer
    CONFIG(debug, debug|release) {
	target.path = $$MiamPlayerBuildDirectory\debug\plugins
	LIBS += -Ldebug -lMiamCore
    }

    CONFIG(release, debug|release) {
	target.path = $$MiamPlayerBuildDirectory\release\plugins
	LIBS += -Lrelease -lMiamCore
    }
}
unix {
    MiamPlayerBuildDirectory = /home/mbach/Miam-Player-release
    target.path = $$MiamPlayerBuildDirectory/MiamPlayer/plugins
    LIBS += -L$$MiamPlayerBuildDirectory/MiamCore -lmiam-core
}

INSTALLS += target

HEADERS += interfaces/basicplugin.h \
    interfaces/mediaplayerplugin.h \
    model/albumdao.h \
    model/artistdao.h \
    model/genericdao.h \
    model/playlistdao.h \
    model/selectedtracksmodel.h \
    model/sqldatabase.h \
    model/trackdao.h \
    model/yeardao.h \
    filehelper.h \
    mediaplayer.h \
    mediaplaylist.h \
    miamcore_global.h \
    minimode.h \
    settings.h \
    timelabel.h \
    minimodewidget.h

SOURCES += minimode.cpp \
    minimodewidget.cpp

RESOURCES += resources.qrc

FORMS += config.ui \
    mini-mode.ui

TRANSLATIONS += translations/Minimode_ar.ts \
    translations/Minimode_cs.ts \
    translations/Minimode_de.ts \
    translations/Minimode_en.ts \
    translations/Minimode_es.ts \
    translations/Minimode_fr.ts \
    translations/Minimode_it.ts \
    translations/Minimode_ja.ts \
    translations/Minimode_kr.ts \
    translations/Minimode_pt.ts \
    translations/Minimode_ru.ts \
    translations/Minimode_th.ts \
    translations/Minimode_vn.ts \
    translations/Minimode_zh.ts
