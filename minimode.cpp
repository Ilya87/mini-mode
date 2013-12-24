#include "minimode.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>

#include <QtDebug>

Minimode::Minimode()
	: QWidget()
{
	setWindowFlags(Qt::FramelessWindowHint);
}

QWidget *Minimode::configPage()
{
    return NULL;
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

	// Window management
	QPushButton *minimize = new QPushButton(this);
	QPushButton *restore = new QPushButton(this);
	QPushButton *close = new QPushButton(this);

	QLayout *hBoxLayout = new QHBoxLayout(this);
	hBoxLayout->setContentsMargins(0, 0, 0, 0);
	hBoxLayout->setSpacing(1);
	this->setLayout(hBoxLayout);
	foreach (QPushButton *b, this->findChildren<QPushButton*>()) {
		b->setFlat(true);
		b->setIconSize(sizeButton);
		layout()->addWidget(b);
	}

	previous->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
	play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	next->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

	minimize->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
	restore->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
	close->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));

	connect(previous, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::skipBackward);
	connect(play, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::play);
	connect(pause, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::pause);
	connect(stop, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::stop);
	connect(next, &QPushButton::clicked, _mediaPlayer.data(), &MediaPlayer::skipForward);
	connect(minimize, &QPushButton::clicked, [=]() {
		QMainWindow *mw = qApp->findChild<QMainWindow*>();
		qDebug() << (mw == NULL);
		mw->showMinimized();
		this->close();
	});
	connect(restore, &QPushButton::clicked, [=]() {
		QMainWindow *mw = qApp->findChild<QMainWindow*>();
		qDebug() << (mw == NULL);
		mw->showNormal();
		this->showMinimized();
	});
	connect(close, &QPushButton::clicked, qApp, &QCoreApplication::quit);
}
