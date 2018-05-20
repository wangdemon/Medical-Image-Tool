#include "MedicalImageTool.h"

// Itk class
#include "itkImageFileWriter.h"

// Vtk class
#include "vtkResliceCursorCallback.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "vtkImageFlip.h"

#include "vtkSmartPointer.h"
#include "vtkCellPicker.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceImageViewer.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"

#include "vtkMetaImageReader.h"
#include "vtkDICOMImageReader.h"
#include "vtkVolume16Reader.h"

#include "vtkProperty.h"
#include "vtkPlane.h"
#include "vtkImageData.h"
#include "vtkCommand.h"
#include "vtkPlaneSource.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkInteractorStyleImage.h"
#include "vtkImageSlabReslice.h"
#include "vtkBoundedPlanePointPlacer.h"
#include "vtkDistanceRepresentation.h"
#include "vtkHandleRepresentation.h"

#include "vtkDistanceRepresentation2D.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkPointHandleRepresentation2D.h"

#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkSTLWriter.h>
#include <vtkTriangleFilter.h>

// Qt class
#include <QDir>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

MedicalImageTool::MedicalImageTool(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), ui(new Ui::MedicalImageTool)
{
	ui->setupUi(this);
	this->setWindowTitle("MedicalImageTool");

	// Initialize variable
	inputItkImage = ItkImageType::New();
	itkReader = ItkReaderType::New();
	itkToVtkConnector = ItkToVtkType::New();
	inputVtkImage = vtkSmartPointer<vtkImageData>::New();

	crosshairEdit = new Crosshair;
	aboutHelp = new About;
	distMeasEdit = new DistanceMeasurement;

	srcImageRenderer = new VolumeRenderer;
	stlFileFilter = new ImageToSTL;

	connect(this->crosshairEdit->ui->resliceModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(resliceMode(int)));
	connect(this->crosshairEdit->ui->thickModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(thickMode(int)));
	this->crosshairEdit->ui->thickModeCheckBox->setEnabled(0);
	connect(this->crosshairEdit->ui->resetButton, SIGNAL(pressed()), this, SLOT(ResetViews()));
	connect(this->distMeasEdit->ui->pushButtonDistMeasAxial, SIGNAL(pressed()), this, SLOT(AddDistanceMeasurementToAxial()));
	connect(this->distMeasEdit->ui->pushButtonDistMeasSagital, SIGNAL(pressed()), this, SLOT(AddDistanceMeasurementToSagital()));
	connect(this->distMeasEdit->ui->pushButtonDistMeasCoronal, SIGNAL(pressed()), this, SLOT(AddDistanceMeasurementToCoronal()));
	connect(this->distMeasEdit->ui->pushButtonDistMeas3DView, SIGNAL(pressed()), this, SLOT(AddDistanceMeasurementTo3DView()));

}

MedicalImageTool::~MedicalImageTool()
{
	delete ui;
	delete crosshairEdit;
	delete aboutHelp;
	delete srcImageRenderer;
	delete stlFileFilter;
}

// Close the program
void MedicalImageTool::on_actionExit_triggered()
{
	this->close();
}

// Open image
void MedicalImageTool::on_actionOpenImage_triggered()
{
	// Get the input image name
	m_InputImageAllName = QFileDialog::getOpenFileName(this,
		"Open Image", "../Image",
		tr("Image Files(*.mhd *.mha *.dcm *.hdr *.img *nii *jpg *jpeg *png *bmp)"));

	if (m_InputImageAllName.isEmpty())
	{
		return;
	}

	QFileInfo fileInfo = QFileInfo(this->m_InputImageAllName);
	this->m_InputImageName = fileInfo.fileName();
	this->m_InputImageSuffixName = fileInfo.suffix();
	this->m_InputPath = fileInfo.absolutePath();

	itkReader->SetFileName(this->m_InputImageAllName.toStdString());
	itkReader->Update();
	inputItkImage = itkReader->GetOutput();
	itkToVtkConnector->SetInput(inputItkImage);
	itkToVtkConnector->Update();

	vtkSmartPointer<vtkImageFlip> vtkFlipFiter =
		vtkSmartPointer<vtkImageFlip>::New();
	vtkFlipFiter->SetInputData(itkToVtkConnector->GetOutput());
	vtkFlipFiter->SetFilteredAxes(1);
	vtkFlipFiter->Update();
	inputVtkImage = vtkFlipFiter->GetOutput();

	LoadImage(m_InputImageAllName);
}

void MedicalImageTool::LoadImage(QString fileName)
{
	vtkSmartPointer<vtkMetaImageReader> reader
		= vtkSmartPointer<vtkMetaImageReader>::New();
	reader->SetFileName(fileName.toStdString().data());
	reader->Update();

	int imageDims[3];
	reader->GetOutput()->GetDimensions(imageDims);

	for (int i = 0; i < 3; i++)
	{
		riw[i] = vtkSmartPointer< vtkResliceImageViewer >::New();
	}

	this->ui->qvtkWidgetSagitalPlane->SetRenderWindow(riw[0]->GetRenderWindow());
	riw[0]->SetupInteractor(
		this->ui->qvtkWidgetSagitalPlane->GetRenderWindow()->GetInteractor());

	this->ui->qvtkWidgetCoronalPlane->SetRenderWindow(riw[1]->GetRenderWindow());
	riw[1]->SetupInteractor(
		this->ui->qvtkWidgetCoronalPlane->GetRenderWindow()->GetInteractor());

	this->ui->qvtkWidgetAxialPlane->SetRenderWindow(riw[2]->GetRenderWindow());
	riw[2]->SetupInteractor(
		this->ui->qvtkWidgetAxialPlane->GetRenderWindow()->GetInteractor());

	for (int i = 0; i < 3; i++)
	{
		// make them all share the same reslice cursor object.
		vtkResliceCursorLineRepresentation *rep =
			vtkResliceCursorLineRepresentation::SafeDownCast(
			riw[i]->GetResliceCursorWidget()->GetRepresentation());
		riw[i]->SetResliceCursor(riw[0]->GetResliceCursor());

		rep->GetResliceCursorActor()->
			GetCursorAlgorithm()->SetReslicePlaneNormal(i);

		riw[i]->SetInputData(reader->GetOutput());
		riw[i]->SetSliceOrientation(i);
		riw[i]->SetResliceModeToAxisAligned();
	}

	vtkSmartPointer<vtkCellPicker> picker =
		vtkSmartPointer<vtkCellPicker>::New();
	picker->SetTolerance(0.005);

	vtkSmartPointer<vtkProperty> ipwProp =
		vtkSmartPointer<vtkProperty>::New();

	vtkSmartPointer< vtkRenderer > ren =
		vtkSmartPointer< vtkRenderer >::New();

	this->ui->qvtkWidget3DView->GetRenderWindow()->AddRenderer(ren);
	vtkRenderWindowInteractor *iren = this->ui->qvtkWidget3DView->GetInteractor();

	for (int i = 0; i < 3; i++)
	{
		planeWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
		planeWidget[i]->SetInteractor(iren);
		planeWidget[i]->SetPicker(picker);
		planeWidget[i]->RestrictPlaneToVolumeOn();
		double color[3] = { 0, 0, 0 };
		color[i] = 1;
		planeWidget[i]->GetPlaneProperty()->SetColor(color);

		color[0] /= 4.0;
		color[1] /= 4.0;
		color[2] /= 4.0;
		riw[i]->GetRenderer()->SetBackground(color);

		planeWidget[i]->SetTexturePlaneProperty(ipwProp);
		planeWidget[i]->TextureInterpolateOff();
		planeWidget[i]->SetResliceInterpolateToLinear();
		planeWidget[i]->SetInputConnection(reader->GetOutputPort());
		planeWidget[i]->SetPlaneOrientation(i);
		planeWidget[i]->SetSliceIndex(imageDims[i] / 2);
		planeWidget[i]->DisplayTextOn();
		planeWidget[i]->SetDefaultRenderer(ren);
		planeWidget[i]->SetWindowLevel(1358, -27);
		planeWidget[i]->On();
		planeWidget[i]->InteractionOn();
	}

	vtkSmartPointer<vtkResliceCursorCallback> cbk =
		vtkSmartPointer<vtkResliceCursorCallback>::New();

	for (int i = 0; i < 3; i++)
	{
		cbk->IPW[i] = planeWidget[i];
		cbk->RCW[i] = riw[i]->GetResliceCursorWidget();
		riw[i]->GetResliceCursorWidget()->AddObserver(
			vtkResliceCursorWidget::ResliceAxesChangedEvent, cbk);
		riw[i]->GetResliceCursorWidget()->AddObserver(
			vtkResliceCursorWidget::WindowLevelEvent, cbk);
		riw[i]->GetResliceCursorWidget()->AddObserver(
			vtkResliceCursorWidget::ResliceThicknessChangedEvent, cbk);
		riw[i]->GetResliceCursorWidget()->AddObserver(
			vtkResliceCursorWidget::ResetCursorEvent, cbk);
		riw[i]->GetInteractorStyle()->AddObserver(
			vtkCommand::WindowLevelEvent, cbk);

		// Make them all share the same color map.
		riw[i]->SetLookupTable(riw[0]->GetLookupTable());
		planeWidget[i]->GetColorMap()->SetLookupTable(riw[0]->GetLookupTable());
		//planeWidget[i]->GetColorMap()->SetInput(riw[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap()->GetInput());
		planeWidget[i]->SetColorMap(riw[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap());

	}

	this->ui->qvtkWidgetSagitalPlane->show();
	this->ui->qvtkWidgetCoronalPlane->show();
	this->ui->qvtkWidgetAxialPlane->show();
}

void MedicalImageTool::on_actionSaveAs_triggered()
{
	if (m_InputImageAllName.isEmpty())
	{
		QMessageBox::information(this, "Prompt Message", "Please input a image !");
		return;
	}
	QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "Image Files (*.mhd *.mha *.dcm *.img *.hdr *.nii)");
	if (fileName.isEmpty())
	{
		QMessageBox::information(this, "Prompt Message", "Please input a file name !");
		return;
	}

	typedef itk::ImageFileWriter<ItkImageType> WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( fileName.toStdString() );
	writer->SetInput(inputItkImage);
	writer->Write();

}

void MedicalImageTool::on_actionOpenSTL_triggered()
{
// 	if (m_InputImageAllName.isEmpty())
// 	{
// 		QMessageBox::information(this, "Prompt Message", "Please input a image !");
// 		return;
// 	}
	this->stlFileFilter->show();
}

void MedicalImageTool::on_actionCrosshair_triggered()
{
	this->crosshairEdit->show();
}

//--------------------------------------------------------------------------------------
// Edit function
void MedicalImageTool::resliceMode(int mode)
{
	this->crosshairEdit->ui->thickModeCheckBox->setEnabled(mode ? 1 : 0);
	this->crosshairEdit->ui->blendModeGroupBox->setEnabled(mode ? 1 : 0);

	for (int i = 0; i < 3; i++)
	{
		riw[i]->SetResliceMode(mode ? 1 : 0);
		riw[i]->GetRenderer()->ResetCamera();
		riw[i]->Render();
	}
}

void MedicalImageTool::thickMode(int mode)
{
	for (int i = 0; i < 3; i++)
	{
		riw[i]->SetThickMode(mode ? 1 : 0);
		riw[i]->Render();
	}
}

void MedicalImageTool::ResetViews()
{
	// Reset the reslice image views
	for (int i = 0; i < 3; i++)
	{
		riw[i]->Reset();
	}

	// Also sync the Image plane widget on the 3D top right view with any
	// changes to the reslice cursor.
	for (int i = 0; i < 3; i++)
	{
		vtkPlaneSource *ps = static_cast<vtkPlaneSource *>(
			planeWidget[i]->GetPolyDataAlgorithm());
		ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
		ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());

		// If the reslice plane has modified, update it on the 3D widget
		this->planeWidget[i]->UpdatePlacement();
	}

	// Render in response to changes.
	this->riw[0]->Render();
	this->riw[1]->Render();
	this->riw[2]->Render();
}

void MedicalImageTool::on_actionDistanceMeasure_triggered()
{
	this->distMeasEdit->show();
}

void MedicalImageTool::AddDistanceMeasurementToAxial()
{
	this->AddDistanceMeasurementToView(2);
}

void MedicalImageTool::AddDistanceMeasurementToSagital()
{
	this->AddDistanceMeasurementToView(0);
}

void MedicalImageTool::AddDistanceMeasurementToCoronal()
{
	this->AddDistanceMeasurementToView(1);
}

void MedicalImageTool::AddDistanceMeasurementTo3DView()
{
	//	this->AddDistanceMeasurementToView(3);
}

void MedicalImageTool::AddDistanceMeasurementToView(int i)
{
	// remove existing widgets.
	if (this->DistanceWidget[i])
	{
		this->DistanceWidget[i]->SetEnabled(0);
		this->DistanceWidget[i] = NULL;
	}

	// add new widget
	this->DistanceWidget[i] = vtkSmartPointer< vtkDistanceWidget >::New();
	this->DistanceWidget[i]->SetInteractor(
		this->riw[i]->GetResliceCursorWidget()->GetInteractor());

	// Set a priority higher than our reslice cursor widget
	this->DistanceWidget[i]->SetPriority(
		this->riw[i]->GetResliceCursorWidget()->GetPriority() + 0.01);

	vtkSmartPointer< vtkPointHandleRepresentation2D > handleRep =
		vtkSmartPointer< vtkPointHandleRepresentation2D >::New();
	vtkSmartPointer< vtkDistanceRepresentation2D > distanceRep =
		vtkSmartPointer< vtkDistanceRepresentation2D >::New();
	distanceRep->SetHandleRepresentation(handleRep);
	this->DistanceWidget[i]->SetRepresentation(distanceRep);
	distanceRep->InstantiateHandleRepresentation();
	distanceRep->GetPoint1Representation()->SetPointPlacer(riw[i]->GetPointPlacer());
	distanceRep->GetPoint2Representation()->SetPointPlacer(riw[i]->GetPointPlacer());

	// Add the distance to the list of widgets whose visibility is managed based
	// on the reslice plane by the ResliceImageViewerMeasurements class
	this->riw[i]->GetMeasurements()->AddItem(this->DistanceWidget[i]);

	this->DistanceWidget[i]->CreateDefaultRepresentation();
	this->DistanceWidget[i]->EnabledOn();
}

//--------------------------------------------------------------------------------------
// Screenshot
void MedicalImageTool::ViewPaneScreenshot(QVTKWidget *widget)
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
void MedicalImageTool::on_actionScreenshotUpperLeft_triggered()
{
	this->ViewPaneScreenshot(this->ui->qvtkWidgetAxialPlane);
}

void MedicalImageTool::on_actionScreenshotUpperRight_triggered()
{
	this->ViewPaneScreenshot(this->ui->qvtkWidgetSagitalPlane);
}

void MedicalImageTool::on_actionScreenshotLowerLeft_triggered()
{
	this->ViewPaneScreenshot(this->ui->qvtkWidgetCoronalPlane);
}

void MedicalImageTool::on_actionScreenshotLowerRight_triggered()
{
	this->ViewPaneScreenshot(this->ui->qvtkWidget3DView);
}

//-----------------------------------------------------------------------------------------------------
// Volume Renderer
void MedicalImageTool::on_actionGPUVolumeRenderer_triggered()
{
	if (m_InputImageAllName.isEmpty())
	{
		QMessageBox::information(this, "Prompt Message", "Please select a image !");
		return;
	}
	
	this->srcImageRenderer->show();
	this->srcImageRenderer->SetInputImage(m_InputImageAllName);
}

//-----------------------------------------------------------------------------------------------------
// Help function
void MedicalImageTool::on_actionAboutProgram_triggered()
{
	this->aboutHelp->show();
}
