#include "About.h"

About::About(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags), ui(new Ui::About)
{
	ui->setupUi(this);
	this->setWindowTitle("About Medical Image Tool");
	this->setMaximumSize(529, 391);
	this->setMinimumSize(529, 391);
}

About::~About()
{
	delete ui;
}

void About::on_pushButton_clicked()
{
	this->close();
}