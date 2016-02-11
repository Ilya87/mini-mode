#include "minimodewidget.h"

#include "model/sqldatabase.h"
#include "settings.h"

#include <QDesktopWidget>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>

#include <QtDebug>

MiniModeWidget::MiniModeWidget(MediaPlayer *mediaPlayer, QWidget *parent)
	: AbstractView(mediaPlayer, parent)
	, _startMoving(false)
	, _pos(0, 0)
{
	//this->setAttribute(Qt::WA_DeleteOnClose, true);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	this->setWindowIcon(QIcon(":/icons/mp_win32"));

	ui.setupUi(this);

	// Media buttons
	ui.previous->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
	ui.playPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui.stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui.next->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

	QMap<QString, QVariant> shortcutMap = Settings::instance()->shortcuts();
	ui.previous->setShortcut(QKeySequence(shortcutMap.value("skipBackward").toString()));
	ui.playPause->setShortcut(QKeySequence(shortcutMap.value("play").toString()));
	ui.stop->setShortcut(QKeySequence(shortcutMap.value("stop").toString()));
	ui.next->setShortcut(QKeySequence(shortcutMap.value("skipForward").toString()));

	// Window management
	ui.minimize->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
	ui.restore->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
	ui.close->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));

	ui.slider->installEventFilter(this);
	this->installEventFilter(this);

	// Multimedia actions
	connect(ui.previous, &QPushButton::clicked, this, &MiniModeWidget::skipBackward);
	connect(ui.playPause, &QPushButton::clicked, this, &MiniModeWidget::playPause);
	connect(ui.stop, &QPushButton::clicked, this, &MiniModeWidget::stop);
	connect(ui.next, &QPushButton::clicked, this, &MiniModeWidget::skipForward);
	connect(ui.slider, &QSlider::valueChanged, this, [=](int v) {
		qDebug() << "slider changed" << v;
	});

	// Windows actions
	connect(ui.minimize, &QPushButton::clicked, this, &MiniModeWidget::showMinimized);
	connect(ui.restore, &QPushButton::clicked, [=]() {
		/// FIXME: magic!
		if (_mediaPlayer->parent()) {
			QMainWindow *mw = qobject_cast<QMainWindow*>(_mediaPlayer->parent());
			mw->showNormal();
			QAction *action = mw->findChild<QAction*>("actionViewPlaylists");
			if (action) {
				action->trigger();
			}
			//hide();
		}
	});
	connect(ui.close, &QPushButton::clicked, &QApplication::quit);

	connect(mediaPlayer, &MediaPlayer::currentMediaChanged, [=](const QString &uri) {
		SqlDatabase db;
		TrackDAO track = db.selectTrackByURI(uri);
		ui.currentTrack->setText(track.trackNumber().append(" - ").append(track.title()));
	});

	connect(mediaPlayer, &MediaPlayer::stateChanged, [=](QMediaPlayer::State state) {
		switch (state) {
		case QMediaPlayer::StoppedState:
			// Reset the label to 0
			ui.time->setTime(0, mediaPlayer->duration());
			ui.playPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
			break;
		case QMediaPlayer::PlayingState:
			ui.playPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
			break;
		case QMediaPlayer::PausedState:
			ui.playPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
			break;
		}
	});

	connect(mediaPlayer, &MediaPlayer::positionChanged, [=] (qint64 pos, qint64 duration) {
		if (duration > 0) {
			ui.time->setTime(pos, duration);
			if (duration > 0) {
				ui.slider->setValue(pos / duration * 100);
			}
		}
	});
}

bool MiniModeWidget::eventFilter(QObject *obj, QEvent *e)
{
	if (obj == ui.slider && e->type() == QEvent::MouseMove) {
		_mediaPlayer->seek((qreal) ui.slider->value() / (qreal)100.0);
	} else if (obj == ui.slider && e->type() == QEvent::MouseButtonRelease) {
		_mediaPlayer->setMute(false);
	} else if (obj == ui.slider && e->type() == QEvent::MouseButtonPress) {
		_mediaPlayer->setMute(true);
		_mediaPlayer->seek((qreal) ui.slider->value() / (qreal)100.0);
	}
	return QObject::eventFilter(obj, e);
}

bool MiniModeWidget::viewProperty(Settings::ViewProperty vp) const
{
	switch (vp) {
	case Settings::VP_MediaControls:
	case Settings::VP_SearchArea:
	case Settings::VP_HideMenuBar:
	case Settings::VP_HasTracksToDisplay:
		return true;
	default:
		return false;
	}
}

void MiniModeWidget::closeEvent(QCloseEvent *)
{
	qDebug() << Q_FUNC_INFO << "about to quit?";
	QCoreApplication::instance()->quit();
}

/** Redefined to be able to drag this widget on screen. */
void MiniModeWidget::mouseMoveEvent(QMouseEvent *e)
{
	static int _OFFSET = 15;
	if (_startMoving) {
		/// TODO multiple screens
		// Detect when one has changed from one screen to another
		if (e->pos().x() < 0 || e->pos().y() < 0) {
			//qDebug() << "todo multiple screens";
		}
		const QRect screen = QApplication::desktop()->screenGeometry();
		// Top edge screen
		if (frameGeometry().top() - screen.top() <= _OFFSET && (e->globalPos().y() - _pos.y()) <= _OFFSET) {
			//qDebug() << "top edge" << frameGeometry() << _pos << e->globalPos();
			move(e->globalPos().x() - _pos.x(), 0);
		} else if (screen.right() - frameGeometry().right() <= _OFFSET && (_globalPos.x() - e->globalPos().x()) <= _OFFSET) { // Right edge screen
			//qDebug() << "right edge" << frameGeometry() << _pos << e->globalPos();
			move(screen.right() - frameGeometry().width() + 1, e->globalPos().y() - _pos.y());
			_globalPos = e->globalPos();
		} else if (screen.bottom() - frameGeometry().bottom() <= _OFFSET && (_pos.y() - e->globalPos().y()) <= _OFFSET) { // Bottom edge screen
			//qDebug() << "bottom edge" << frameGeometry() << _pos << e->globalPos();
			move(e->globalPos().x() - _pos.x(), 0);
		} else if (frameGeometry().left() - screen.left() <= _OFFSET && (e->globalPos().x() - _pos.x()) <= _OFFSET) { // Left edge screen
			//qDebug() << "left edge" << frameGeometry() << _pos << e->globalPos();
			move(0, e->globalPos().y() - _pos.y());
		} else { // Inside the screen
			//qDebug() << "inside screen" << frameGeometry();
			// Substract the click in the middle of this widget because we don't want to move() to (0,0)
			move(e->globalPos().x() - _pos.x(), e->globalPos().y() - _pos.y());
		}
	}
	QWidget::mouseMoveEvent(e);
}

/** Redefined to be able to drag this widget on screen. */
void MiniModeWidget::mouseReleaseEvent(QMouseEvent *e)
{
	// Reset the position
	_startMoving = false;
	_pos = QPoint();
	QWidget::mouseReleaseEvent(e);
}

void MiniModeWidget::mousePressEvent(QMouseEvent *e)
{
	_startMoving = true;
	// Keep a reference from one's cursor
	_pos = e->pos();
	_globalPos = e->globalPos();
	//}
	QWidget::mousePressEvent(e);
}

void MiniModeWidget::setViewProperty(Settings::ViewProperty vp, QVariant value)
{
	Q_UNUSED(vp)
	Q_UNUSED(value)
}

void MiniModeWidget::applyColorToStandardIcon(bool hasTheme, QAbstractButton *button)
{
	QPixmap sourcePixmap(8, 8);
	QPixmap destinationPixmap = button->icon().pixmap(8, 8);
	QPixmap resultPixmap = QPixmap(destinationPixmap);

	QPainter painter(&resultPixmap);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultPixmap.rect(), Qt::transparent);

	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, destinationPixmap);

	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	if (hasTheme) {
		sourcePixmap.fill(QColor(236, 206, 122));
		ui.title->setStyleSheet("color: white;");
		ui.currentTrack->setStyleSheet("color: #00ff00;");
		ui.time->setStyleSheet("color: #00ff00;");
		this->setStyleSheet("background: #39395A;");
		ui.slider->setStyleSheet("QSlider::groove:horizontal  { \
									  border: 1px solid #999999; \
									  border-top-color: #000010; \
									  border-right-color: #5A6B7B; \
									  border-bottom-color: #5A6B7B; \
									  border-left-color: #000010; \
									  background: #39395A; \
									  margin: 0; \
								  } \
								  QSlider::handle:horizontal  { \
									  background: 1px solid #ECCE7A; \
									  border-radius: 1px; \
									  margin-top: -1px; \
									  margin-bottom: -1px; \
									  width: 3px; \
								  }");
	} else {
		sourcePixmap.fill(QApplication::palette().text().color());
		ui.title->setStyleSheet("");
		ui.currentTrack->setStyleSheet("");
		ui.time->setStyleSheet("");
		this->setStyleSheet("");
		ui.slider->setStyleSheet("QSlider::groove:horizontal  { \
									  border: 1px solid #999999; \
									  border-top-color: #000010; \
									  border-right-color: #5A6B7B; \
									  border-bottom-color: #5A6B7B; \
									  border-left-color: #000010; \
									  margin: 0; \
								  } \
								  QSlider::handle:horizontal  { \
									  background: 1px solid #000010; \
									  border-radius: 1px; \
									  margin-top: -1px; \
									  margin-bottom: -1px; \
									  width: 3px; \
								  }");
	}
	painter.drawPixmap(0, 0, sourcePixmap);

	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.fillRect(resultPixmap.rect(), Qt::transparent);
	painter.end();

	button->setIcon(resultPixmap);
}
