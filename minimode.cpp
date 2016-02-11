#include "minimode.h"

#include "model/sqldatabase.h"
#include "filehelper.h"
#include "minimodewidget.h"
#include "settings.h"

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
	, _mediaPlayer(nullptr)
{}

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

	/*auto apply = [this](bool colorIcons) {
		for (QPushButton *b : findChildren<QPushButton*>()) {
			_miniModeWidget->applyColorToStandardIcon(colorIcons, b);
		}
	};
	apply(hasWinampTheme);*/

	// Connect the UI with the settings
	connect(_config.winampCheckBox, &QCheckBox::toggled, [=](bool b) {
		Settings::instance()->setValue("MiniMode/hasWinampTheme", b);
		//apply(b);
	});
	return widget;
}

AbstractView *Minimode::instanciateView()
{
	return new MiniModeWidget(_mediaPlayer);
}

void Minimode::setMediaPlayer(MediaPlayer *mediaPlayer)
{
	_mediaPlayer = mediaPlayer;

}
