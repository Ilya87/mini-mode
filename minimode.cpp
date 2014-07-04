#include "minimode.h"

#include "settings.h"
#include "filehelper.h"

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
	: QWidget(NULL), _startMoving(false), _pos(0, 0)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
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
}

Minimode::~Minimode()
{}

QWidget* Minimode::configPage()
{
	QWidget *widget = new QWidget();
	_config.setupUi(widget);

	// Init the UI with correct values
	bool hasWinampTheme = Settings::getInstance()->value("hasWinampTheme").toBool();
	_config.winampCheckBox->setChecked(hasWinampTheme);

	auto apply = [this](bool colorIcons) {
		foreach (QPushButton *b, findChildren<QPushButton*>()) {
			this->applyColorToStandardIcon(colorIcons, b);
		}
	};
	apply(hasWinampTheme);


	// Connect the UI with the settings
	connect(_config.winampCheckBox, &QCheckBox::toggled, [=](bool b) {
		Settings::getInstance()->setValue("hasWinampTheme", b);
		apply(b);
	});
	return widget;
}

void Minimode::setMediaPlayer(QWeakPointer<MediaPlayer> mediaPlayer)
{
	_mediaPlayer = mediaPlayer;

	// Multimedia actions
	connect(_ui.previous, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::skipBackward);
	connect(_ui.play, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::play);
	connect(_ui.pause, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::pause);
	connect(_ui.stop, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::stop);
	connect(_ui.next, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::skipForward);

	// Windows actions
	connect(_ui.minimize, &QPushButton::clicked, [=]() {
		QMainWindow *mw = qobject_cast<QMainWindow*>(_mediaPlayer.data()->parent());
		if (mw) {
			mw->close();
		}
		this->showMinimized();
	});
	connect(_ui.restore, &QPushButton::clicked, [=]() {
		QMainWindow *mw = qobject_cast<QMainWindow*>(_mediaPlayer.data()->parent());
		if (mw) {
			mw->showNormal();
		}
		this->close();
	});
	connect(_ui.close, &QPushButton::clicked, &QApplication::quit);

	connect(_mediaPlayer.data(), &MediaPlayer::currentMediaChanged, [=](const QMediaContent &media) {
		FileHelper fh(media);
		_ui.currentTrack->setText(fh.trackNumber().append(" - ").append(fh.title()));
	});

	connect(_mediaPlayer.data(), &MediaPlayer::positionChanged, [=] (qint64 pos) {
		if (_mediaPlayer.data()->duration() > 0) {
			_ui.time->setTime(pos, _mediaPlayer.data()->duration());
		}
	});
}

/*void Minimode::toggleViews(QWidget *view)
{
	view->close();
	this->show();
}*/

/** Redefined to be able to drag this widget on screen. */
void Minimode::mouseMoveEvent(QMouseEvent *e)
{
	static int _OFFSET = 15;
	if (_startMoving) {
		const QRect screen = QApplication::desktop()->screenGeometry();
		// Top edge screen
		if (frameGeometry().top() - screen.top() <= _OFFSET && (e->globalPos().y() - _pos.y()) <= _OFFSET) {
			qDebug() << "top edge" << frameGeometry() << _pos << e->globalPos();
			move(e->globalPos().x() - _pos.x(), 0);
		} else if (screen.right() - frameGeometry().right() <= _OFFSET && (_globalPos.x() - e->globalPos().x()) <= _OFFSET) { // Right edge screen
			qDebug() << "right edge" << frameGeometry() << _pos << e->globalPos();
			move(screen.right() - frameGeometry().width() + 1, e->globalPos().y() - _pos.y());
			_globalPos = e->globalPos();
		} else if (screen.bottom() - frameGeometry().bottom() <= _OFFSET && (_pos.y() - e->globalPos().y()) <= _OFFSET) { // Bottom edge screen
			qDebug() << "bottom edge" << frameGeometry() << _pos << e->globalPos();
			move(e->globalPos().x() - _pos.x(), 0);
		} else if (frameGeometry().left() - screen.left() <= _OFFSET && (e->globalPos().x() - _pos.x()) <= _OFFSET) { // Left edge screen
			qDebug() << "left edge" << frameGeometry() << _pos << e->globalPos();
			move(0, e->globalPos().y() - _pos.y());
		} else { // Inside the screen
			qDebug() << "inside screen" << frameGeometry();
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
	qDebug() << Q_FUNC_INFO << e->pos() << e->globalPos();
	//if (_startMoving == false) {
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
