#ifndef MINIMODE_H
#define MINIMODE_H

/// From the API
#include "interfaces/mediaplayerplugin.h"
#include "miamcore_global.h"
#include "mediaplayer.h"

/// From this plugin
#include "ui_config.h"
#include "minimodewidget.h"

/**
 * \brief       The Minimode class is a mini player like the good old Winamp
 * \author      Matthieu Bachelier
 * \version     1.1
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
	MiniModeWidget *_miniModeWidget;

public:
	explicit Minimode(QObject *parent = nullptr);

	virtual ~Minimode();

	virtual QWidget* configPage() override;

	virtual bool eventFilter(QObject *obj, QEvent *e) override;

	inline virtual bool isConfigurable() const override { return true; }

	inline virtual QString name() const override { return "Minimode"; }

	inline virtual QWidget* providesView() override { return _miniModeWidget; }

	virtual void setMediaPlayer(MediaPlayer *mediaPlayer) override;

	inline virtual QString version() const override { return "1.1"; }

	/** No new extensions are supported with this mini-player. */
	inline virtual QStringList extensions() const override { return QStringList(); }
};

#endif // MINIMODE_H
