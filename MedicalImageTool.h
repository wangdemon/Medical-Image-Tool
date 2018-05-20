#ifndef MEDICALIMAGETOOL_H
#define MEDICALIMAGETOOL_H

#include <QtGui/QMainWindow>
#include "ui_MedicalImageTool.h"

// Itk class
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"

// Vtk class
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkResliceImageViewer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkDistanceWidget.h"
#include "vtkResliceImageViewerMeasurements.h"
#include "VolumeRenderer.h"

// Qt class
#include "Crosshair.h"
#include "DistanceMeasurement.h"
#include "About.h"
#include "ImageToSTL.h"

#include "QVTKWidget.h"

namespace Ui
{
	class MedicalImageTool;
}

class MedicalImageTool : public QMainWindow
{
	Q_OBJECT

public:
	MedicalImageTool(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MedicalImageTool();

	typedef itk::Image<float, 3> ItkImageType;
	typedef itk::ImageFileReader<ItkImageType> ItkReaderType;
	typedef itk::ImageToVTKImageFilter<ItkImageType> ItkToVtkType;

protected slots:

	// Edit function
	virtual void resliceMode(int);
	virtual void thickMode(int);
	virtual void ResetViews();

	virtual void AddDistanceMeasurementToAxial();
	virtual void AddDistanceMeasurementToSagital();
	virtual void AddDistanceMeasurementToCoronal();
	virtual void AddDistanceMeasurementTo3DView();
	virtual void AddDistanceMeasurementToView(int);

private slots:
	// File function
	void on_actionExit_triggered();
	void on_actionOpenImage_triggered();

	void on_actionSaveAs_triggered();
	void on_actionOpenSTL_triggered();

	// Edit function
	void on_actionCrosshair_triggered();
	void on_actionDistanceMeasure_triggered();

	void on_actionScreenshotUpperLeft_triggered();
	void on_actionScreenshotUpperRight_triggered();
	void on_actionScreenshotLowerLeft_triggered();
	void on_actionScreenshotLowerRight_triggered();

	// Help function
	void on_actionAboutProgram_triggered();

	// GPU Renderer
	void on_actionGPUVolumeRenderer_triggered();

private:
	Ui::MedicalImageTool *ui;

	// Itk variable
	ItkImageType::Pointer inputItkImage;
	ItkReaderType::Pointer itkReader;
	ItkToVtkType::Pointer itkToVtkConnector;

	// Vtk variable
	vtkSmartPointer<vtkImageData> inputVtkImage;

	// Qt variable
	QString m_InputImageAllName;
	QString m_InputImageName;
	QString m_InputImageSuffixName;
	QString m_InputPath;

	vtkSmartPointer< vtkResliceImageViewer > riw[3];
	vtkSmartPointer< vtkImagePlaneWidget > planeWidget[3];
	vtkSmartPointer< vtkDistanceWidget > DistanceWidget[3];
	vtkSmartPointer< vtkResliceImageViewerMeasurements > ResliceMeasurements;

	void LoadImage(QString fileName);

	Crosshair *crosshairEdit;
	DistanceMeasurement *distMeasEdit;
	About *aboutHelp;
	VolumeRenderer * srcImageRenderer;
	ImageToSTL *stlFileFilter;

	// Screenshot
	void ViewPaneScreenshot(QVTKWidget *widget);
};

#endif // MEDICALIMAGETOOL_H
