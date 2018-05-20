#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

#include <QtGui/QMainWindow>
#include "ui_VolumeRenderer.h"

#include <QMainWindow>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>

#include <iostream>
#include <memory>
#include <cstdlib>

#ifdef VTK_OPENGL2
#include <vtk_glew.h>
#include <QVTKWidget2.h>
#else
#include <vtkgl.h>
#include <QVTKWidget.h>
#endif

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkMetaImageReader.h>
#include <vtkVolume16Reader.h>
#include <vtkNew.h>
#include <vtkNrrdReader.h>
#include <vtkImageShiftScale.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRendererCollection.h>

#include "ctkTransferFunction.h"
#include "ctkVTKColorTransferFunction.h"
#include "ctkTransferFunctionView.h"
#include "ctkTransferFunctionGradientItem.h"
#include "ctkTransferFunctionControlPointsItem.h"
#include "ctkVTKVolumePropertyWidget.h"

#include "vtkSlicerGPURayCastVolumeMapper.h"

namespace Ui{
	class VolumeRenderer;
}

class VolumeRenderer : public QMainWindow
{
	Q_OBJECT

public:
	VolumeRenderer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~VolumeRenderer();

	void SetInputImage(QString fileName);

private:
	Ui::VolumeRenderer *ui;

	QString filename;
	vtkSmartPointer<vtkRenderWindowInteractor> interactor;
	QVTKWidget widget;
	ctkVTKVolumePropertyWidget volumePropertywidget;
	
private slots:
	void on_actionOpenImage_triggered();
	void on_action_Exit_triggered();
	void on_action_About_triggered();
	void on_action_vtkSlicerGPURayCastVolumeMapper_triggered();
	void on_action_vtkGPUVolumeRayCastMapper_triggered();

	void on_actionScreenshotViewPane_triggered();
};

#endif // VOLUMERENDERER_H
