#ifndef MINIMODE_H
#define MINIMODE_H

#include "mediaplayerplugininterface.h"
#include "miamcore_global.h"

#include <mediaplayer.h>

#include <QWindow>

class Minimode : public QWidget, public MediaPlayerPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MediaPlayerPluginInterface_iid)
    Q_INTERFACES(MediaPlayerPluginInterface)

private:
	QWeakPointer<MediaPlayer> _mediaPlayer;

public:
    Minimode();

	inline virtual QString name() const { return "Minimode"; }

	inline virtual QString version() const { return "1.0"; }

	inline virtual bool providesView() const { return true; }

	virtual QWidget *configPage();

	virtual void setMediaPlayer(QWeakPointer<MediaPlayer> mediaPlayer);
};

#endif // MINIMODE_H
