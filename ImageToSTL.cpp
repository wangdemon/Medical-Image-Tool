#include "ImageToSTL.h"

// Itk class
#include "itkStatisticsImageFilter.h"

// Vtk header file
#include "vtkImageFlip.h"
#include "vtkImageCast.h"
#include "vtkImageMapper3D.h"

// Vtk connect qt
#include <vtkEventQtSlotConnect.h>

// Vtk actor property
#include "vtkImageProperty.h"

// Vtk tracer
#include "vtkImageTracerWidget.h"
// Vtk tracer property
#include "vtkProperty.h"

// Vtk call back
#include "vtkCallbackCommand.h"

#include <vtkPolyData.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkMarchingCubes.h>
#include <vtkPLYWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkTriangleFilter.h>
#include <vtkSTLWriter.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>


// Qt header file
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

void CallbackFunction(vtkObject* caller,
	long unsigned int vtkNotUsed(eventId),
	void* vtkNotUsed(clientData),
	void* vtkNotUsed(callData));

ImageToSTL::ImageToSTL(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), ui(new Ui::ImageToSTL)
{
	ui->setupUi(this);
	this->setWindowTitle("Image To STL");


	ui->groupBox->setMaximumSize(201, 221);
	ui->groupBox->setMinimumSize(201, 221);

	this->ui->labelXCoordinate->setAlignment(Qt::AlignCenter);
	this->ui->labelYCoordinate->setAlignment(Qt::AlignCenter);
	this->ui->labelZCoordinate->setAlignment(Qt::AlignCenter);

	this->ui->labelImageName->setAlignment(Qt::AlignCenter);
	this->ui->labelImageIntensity->setAlignment(Qt::AlignCenter);

	ui->labelSourceImageSliceNum->setMaximumSize(30, 20);
	ui->labelSourceImageSliceNum->setMinimumSize(30, 20);
	ui->labelSourceImageSliceNum->setAlignment(Qt::AlignCenter);

	ui->divideLineLabel1->setMaximumSize(30, 20);
	ui->divideLineLabel1->setMinimumSize(30, 20);
	ui->divideLineLabel1->setAlignment(Qt::AlignCenter);

	ui->labelSourceImageSliceTotal->setMaximumSize(30, 20);
	ui->labelSourceImageSliceTotal->setMinimumSize(30, 20);
	ui->labelSourceImageSliceTotal->setAlignment(Qt::AlignCenter);

	//initialize the inputSourceImageReader and inputSourceImage variable 
	itkImageReader      = ITKReadType::New();
	itkInputSourceImage = ITKImageType::New();
	itkToVtkConnector   = ITKtoVTKConnectorType::New();

	// initialize vtk variable
	imageData				= vtkSmartPointer<vtkImageData>::New();
	vtkWindowLevel			= vtkSmartPointer<vtkImageMapToWindowLevelColors>::New();
	imageActor				= vtkSmartPointer<vtkImageActor>::New();
	vtkRender				= vtkSmartPointer<vtkRenderer>::New();
	renderWindow            = vtkSmartPointer<vtkRenderWindow>::New();
	renderWinInteractor     = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactorStyleImage    = vtkSmartPointer<vtkInteractorStyleImage>::New();
	interactorStyleGetPixel = vtkSmartPointer<vtkInteractorStyleImageGetPixel>::New();
	
	this->currentInteractorStyle = interactorStyleGetPixel;

	// make vtk render window to display qvtkWidget
	ui->qvtkWidgetDisplaySourceImage->SetRenderWindow(renderWindow);

	// Vtk connect Qt slots
	vtkConnectQt = vtkEventQtSlotConnect::New();
	vtkConnectQt->Connect(renderWinInteractor, vtkCommand::MouseMoveEvent, this, SLOT(updateCoordinate(vtkObject*)));

	stlParameterSet = new STLSet;
	polyData = vtkSmartPointer<vtkPolyData>::New();

	this->initialWindow = 1000;
	this->initialLevel = 0;
}

ImageToSTL::~ImageToSTL()
{
	delete ui;
	delete stlParameterSet;
}

// Prompt message whether to input source image
void ImageToSTL::inputSourceImagePromptMessage()
{
	if (inputSourceImageAllName.isEmpty())
	{
		QMessageBox::information(this, "Prompt Message", "Please input source Image !");
		return;
	}
}

// Adjust input source image's window and level
void ImageToSTL::setSourceImageLevel()
{
	inputSourceImagePromptMessage();
	this->inputSourceImageLevel = ui->lineEditSourceImageLevel->text().toFloat();
	this->vtkWindowLevel->SetLevel(this->inputSourceImageLevel);
	vtkRender->ResetCamera();
	renderWindow->Render();
}

void ImageToSTL::setSourceImageWindow()
{
	inputSourceImagePromptMessage();
	this->inputSourceImageWindow = ui->lineEditSourceImageWindow->text().toFloat();
	this->vtkWindowLevel->SetWindow(this->inputSourceImageWindow);
	vtkRender->ResetCamera();
	renderWindow->Render();
}

void ImageToSTL::setSourceImageSliceSlot(int slice)
{
	inputSourceImagePromptMessage();

	// Change input image slice to show
	ui->sliceSourceImagePosition->setRange(0, itkInputSourceImageSize[2] - 1);
	ui->labelSourceImageSliceNum->setText(QString::number(slice));

	//	imageActor->SetDisplayExtent(0, itkInputSourceImageSize[0] - 1, 0, itkInputSourceImageSize[1] - 1, slice, slice);

	// If you use the Scribble and GetPixel interactor style, you must be update the style input information
	interactorStyleGetPixel->SetInputInfo(imageData, imageActor, slice);

	// if using the ResetCameraClippingRange function, the image display size will be as the number changes in size
	//	vtkRender->ResetCameraClippingRange();
	vtkRender->ResetCamera();
	renderWindow->Render();
}

void ImageToSTL::openSourceImageSlot()
{
	inputSourceImageAllName = QFileDialog::getOpenFileName(this, "Open Image", 
		"../Image", tr("Image Files(*.png *.jpg *.bmp *.jpeg *.dcm *.mhd *.mha *.nii *.hdr)"));
	inputSourceImagePromptMessage();

	// Get the input image name
	QFileInfo inputImageNameInfo = QFileInfo(inputSourceImageAllName);
	//	inputSourceImageName = inputImageNameInfo.fileName();
	inputSourceImageName = inputImageNameInfo.baseName();
	ui->labelImageName->setText(inputSourceImageName);

	// Read the source image
	itkImageReader->SetFileName(inputSourceImageAllName.toStdString());
	itkImageReader->Update();
	itkInputSourceImage = itkImageReader->GetOutput();
	itkInputSourceImageSize = itkInputSourceImage->GetLargestPossibleRegion().GetSize();

	ui->labelSourceImageSliceTotal->setText(QString::number(itkInputSourceImageSize[2] - 1));
	// initialize the source image's first slice 
	ui->labelSourceImageSliceNum->setText(QString::number(0));

	// itk image data to vtk image data
	itkToVtkConnector->SetInput(itkInputSourceImage);
	itkToVtkConnector->Update();

	// flip image
	vtkSmartPointer<vtkImageFlip> vtkFlipImageFilter = vtkSmartPointer<vtkImageFlip>::New();
	vtkFlipImageFilter->SetInputData(itkToVtkConnector->GetOutput());
	vtkFlipImageFilter->SetFilteredAxes(1);
	vtkFlipImageFilter->Update();

	// cast float type image to int, that make display image better
	vtkSmartPointer<vtkImageCast> vtkImageCastFilter = vtkSmartPointer<vtkImageCast>::New();
	vtkImageCastFilter->SetInputData(vtkFlipImageFilter->GetOutput());
	vtkImageCastFilter->SetOutputScalarTypeToInt();
	vtkImageCastFilter->Update();

	this->imageData = vtkImageCastFilter->GetOutput();

	vtkWindowLevel->SetInputConnection(vtkImageCastFilter->GetOutputPort());
	// The Window and Level is empirical value
	this->calculateSuitableWinLev();

	vtkWindowLevel->SetWindow(this->initialWindow);
	vtkWindowLevel->SetLevel(this->initialLevel);

	// Display image window and level
	ui->lineEditSourceImageLevel->setValue(this->vtkWindowLevel->GetLevel());
	ui->lineEditSourceImageWindow->setValue(this->vtkWindowLevel->GetWindow());

	imageActor->GetMapper()->SetInputConnection(vtkWindowLevel->GetOutputPort());
	vtkRender->AddActor(imageActor);
	renderWindow->AddRenderer(vtkRender);
	renderWindow->Render();

	renderWinInteractor->SetInteractorStyle(interactorStyleGetPixel);
	renderWinInteractor->SetRenderWindow(renderWindow);
	renderWinInteractor->Initialize();

	interactorStyleGetPixel->SetInputInfo(imageData, imageActor, 0);
	// 	interactorStyleScribble->SetCurrentRenderer(vtkRender);
	// 	interactorStyleGetPixel->AddObserver(this->interactorStyleGetPixel->ScribbleEvent, this, &GraphCutsUI::ScribbleEventHandler);	
	// 	interactorStyleScribble->SetColorToRed();
	// 	// this must come after the SetInteractorStyle call
	// 	interactorStyleScribble->InitializeTracer();

// 	vtkSmartPointer<vtkImageTracerWidget> tracer = vtkSmartPointer<vtkImageTracerWidget>::New();
// 	tracer->GetLineProperty()->SetLineWidth(5);
// 	tracer->GetLineProperty()->SetOpacity(1);
// 	tracer->SetInteractor(renderWinInteractor);
// 	tracer->SetViewProp(imageActor);
// 	tracer->ProjectToPlaneOn();

	// The observer must be added BEFORE the On() call.
// 	vtkSmartPointer<vtkCallbackCommand> callback =
// 		vtkSmartPointer<vtkCallbackCommand>::New();
// 	callback->SetCallback(CallbackFunction);
// 	tracer->AddObserver(vtkCommand::EndInteractionEvent, callback);
// 	tracer->On();

	// Set image actor opacity
	//	imageActor->GetProperty()->SetOpacity(0);

	vtkRender->ResetCamera();
	renderWinInteractor->Start();
}

// Display current pixel's coordinate and value
void ImageToSTL::displayPixelCoordinateAndValue()
{
	// Display coordinate
	ui->labelXCoordinate->setText(QString::number(this->interactorStyleGetPixel->GetCurrentPixelCoordinate()[0]));
	ui->labelYCoordinate->setText(QString::number(this->interactorStyleGetPixel->GetCurrentPixelCoordinate()[1]));
	ui->labelZCoordinate->setText(QString::number(this->interactorStyleGetPixel->GetCurrentPixelCoordinate()[2]));

	// Display pixel value
	ui->labelImageIntensity->setText(QString::number(this->interactorStyleGetPixel->GetCurrentPixelValue()));
}


// Update current coordinate
void ImageToSTL::updateCoordinate(vtkObject* obj)
{
	this->displayPixelCoordinateAndValue();
}

void CallbackFunction(vtkObject* caller,
	long unsigned int vtkNotUsed(eventId),
	void* vtkNotUsed(clientData),
	void* vtkNotUsed(callData))
{
// 	vtkImageTracerWidget* tracerWidget =
// 		static_cast<vtkImageTracerWidget*>(caller);
// 	vtkSmartPointer<vtkPolyData> path =
// 		vtkSmartPointer<vtkPolyData>::New();
// 	tracerWidget->GetPath(path);
// 	std::cout << "There are " << path->GetNumberOfPoints() << " points in the path." << std::endl;
}

// Screenshot
void ImageToSTL::ViewPaneScreenshot(QVTKWidget *widget)
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save Screenshot", "", "Pircture Files (*.png *.jpg)");
	if (fileName.isEmpty())
	{
		return;
	}

	vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
		vtkSmartPointer<vtkWindowToImageFilter>::New();
	windowToImageFilter->SetInput(widget->GetRenderWindow());
	//windowToImageFilter->SetMagnification(3);
	windowToImageFilter->Update();

	vtkSmartPointer<vtkPNGWriter> writer =
		vtkSmartPointer<vtkPNGWriter>::New();
	writer->SetFileName(fileName.toStdString().c_str());
	writer->SetInputConnection(windowToImageFilter->GetOutputPort());
	writer->Write();
}

void ImageToSTL::on_actionScreenshotLeft_triggered()
{
	ViewPaneScreenshot(this->ui->qvtkWidgetDisplaySourceImage);
}

void ImageToSTL::on_actionScreenshotRight_triggered()
{
	ViewPaneScreenshot(this->ui->qvtkWidgetPolyData);
}

// View
void ImageToSTL::on_actionDisplayPly_triggered()
{
	vtkImageDataToPly();

	//vtkSmartPointer<vtkImageViewer2> imgViewer = vtkSmartPointer<vtkImageViewer2>::New();

	vtkSmartPointer<vtkPolyDataMapper> cylinderMapper = 
		vtkSmartPointer<vtkPolyDataMapper>::New();
	cylinderMapper->SetInputData( polyData ); 

	vtkSmartPointer<vtkActor> cylinderActor = 
		vtkSmartPointer<vtkActor>::New();
	cylinderActor->SetMapper( cylinderMapper );
	cylinderActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

	vtkSmartPointer<vtkRenderer> renderer = 
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor( cylinderActor );
	renderer->SetBackground( 1.0, 1.0, 1.0 );

	vtkSmartPointer<vtkRenderWindow> renWin = 
		vtkSmartPointer<vtkRenderWindow>::New();
	renWin->AddRenderer( renderer );
	renWin->SetSize( 640, 480 );
	renWin->Render();
	renWin->SetWindowName("RenderCylinder");

	this->ui->qvtkWidgetPolyData->SetRenderWindow(renWin);

	vtkSmartPointer<vtkRenderWindowInteractor> iren = 
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = 
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	iren->SetInteractorStyle(style);

	//display axis
	vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
	vtkSmartPointer<vtkOrientationMarkerWidget> axesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	axesWidget->SetOrientationMarker(axesActor);
	axesWidget->SetInteractor(iren);
	axesWidget->On();
	axesWidget->SetInteractive(0);

	iren->Initialize();
	iren->Start();

}

void ImageToSTL::vtkImageDataToPly()
{
	if (!this->stlParameterSet->ui->lineEditIosValue->text().isEmpty())
	{
		isoValue = this->stlParameterSet->ui->lineEditIosValue->text().toFloat();
	}
	this->imageData->DeepCopy(itkToVtkConnector->GetOutput());
	
	vtkSmartPointer<vtkMarchingCubes> surface = 
		vtkSmartPointer<vtkMarchingCubes>::New();
	surface->SetInputData(imageData);

	surface->ComputeNormalsOn();
	surface->SetValue(0, isoValue);
	surface->Update();
	polyData = surface->GetOutput();
}

void ImageToSTL::on_actionSavePly_triggered()
{
	vtkImageDataToPly();
	QString fileName = QFileDialog::getSaveFileName(this, "Save Ply", "", "Ply Files(*.ply)");
	if (fileName.isEmpty())
	{
		QMessageBox::information(this, "Prompt Message", "Please input the name of save ply file!");
		return;
	}
	vtkSmartPointer<vtkPLYWriter> plyWriter = vtkSmartPointer<vtkPLYWriter>::New();
	plyWriter->SetFileName(fileName.toStdString().c_str());
	plyWriter->SetFileTypeToASCII();
	plyWriter->SetInputData(polyData);
	plyWriter->Write();
}

void ImageToSTL::on_actionSaveSTL_triggered()
{
	vtkImageDataToPly();
	QString fileName = QFileDialog::getSaveFileName(this, "Save STL", "", "Ply Files(*.stl)");
	if (fileName.isEmpty())
	{
		QMessageBox::information(this, "Prompt Message", "Please input the name of save ply file!");
		return;
	}

	vtkSmartPointer<vtkTriangleFilter> stlFilter = vtkSmartPointer<vtkTriangleFilter>::New();
	stlFilter->SetInputData(polyData);
	vtkSmartPointer<vtkSTLWriter> stlWriter =
		vtkSmartPointer<vtkSTLWriter>::New();
	stlWriter->SetFileName(fileName.toStdString().c_str());
	stlWriter->SetInputConnection(stlFilter->GetOutputPort());
	stlWriter->Write();
}

// Parameter set
void ImageToSTL::on_actionSTLSet_triggered()
{
	this->stlParameterSet->show();
}

void ImageToSTL::calculateSuitableWinLev()
{
	typedef itk::StatisticsImageFilter< ITKImageType > StatisticsFilterType;
	StatisticsFilterType::Pointer statisticsFilter = StatisticsFilterType::New();
	statisticsFilter->SetInput(this->itkInputSourceImage);
	statisticsFilter->Update();

	this->initialWindow = statisticsFilter->GetMaximum() - statisticsFilter->GetMinimum();
	this->initialLevel = this->initialWindow / 2;
}