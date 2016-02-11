QT      += widgets multimedia sql

TARGET   = $$qtLibraryTarget(mini-mode)
TEMPLATE = lib

DEFINES += MIAM_PLUGIN

CONFIG  += plugin c++11
# TODO: how to minimize hardcoded paths?
win32 {
    MiamPlayerBuildDirectory = C:\dev\Miam-Player-build\src\Player
    CONFIG(debug, debug|release) {
	target.path = $$MiamPlayerBuildDirectory\debug\plugins
	LIBS += -Ldebug -lCore
    }

    CONFIG(release, debug|release) {
	target.path = $$MiamPlayerBuildDirectory\release\plugins
	LIBS += -Lrelease -lCore
    }
}
unix {
    #QMAKE_CXXFLAGS += -std=c++11
}
unix:!macx {
    MiamPlayerBuildDirectory = /home/mbach/Miam-Player-release
    target.path = $$MiamPlayerBuildDirectory/MiamPlayer/plugins
    LIBS += -L$$MiamPlayerBuildDirectory/Core -lmiam-core
}
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    MiamPlayerBuildDirectory = /Users/mbach/dev/Miam-Player-build/src
    target.path = $$MiamPlayerBuildDirectory/Player/MiamPlayer.app/Contents/PlugIns
    LIBS += -L$$MiamPlayerBuildDirectory/Core -lmiam-core
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
    widgets/timelabel.h \
    abstractview.h \
    filehelper.h \
    mediaplayer.h \
    mediaplaylist.h \
    miamcore_global.h \
    minimode.h \
    minimodewidget.h \
    settings.h

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
