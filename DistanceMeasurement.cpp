#include "DistanceMeasurement.h"

DistanceMeasurement::DistanceMeasurement(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags),ui(new Ui::DistanceMeasurement)
{
	ui->setupUi(this);
	this->setWindowTitle("DistanceMeasurement");
	this->setMaximumSize(311, 241);
	this->setMinimumSize(311, 241);
}

DistanceMeasurement::~DistanceMeasurement()
{
	delete ui;
}
