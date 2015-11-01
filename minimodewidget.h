#ifndef MINIMODEWIDGET_H
#define MINIMODEWIDGET_H

#include <QWidget>
#include "ui_mini-mode.h"

class MiniModeWidget : public QWidget
{
	Q_OBJECT
private:
	bool _startMoving;
	QPoint _pos, _globalPos;

public:
	Ui::MiniMode ui;

	explicit MiniModeWidget(QWidget *parent = nullptr);

	void applyColorToStandardIcon(bool hasTheme, QAbstractButton *button);

protected:
	virtual void closeEvent(QCloseEvent *) override;

	/** Redefined to be able to drag this widget on screen. */
	virtual void mouseMoveEvent(QMouseEvent *e) override;

	/** Redefined to be able to drag this widget on screen. */
	virtual void mouseReleaseEvent(QMouseEvent *e) override;

	virtual void mousePressEvent(QMouseEvent *e) override;
};

#endif // MINIMODEWIDGET_H
