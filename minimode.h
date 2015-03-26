#ifndef MINIMODE_H
#define MINIMODE_H

#include "interfaces/mediaplayerplugin.h"
#include "miamcore_global.h"

#include "mediaplayer.h"

#include <QMouseEvent>
#include <QPushButton>
#include <QWindow>

#include "ui_config.h"
#include "ui_mini-mode.h"

/**
 * \brief       The Minimode class is a mini player like the good old Winamp
 * \author      Matthieu Bachelier
 * \version     1.0
 * \copyright   GNU General Public License v3
 */
class Minimode : public QWidget, public MediaPlayerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID MediaPlayerPlugin_iid)
	Q_INTERFACES(MediaPlayerPlugin)

private:
	Ui::MiniModeConfigPage _config;
	Ui::MiniMode _ui;

	QWeakPointer<MediaPlayer> _mediaPlayer;
	bool _startMoving;
	QPoint _pos, _globalPos;

public:
	explicit Minimode();

	virtual ~Minimode();

	virtual bool eventFilter(QObject *obj, QEvent *e);

	virtual QWidget* configPage();

	inline virtual bool isConfigurable() const { return true; }

	inline virtual QString name() const { return "Minimode"; }

	inline virtual QWidget* providesView() { return this; }

	virtual void setMediaPlayer(QWeakPointer<MediaPlayer> mediaPlayer);

	inline virtual QString version() const { return "1.0"; }

	/** No new extensions are supported with this mini-player. */
	inline virtual QStringList extensions() const { return QStringList(); }

protected:
	/** Redefined to be able to drag this widget on screen. */
	void mouseMoveEvent(QMouseEvent *e);

	/** Redefined to be able to drag this widget on screen. */
	void mouseReleaseEvent(QMouseEvent *e);

	void mousePressEvent(QMouseEvent *e);

private:
	void applyColorToStandardIcon(bool hasTheme, QAbstractButton *button);
};

#endif // MINIMODE_H
