#ifndef MINIMODE_H
#define MINIMODE_H

#include "mediaplayerplugininterface.h"
#include "miamcore_global.h"

#include "mediaplayer.h"

#include <QMouseEvent>
#include <QPushButton>
#include <QWindow>

#include "ui_config.h"

/**
 * \brief       The Minimode class is a mini player like the good old Winamp
 * \author      Matthieu Bachelier
 * \version     1.0
 * \copyright   GNU General Public License v3
 */
class Minimode : public QWidget, public MediaPlayerPluginInterface
{
    Q_OBJECT
	Q_PLUGIN_METADATA(IID MediaPlayerPluginInterface_iid)
    Q_INTERFACES(MediaPlayerPluginInterface)

private:
	Ui::ConfigForm _ui;

	QWeakPointer<MediaPlayer> _mediaPlayer;
	bool _startMoving;
	QPoint _pos, _globalPos;

public:
	explicit Minimode();

	virtual ~Minimode();

	virtual QWidget* configPage();

	inline virtual bool isConfigurable() const { return true; }

	inline virtual QString name() const { return "Minimode"; }

	inline virtual bool providesView() const { return true; }

	virtual void setMediaPlayer(QWeakPointer<MediaPlayer> mediaPlayer);

	virtual void toggleViews(QWidget *view);

	inline virtual QString version() const { return "1.0"; }

protected:
	/** Redefined to be able to drag this widget on screen. */
	void mouseMoveEvent(QMouseEvent *e);

	/** Redefined to be able to drag this widget on screen. */
	void mouseReleaseEvent(QMouseEvent *e);

	void mousePressEvent(QMouseEvent *e);

private:
	void applyColorToStandardIcon(QPushButton *button);

};

#endif // MINIMODE_H
