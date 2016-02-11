#ifndef MINIMODE_H
#define MINIMODE_H

/// From the API
#include "interfaces/mediaplayerplugin.h"
#include "miamcore_global.h"
#include "mediaplayer.h"

/// From this plugin
#include "ui_config.h"

/**
 * \brief       The Minimode class is a mini player like the good old Winamp
 * \author      Matthieu Bachelier
 * \version     1.2
 * \copyright   GNU General Public License v3
 */
class Minimode : public MediaPlayerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID MediaPlayerPlugin_iid)
	Q_INTERFACES(MediaPlayerPlugin)

private:
	Ui::MiniModeConfigPage _config;
	MediaPlayer *_mediaPlayer;

public:
	explicit Minimode(QObject *parent = nullptr);

	virtual ~Minimode();

	virtual QWidget* configPage() override;

	inline virtual bool isConfigurable() const override { return true; }

	inline virtual QString name() const override { return "Minimode"; }

	virtual AbstractView* instanciateView() override;

	virtual void setMediaPlayer(MediaPlayer *mediaPlayer) override;

	inline virtual QString version() const override { return "1.1"; }

	/** No new extensions are supported with this mini-player. */
	inline virtual QStringList extensions() const override { return QStringList(); }
};

#endif // MINIMODE_H
