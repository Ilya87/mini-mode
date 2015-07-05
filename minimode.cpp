#include "minimode.h"

#include "settings.h"
#include "filehelper.h"
#include "model/sqldatabase.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QPainter>
#include <QPixmap>
#include <QSlider>
#include <QStyle>

#include <QtDebug>

Minimode::Minimode()
	: QWidget(nullptr), _startMoving(false), _pos(0, 0)
{
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	this->setWindowIcon(QIcon(":/icons/mmmmp_win32"));

	_ui.setupUi(this);

	// Media buttons
	_ui.previous->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
	_ui.play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	_ui.pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	_ui.stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	_ui.next->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

	// Window management
	_ui.minimize->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
	_ui.restore->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
	_ui.close->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));

	_ui.slider->installEventFilter(this);
}

Minimode::~Minimode() {}

bool Minimode::eventFilter(QObject *obj, QEvent *e)
{
	if (obj == _ui.slider && e->type() == QEvent::MouseMove) {
		_mediaPlayer->seek((float) _ui.slider->value() / 100.0);
	} else if (obj == _ui.slider && e->type() == QEvent::MouseButtonRelease) {
		_mediaPlayer->setMute(false);
	} else if (obj == _ui.slider && e->type() == QEvent::MouseButtonPress) {
		_mediaPlayer->setMute(true);
		_mediaPlayer->seek((float) _ui.slider->value() / 100.0);
	}
	return QWidget::eventFilter(obj, e);
}

QWidget* Minimode::configPage()
{
	QWidget *widget = new QWidget;
	_config.setupUi(widget);

	// Init the UI with correct values
	bool hasWinampTheme = Settings::instance()->value("hasWinampTheme").toBool();
	_config.winampCheckBox->setChecked(hasWinampTheme);

	auto apply = [this](bool colorIcons) {
		for (QPushButton *b : findChildren<QPushButton*>()) {
			this->applyColorToStandardIcon(colorIcons, b);
		}
	};
	apply(hasWinampTheme);

	// Connect the UI with the settings
	connect(_config.winampCheckBox, &QCheckBox::toggled, [=](bool b) {
		Settings::instance()->setValue("hasWinampTheme", b);
		apply(b);
	});
	return widget;
}

void Minimode::setMediaPlayer(MediaPlayer *mediaPlayer)
{
	_mediaPlayer = mediaPlayer;

	// Multimedia actions
	connect(_ui.previous, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::skipBackward);
	connect(_ui.play, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::play);
	connect(_ui.pause, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::pause);
	connect(_ui.stop, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::stop);
	connect(_ui.next, &QPushButton::clicked, _mediaPlayer, &MediaPlayer::skipForward);
	connect(_ui.slider, &QSlider::valueChanged, this, [=](int) {
		//qDebug() << "slider changed" << v;
	});

	// Windows actions
	connect(_ui.minimize, &QPushButton::clicked, [=]() {
		QMainWindow *mw = qobject_cast<QMainWindow*>(_mediaPlayer->parent());
		if (mw) {
			mw->hide();
		}
		this->showMinimized();
	});
	connect(_ui.restore, &QPushButton::clicked, [=]() {
		if (_mediaPlayer->parent()) {
			QMainWindow *mw = qobject_cast<QMainWindow*>(_mediaPlayer->parent());
			mw->showNormal();
			this->hide();
			QAction *action = mw->findChild<QAction*>(Settings::instance()->lastActiveView());
			if (action) {
				action->trigger();
			}
		}
	});
	connect(_ui.close, &QPushButton::clicked, &QApplication::quit);

	connect(_mediaPlayer, &MediaPlayer::currentMediaChanged, [=](const QString &uri) {
		TrackDAO track = SqlDatabase::instance()->selectTrackByURI(uri);
		_ui.currentTrack->setText(track.trackNumber().append(" - ").append(track.title()));
	});

	connect(_mediaPlayer, &MediaPlayer::positionChanged, [=] (qint64 pos, qint64 duration) {
		if (duration > 0) {
			_ui.time->setTime(pos, duration);
			if (duration > 0) {
				_ui.slider->setValue(pos / duration * 100);
			}
		}
	});
}

/** Redefined to be able to drag this widget on screen. */
void Minimode::mouseMoveEvent(QMouseEvent *e)
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
void Minimode::mouseReleaseEvent(QMouseEvent *e)
{
	// Reset the position
	_startMoving = false;
	_pos = QPoint();
	QWidget::mouseReleaseEvent(e);
}

void Minimode::mousePressEvent(QMouseEvent *e)
{
	_startMoving = true;
	// Keep a reference from one's cursor
	_pos = e->pos();
	_globalPos = e->globalPos();
	//}
	QWidget::mousePressEvent(e);
}

void Minimode::applyColorToStandardIcon(bool hasTheme, QAbstractButton *button)
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
		_ui.title->setStyleSheet("color: white;");
		_ui.currentTrack->setStyleSheet("color: #00ff00;");
		_ui.time->setStyleSheet("color: #00ff00;");
		this->setStyleSheet("background: #39395A;");
		_ui.slider->setStyleSheet("QSlider::groove:horizontal  { \
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
		_ui.title->setStyleSheet("");
		_ui.currentTrack->setStyleSheet("");
		_ui.time->setStyleSheet("");
		this->setStyleSheet("");
		_ui.slider->setStyleSheet("QSlider::groove:horizontal  { \
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
