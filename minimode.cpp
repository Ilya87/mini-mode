#include "minimode.h"

#include "settings.h"

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
	this->setMaximumSize(275, 28);

	QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
	vBoxLayout->setObjectName("vBoxLayout");
	QHBoxLayout *topHBoxLayout = new QHBoxLayout(this);
	topHBoxLayout->setObjectName("topHBoxLayout");
	QHBoxLayout *bottomHBoxLayout = new QHBoxLayout(this);
	bottomHBoxLayout->setObjectName("bottomHBoxLayout");

	vBoxLayout->setContentsMargins(0, 0, 0, 0);
	vBoxLayout->setSpacing(0);
	topHBoxLayout->setContentsMargins(0, 0, 0, 0);
	topHBoxLayout->setSpacing(0);
	bottomHBoxLayout->setContentsMargins(0, 0, 0, 0);
	bottomHBoxLayout->setSpacing(0);

	vBoxLayout->addLayout(topHBoxLayout);
	vBoxLayout->addLayout(bottomHBoxLayout);
	this->setLayout(vBoxLayout);

	QLabel *currentTrack = new QLabel(tr("<No track>").toUpper());
	currentTrack->setFont(QFont("Arial", 7));
	bottomHBoxLayout->addWidget(currentTrack);
}

Minimode::~Minimode()
{}

QWidget* Minimode::configPage()
{
	QWidget *widget = new QWidget();
	_ui.setupUi(widget);

	// Init the UI with correct values
	_ui.winampCheckBox->setChecked(Settings::getInstance()->value("hasWinampTheme").toBool());

	// Connect the UI with the settings
	connect(_ui.winampCheckBox, &QCheckBox::stateChanged, [=](int s) {
		Settings::getInstance()->setValue("hasWinampTheme", (s == Qt::Checked));
	});
	return widget;
}

void Minimode::setMediaPlayer(QWeakPointer<MediaPlayer> mediaPlayer)
{
    _mediaPlayer = mediaPlayer;

	QSize sizeButton(10, 10);

	// Media buttons
	QPushButton *previous = new QPushButton(this);
	QPushButton *play = new QPushButton(this);
	QPushButton *pause = new QPushButton(this);
	QPushButton *stop = new QPushButton(this);
	QPushButton *next = new QPushButton(this);

	previous->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
	play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	next->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

	// Control
	QSlider *slider = new QSlider(Qt::Horizontal, this);
	slider->setMaximumWidth(16);
	slider->setMaximumHeight(7);
	slider->setStyleSheet("QSlider::groove:horizontal  {" \
		"border: 1px solid #999999;" \
		"border-top-color: #000010;" \
		"border-right-color: #5A6B7B;" \
		"border-bottom-color: #5A6B7B;" \
		"border-left-color: #000010;" \
		"background: #39395A;" \
		"margin: 0;" \
	"}" \
	"QSlider::handle:horizontal  {" \
		"background: 1px solid #ECCE7A;" \
		"border-radius: 1px;" \
		"margin-top: -1px;" \
		"margin-bottom: -1px;" \
		"width: 3px;" \
	"}");

	// Window management
	QPushButton *minimize = new QPushButton(this);
	QPushButton *restore = new QPushButton(this);
	QPushButton *close = new QPushButton(this);

	minimize->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
	restore->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
	close->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));

	//foreach (QPushButton *b, QList<QPushButton*>({previous, play, pause, stop, next})) {
	QList<QPushButton *> buttons = QList<QPushButton*>() << previous << play << stop << next;
	foreach (QPushButton *b, buttons) {
		b->setFlat(true);
		b->setIconSize(sizeButton);
		layout()->itemAt(0)->layout()->addWidget(b);
		b->installEventFilter(this);
		this->applyColorToStandardIcon(b);
	}
	layout()->itemAt(0)->layout()->addWidget(slider);
	//foreach (QPushButton *b, QList<QPushButton*>({minimize, restore, close})) {
	QList<QPushButton *> buttons2 = QList<QPushButton*>() << minimize << restore << close;
	foreach (QPushButton *b, buttons2) {
		b->setFlat(true);
		b->setIconSize(sizeButton);
		layout()->itemAt(0)->layout()->addWidget(b);
		b->installEventFilter(this);
		this->applyColorToStandardIcon(b);
	}
	this->setStyleSheet("QWidget { background: #39395A; } QPushButton { margin: 0; padding: 0; }");

	connect(previous, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::skipBackward);
	connect(play, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::play);
	connect(pause, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::pause);
	connect(stop, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::stop);
	connect(next, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::skipForward);
	connect(minimize, &QPushButton::clicked, [=]() {
		QMainWindow *mw = qobject_cast<QMainWindow*>(_mediaPlayer.data()->parent());
		if (mw) {
			mw->close();
		}
		this->showMinimized();
	});
	connect(restore, &QPushButton::clicked, [=]() {
		QMainWindow *mw = qobject_cast<QMainWindow*>(_mediaPlayer.data()->parent());
		if (mw) {
			mw->showNormal();
		}
		this->close();
	});
	connect(close, &QPushButton::clicked, &QApplication::quit);
}

void Minimode::toggleViews(QWidget *view)
{
	view->close();
	this->show();
}

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

void Minimode::applyColorToStandardIcon(QPushButton *button)
{
	QPixmap sourcePixmap = QIcon(":/icons/yellow").pixmap(QSize(8, 8));
	QPixmap destinationPixmap = button->icon().pixmap(QSize(8, 8));
	QPixmap resultPixmap = QPixmap(destinationPixmap);

	QPainter painter(&resultPixmap);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultPixmap.rect(), Qt::transparent);

	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, destinationPixmap);

	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, sourcePixmap);

	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.fillRect(resultPixmap.rect(), Qt::transparent);
	painter.end();

	button->setIcon(resultPixmap);
}
