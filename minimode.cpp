#include "minimode.h"

#include "settings.h"
#include "filehelper.h"
#include "model/sqldatabase.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QSlider>
#include <QStyle>

#include <QtDebug>

Minimode::Minimode(QObject *parent)
	: MediaPlayerPlugin(parent)
	, _miniModeWidget(new MiniModeWidget)
{
}

Minimode::~Minimode()
{
	qDebug() << Q_FUNC_INFO;
}

QWidget* Minimode::configPage()
{
	QWidget *widget = new QWidget;
	_config.setupUi(widget);

	// Init the UI with correct values
	bool hasWinampTheme = Settings::instance()->value("MiniMode/hasWinampTheme").toBool();
	_config.winampCheckBox->setChecked(hasWinampTheme);

	auto apply = [this](bool colorIcons) {
		for (QPushButton *b : findChildren<QPushButton*>()) {
			_miniModeWidget->applyColorToStandardIcon(colorIcons, b);
		}
	};
	apply(hasWinampTheme);

	// Connect the UI with the settings
	connect(_config.winampCheckBox, &QCheckBox::toggled, [=](bool b) {
		Settings::instance()->setValue("MiniMode/hasWinampTheme", b);
		apply(b);
	});
	return widget;
}

bool Minimode::eventFilter(QObject *obj, QEvent *e)
{
	if (obj == _miniModeWidget->ui.slider && e->type() == QEvent::MouseMove) {
		_mediaPlayer->seek((qreal) _miniModeWidget->ui.slider->value() / (qreal)100.0);
	} else if (obj == _miniModeWidget->ui.slider && e->type() == QEvent::MouseButtonRelease) {
		_mediaPlayer->setMute(false);
	} else if (obj == _miniModeWidget->ui.slider && e->type() == QEvent::MouseButtonPress) {
		_mediaPlayer->setMute(true);
		_mediaPlayer->seek((qreal) _miniModeWidget->ui.slider->value() / (qreal)100.0);
	}
	return QObject::eventFilter(obj, e);
}

void Minimode::setMediaPlayer(MediaPlayer *mediaPlayer)
{
	_mediaPlayer = mediaPlayer;

	// Multimedia actions
	connect(_miniModeWidget->ui.previous, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::skipBackward);
	connect(_miniModeWidget->ui.play, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::play);
	connect(_miniModeWidget->ui.pause, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::pause);
	connect(_miniModeWidget->ui.stop, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::stop);
	connect(_miniModeWidget->ui.next, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::skipForward);
	connect(_miniModeWidget->ui.slider, &QSlider::valueChanged, this, [=](int v) {
		qDebug() << "slider changed" << v;
	});

	// Windows actions
	connect(_miniModeWidget->ui.minimize, &QPushButton::clicked, [=]() {
		QMainWindow *mw = qobject_cast<QMainWindow*>(_mediaPlayer->parent());
		if (mw) {
			mw->hide();
		}
		_miniModeWidget->showMinimized();
	});
	connect(_miniModeWidget->ui.restore, &QPushButton::clicked, [=]() {
		if (_mediaPlayer->parent()) {
			QMainWindow *mw = qobject_cast<QMainWindow*>(_mediaPlayer->parent());
			mw->showNormal();
			QAction *action = mw->findChild<QAction*>(Settings::instance()->lastActiveView());
			if (action) {
				action->trigger();
			}
			_miniModeWidget->hide();
		}
	});
	connect(_miniModeWidget->ui.close, &QPushButton::clicked, &QApplication::quit);

	connect(_mediaPlayer, &MediaPlayer::currentMediaChanged, [=](const QString &uri) {
		TrackDAO track = SqlDatabase::instance()->selectTrackByURI(uri);
		_miniModeWidget->ui.currentTrack->setText(track.trackNumber().append(" - ").append(track.title()));
	});

	// Reset the label to 0
	connect(_mediaPlayer, &MediaPlayer::stateChanged, [=](QMediaPlayer::State state) {
		if (state == QMediaPlayer::StoppedState) {
			_miniModeWidget->ui.time->setTime(0, _mediaPlayer->duration());
		}
	});

	connect(_mediaPlayer, &MediaPlayer::positionChanged, [=] (qint64 pos, qint64 duration) {
		if (duration > 0) {
			_miniModeWidget->ui.time->setTime(pos, duration);
			if (duration > 0) {
				_miniModeWidget->ui.slider->setValue(pos / duration * 100);
			}
		}
	});
}
