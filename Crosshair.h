#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include <QtGui/QWidget>
#include "ui_Crosshair.h"

namespace Ui
{
	class Crosshair;
}

class Crosshair : public QWidget
{
	Q_OBJECT

public:
	Crosshair(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Crosshair();

	Ui::Crosshair *ui;

private:
//	Ui::Crosshair *ui;

private slots:
	void on_pushButtonClose_clicked();
};

#endif // CROSSHAIR_H
