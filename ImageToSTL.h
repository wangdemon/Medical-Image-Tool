#ifndef IMAGETOSTL_H
#define IMAGETOSTL_H

#include <QtGui/QMainWindow>
#include "ui_ImageToSTL.h"

// itk class header file
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"

// vtk header file
#include "vtkSmartPointer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageActor.h"
#include "vtkImageMapToWindowLevelColors.h"

// vtk class get pixel
#include "vtkInteractorStyleImageGetPixel.h"

#include <vtkPolyData.h>

// Qt class
#include <QVTKWidget.h>

#include "STLSet.h"

// Update coordinate
class vtkEventQtSlotConnect;

namespace Ui{
	class ImageToSTL;
}

class ImageToSTL : public QMainWindow
{
	Q_OBJECT

public:
	ImageToSTL(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ImageToSTL();

	//itk Standard class typedefs
	typedef itk::Image<float, 3> 				        ITKImageType;
	typedef itk::ImageFileReader<ITKImageType>			ITKReadType;
	typedef itk::ImageToVTKImageFilter<ITKImageType>	ITKtoVTKConnectorType;

protected:
//	void ScribbleEventHandler(vtkObject* caller, long unsigned int eventId, void* callData);

private:
	Ui::ImageToSTL *ui;

	//define reader source image use itk class
	ITKReadType::Pointer itkImageReader;
	ITKImageType::Pointer itkInputSourceImage;
	ITKtoVTKConnectorType::Pointer itkToVtkConnector;
	ITKImageType::RegionType::SizeType itkInputSourceImageSize;

	// define vtk variable use vtk class
	vtkSmartPointer<vtkImageData> imageData;
	vtkSmartPointer<vtkImageMapToWindowLevelColors> vtkWindowLevel;
	vtkSmartPointer<vtkImageActor> imageActor;
	vtkSmartPointer<vtkRenderer> vtkRender;
	vtkSmartPointer<vtkRenderWindow> renderWindow;
	vtkSmartPointer<vtkRenderWindowInteractor>renderWinInteractor;

	// Three different interactive style
	vtkSmartPointer<vtkInteractorStyleImage> interactorStyleImage;
	vtkSmartPointer<vtkInteractorStyleImageGetPixel> interactorStyleGetPixel;
	
	// Define the current interactive style
	vtkInteractorStyleImage *currentInteractorStyle;

	//define file name in Qt
	QString inputSourceImageAllName;
	QString inputSourceImageName;

	// Input source image's window and level
	float inputSourceImageLevel;
	float inputSourceImageWindow;

	// Source image prompt message
	void inputSourceImagePromptMessage();

	// Display current pixel's coordinate and value
	void displayPixelCoordinateAndValue();

	// Vtk connect qt
	vtkEventQtSlotConnect *vtkConnectQt;

	void vtkImageDataToPly();
	vtkSmartPointer<vtkPolyData> polyData;
	double isoValue;

	STLSet *stlParameterSet;

	// Initial window and level
	float initialWindow;
	float initialLevel;

	void calculateSuitableWinLev();

private slots:
	// Open source image slot
	void openSourceImageSlot();

	// Save ply
	void on_actionSavePly_triggered();
	void on_actionSaveSTL_triggered();

	// Set source image slice slot
	void setSourceImageSliceSlot(int slice);

	// Adjust input source image's window and level
	void setSourceImageLevel();
	void setSourceImageWindow();

	// Update current coordinate
	void updateCoordinate(vtkObject* obj);

	// Screenshot
	void ViewPaneScreenshot(QVTKWidget *widget);
	void on_actionScreenshotLeft_triggered();
	void on_actionScreenshotRight_triggered();

	// View
	void on_actionDisplayPly_triggered();

	// Patameter set
	void on_actionSTLSet_triggered();
	
};

#endif // IMAGETOSTL_H
