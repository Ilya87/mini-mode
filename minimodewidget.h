#ifndef MINIMODEWIDGET_H
#define MINIMODEWIDGET_H

#include "abstractview.h"
#include "ui_mini-mode.h"

class MiniModeWidget : public AbstractView
{
	Q_OBJECT
private:
	bool _startMoving;
	QPoint _pos, _globalPos;

public:
	Ui::MiniMode ui;

	explicit MiniModeWidget(MediaPlayer *mediaPlayer, QWidget *parent = nullptr);

	void applyColorToStandardIcon(bool hasTheme, QAbstractButton *button);

	virtual bool eventFilter(QObject *obj, QEvent *e) override;

	virtual bool viewProperty(Settings::ViewProperty vp) const override;

protected:
	virtual void closeEvent(QCloseEvent *) override;

	/** Redefined to be able to drag this widget on screen. */
	virtual void mouseMoveEvent(QMouseEvent *e) override;

	/** Redefined to be able to drag this widget on screen. */
	virtual void mouseReleaseEvent(QMouseEvent *e) override;

	virtual void mousePressEvent(QMouseEvent *e) override;

public slots:
	virtual void setViewProperty(Settings::ViewProperty vp, QVariant value) override;

};

#endif // MINIMODEWIDGET_H
