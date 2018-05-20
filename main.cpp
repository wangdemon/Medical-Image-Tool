#include "MedicalImageTool.h"
#include <QtGui/QApplication>

#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QEventLoop>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QPixmap pixmap("splashscreen.PNG");
	QSplashScreen splash(pixmap);
	splash.show();
// 	QEventLoop eventloop;
// 	QTimer::singleShot(100, &eventloop, SLOT(quit()));
// 	eventloop.exec();

	MedicalImageTool w;
	w.show();
	return a.exec();
}
