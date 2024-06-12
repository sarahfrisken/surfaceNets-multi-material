//
// appwindow.cpp
//
// Application window
//  + Contains the an OpenGl window and a GUI control panel

#include <string>
#include <vector>
#include <array>

#include "appWindow.h"
#include "mainWindow.h"
#include "glView.h"
#include "materialTable.h"
#include "setValueGroup.h"
#include "newModelDialog.h"
#include "openModelFileDialog.h"

#include "MMSurfaceNet.h"
#include "MMGeometryOBJ.h"

#include <QKeyEvent>
#include <QFileDialog>
#include <QString>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QFile>

// Colors for up to 256 materials. Colors are generated from a base color and a hue shift 
static const QColor baseColors[30] = {
	QColor(66, 169, 251), QColor(247, 221, 32), QColor(227, 74, 61), QColor(77, 157, 61), QColor(233, 198, 255),
	QColor(89, 252, 227), QColor(244, 173, 31), QColor(240, 26, 109), QColor(102, 205, 120), QColor(104, 79, 224),
	QColor(36, 77, 224), QColor(237, 254, 47), QColor(240, 105, 32), QColor(93, 250, 12), QColor(246, 114, 169),
	QColor(82, 222, 253), QColor(246, 204, 32), QColor(245, 112, 97), QColor(69, 163, 85), QColor(165, 149, 246),
	QColor(76, 205, 162), QColor(242, 134, 31), QColor(244, 49, 130), QColor(126, 253, 148), QColor(39, 1, 224),
	QColor(54, 125, 251), QColor(255, 248, 33), QColor(240, 49, 32), QColor(133, 243, 112), QColor(212, 149, 251)
};
static const int hueShift[9] = { 0, 240, 120, 300, 180, 60, 330, 210, 90 };

AppWindow::AppWindow(MainWindow *mw)
	: 
	m_mainWindow(mw),
	m_surfaceNet(nullptr)
{
	// Initialize relaxation attributes
	initRelaxAttrs();

	// Create an OpenGL view widget
	glView = new GLView();
	glView->setFocusPolicy(Qt::StrongFocus);
	m_mainLayout.addWidget(glView, 2);

	// Create a GUI control widget
	m_controlWidget.setLayout(&m_controlLayout);
	m_controlLayout.addStretch();

	// Create sliders for controlling relaxation attributes
	m_surfaceFairingGroupBox.setLayout(&m_surfaceFairingLayout);
	m_surfaceFairingGroupBox.setTitle(tr("Surface Fairing"));
	m_controlLayout.addWidget(&m_surfaceFairingGroupBox, 0);

	m_setMaxOffsetWidget = new SetValueGroup("Max Distance from Voxel Center", 0.0, 3.0, m_relaxAttrs.maxDistFromCellCenter, this);
	m_setNumIterWidget = new SetValueGroup("# Relaxation Iterations", 0.0, 100.0, m_relaxAttrs.numRelaxIterations, this);
	m_setFactorWidget = new SetValueGroup("Relaxation Factor", 0.0, 1.0, m_relaxAttrs.relaxFactor, this);

	m_surfaceFairingLayout.addWidget(m_setMaxOffsetWidget, 0); 
	m_surfaceFairingLayout.addWidget(m_setNumIterWidget, 0); 
	m_surfaceFairingLayout.addWidget(m_setFactorWidget, 0); 

	connect(m_setMaxOffsetWidget, SIGNAL(valueChanged(float)), this, SLOT(setRelaxMaxDist(float)));
	connect(m_setNumIterWidget, SIGNAL(valueChanged(float)), this, SLOT(setRelaxNumIterations(float)));
	connect(m_setFactorWidget, SIGNAL(valueChanged(float)), this, SLOT(setRelaxFactor(float)));

	// Create a table for material rendering properties
	m_renderingGroupBox.setLayout(&m_renderingLayout);
	m_renderingGroupBox.setTitle(tr("Rendering"));
	m_controlLayout.addWidget(&m_renderingGroupBox, 0, Qt::AlignBottom);

	m_renderingLayout.addWidget(&m_materialTable, 0);
	connect(&m_materialTable, SIGNAL(cellClicked(int, int)), this, SLOT(tableCellClicked(int, int)));
	connect(&m_materialTable, &MaterialTable::colorChanged, this, &AppWindow::renderParameterChanged);
	connect(&m_materialTable, &MaterialTable::visibilityChanged, this, &AppWindow::renderParameterChanged);

	// Set the main layout
	m_mainLayout.addWidget(&m_controlWidget);
	setLayout(&m_mainLayout);
}

void AppWindow::keyPressEvent(QKeyEvent *e)
{
	int key = e->key();
	switch (key) {
	case Qt::Key_N:
		onNew();
		break;

	case Qt::Key_Escape:
		m_mainWindow->close();
		break;

	default:
		QWidget::keyPressEvent(e);
	}
}

// Note that while labels between 0 and 65534 are supported by the SurfaceNets library, only 
// the first 256 labels are rendered (to increase this number, the glShader needs to be 
// updated).
void AppWindow::onNewData(unsigned short* data, int arraySize[3], float voxelSize[3])
{
	// Clean up previous SurfaceNet
	delete m_surfaceNet;

	// Make the SurfaceNet
	m_surfaceNet = new MMSurfaceNet(data, arraySize, voxelSize);

	// Use current parameters to relax the SurfaceNet
	m_surfaceNet->relax(m_relaxAttrs);

	// Update the material table. In this application, a material index of zero
	// is used for the background
	m_materialTable.clear();
	std::vector<int> materials = m_surfaceNet->labels();
	std::vector<QColor> colors;
	std::vector<bool> isVisible;
	int numBaseColors = sizeof(baseColors) / sizeof(QColor);
	int numMaterials = materials.size() < 256 ? materials.size() : 256;
	for (int i = 0; i < numMaterials; i++) {
		QColor color = QColor(180, 180, 180);
		int visible = false;
		if (i > 0) {
			int colorIndex = i % numBaseColors;
			color = baseColors[colorIndex];
			int hueIndex = i / numBaseColors;
			if (hueIndex > 0) {
				int hue = (color.hue() + hueShift[hueIndex]) % 360;
				color.setHsv(hue, color.saturation(), color.value());
				color.toRgb();
			}
			visible = true;
		}
		colors.push_back(color);
		isVisible.push_back(visible);
		m_materialTable.addMaterial( materials[i], color, visible);
	}

	// Reset the openGL view and its geometry
	glView->reset();
	glView->makeGeometry(m_surfaceNet);
	glView->updateRenderParameters(colors, isVisible);
	glView->update();
}

void AppWindow::makeSpheres(int numSpheres, int arraySize[3], float voxelSize[3])
{
	int size = arraySize[0] * arraySize[1] * arraySize[2];
	float* dists = new float[size];
	unsigned short* data = new unsigned short[size];
	if ((data == nullptr) || (dists == nullptr) ) {
		if (data != nullptr) delete[] data;
		if (dists != nullptr) delete[] dists;
		return;
	}

	int maxSize = arraySize[0] > arraySize[1] ? arraySize[0] : arraySize[1];
	maxSize = (maxSize > arraySize[2]) ? maxSize : arraySize[2];
	float maxDist = (float)maxSize;
	float* pDist = dists;
	unsigned short* pData = data;
	for (int k = 0; k < arraySize[2]; k++) {
		for (int j = 0; j < arraySize[1]; j++) {
			for (int i = 0; i < arraySize[0]; i++) {
				*pData++ = 0;
				*pDist++ = -maxDist;
			}
		}
	}

	// Add numSphere spheres to the data volume. Each sphere has a unique material index and
	// a random center point and radius.
	int largePrime = 7919;
	float invLargePrime = 1.0f / 7919.0f;
	srand(time(NULL));
	float meanRadius = (float(maxSize)) / sqrt(2.0 + float(numSpheres));
	for (int idx = 0; idx < numSpheres; idx++) {
		unsigned short index = (unsigned short)idx + 1;
		float cx = arraySize[0] * ((rand() % largePrime) * invLargePrime);
		float cy = arraySize[1] * ((rand() % largePrime) * invLargePrime);
		float cz = arraySize[2] * ((rand() % largePrime) * invLargePrime);
		float radius = meanRadius * (0.5f + (rand() % largePrime) * invLargePrime);
		int i0 = std::max(0, (int)(cx - radius));
		int i1 = std::min(arraySize[0] - 1, (int)(cx + radius));
		int j0 = std::max(0, (int)(cy - radius));
		int j1 = std::min(arraySize[1] - 1, (int)(cy + radius));
		int k0 = std::max(0, (int)(cz - radius));
		int k1 = std::min(arraySize[2] - 1, (int)(cz + radius));
		for (int k = k0; k <= k1; k++) {
			float dzSqr = (cz - k) * (cz - k);
			for (int j = j0; j <= j1; j++) {
				float dySqr = (cy - j) * (cy - j);
				for (int i = i0; i <= i1; i++) {
					float dxSqr = (cx - i) * (cx - i);
					float distSqr = dxSqr + dySqr + dzSqr;
					if (distSqr < radius * radius) {
						int idx = i + j * arraySize[0] + k * arraySize[0] * arraySize[1];
						float dist = radius - sqrt(distSqr);
						if (dist > dists[idx]) {
							data[idx] = index;
							dists[idx] = dist;
						}
					}
				}
			}
		}
	}
	delete[] dists;

	onNewData(data, arraySize, voxelSize);
	delete[] data;
}

void AppWindow::importRaw(const char* rawFilename, int bytesPerVoxel, int arraySize[3], float voxelSize[3])
{
	// Open the raw data file and read data
	FILE* fp;
	int openErr = 0;
	if (!(fp = fopen(rawFilename, "rb"))) return;

	unsigned short* data;
	int inputSize = arraySize[0] * arraySize[1] * arraySize[2];
	try {
		data = new unsigned short[inputSize];
	}
	catch (std::bad_alloc& ba) {
		fclose(fp);
		return;
	}

	if (bytesPerVoxel == 2) {
		// Open the raw data file and read data
		int numRead = fread(data, sizeof(unsigned short), inputSize, fp);
		fclose(fp);
		if (numRead != inputSize) {
			fclose(fp);
			delete[] data;
			return;
		}
	}
	else if (bytesPerVoxel == 1) {
		// Open the raw data file, read and convert the data to unsigned shorts
		unsigned char* input;
		int bufferSize = arraySize[0] * arraySize[1];
		try {
			input = new unsigned char[bufferSize];
		}
		catch (std::bad_alloc& ba) {
			fclose(fp);
			delete[] data;
			return;
		}
		unsigned short* pData = data;
		int numRead = 0;
		for (int k = 0; k < arraySize[2]; k++) {
			numRead += fread(input, sizeof(unsigned char), bufferSize, fp);
			unsigned char* pInput = input;
			for (int j = 0; j < arraySize[1]; j++) {
				for (int i = 0; i < arraySize[0]; i++) {
					*pData++ = (unsigned short)*pInput++;
				}
			}
		}
		delete[] input;
		if (numRead != inputSize) {
			fclose(fp);
			delete[] data;
			return;
		}
	}
	else {
		//Other data types not handled
		return;
	}

	// Generate the SurfaceNet
	onNewData(data, arraySize, voxelSize);
	delete[] data;
	fclose(fp);
}

void AppWindow::onNew()
{
	// Open a dialog to get the # spheres, array size and voxel size
	NewModelDialog* dialog = new NewModelDialog(this);

	// Open the dialog to get parameters
	if (dialog->exec() == QDialog::Accepted) {
		int arraySize[3];
		float voxelSize[3];
		int numSpheres = dialog->getNumSpheres();
		dialog->getDimensions(arraySize);
		dialog->getVoxelSize(voxelSize);

		// Create the model and its SurfaceNet
		makeSpheres(numSpheres, arraySize, voxelSize);
	}
}

void AppWindow::onOpen()
{
	// Open a dialog to get the filename, array size and voxel size
	OpenModelFileDialog* dialog = new OpenModelFileDialog();

	// Open the dialog to get parameters
	if (dialog->exec() == QDialog::Accepted) {
		int arraySize[3];
		float voxelSize[3];
		QString filename = dialog->getFilename();
		int bytesPerVoxel = dialog->getBytesPerVoxel();
		dialog->getDimensions(arraySize);
		dialog->getVoxelSize(voxelSize);

		// Import the raw data and create its SurfaceNet
		importRaw(filename.toStdString().c_str(), bytesPerVoxel, arraySize, voxelSize);
	}
}

void AppWindow::onExport()
{
	if (!m_surfaceNet) return;
	MMGeometryOBJ* geometry = new MMGeometryOBJ(m_surfaceNet);

	// Open a dialog to get the export file path
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	QString path = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), QDir::currentPath());

	// Export an OBJ file for each material to the specified path
	std::vector<int> materials = geometry->labels();
	for (std::vector<int>::iterator itMatIdx = materials.begin(); itMatIdx != materials.end(); itMatIdx++) {
		QString filename = path + QString("/") + QString::number(*itMatIdx) + QString(".obj");
		QFile file(filename);
		if (file.open(QIODevice::WriteOnly)) {
			QTextStream stream(&file);
			MMGeometryOBJ::OBJData data = geometry->objData(*itMatIdx);
			for (std::vector<std::array<float, 3>>::iterator v = data.vertexPositions.begin(); v != data.vertexPositions.end(); v++) {
				stream << "v " << (*v)[0] << ' ' << (*v)[1] << ' ' << (*v)[2] << endl;
			}
			for (std::vector< std::array<int, 3>>::iterator t = data.triangles.begin(); t != data.triangles.end(); t++) {
				stream << "f " << (*t)[0] << ' ' << (*t)[1] << ' ' << (*t)[2] << endl;
			}
		}
	}
	delete geometry;
}

void AppWindow::initRelaxAttrs()
{
	m_relaxAttrs.relaxFactor = 0.5f;
	m_relaxAttrs.numRelaxIterations = 20;
	m_relaxAttrs.maxDistFromCellCenter = 1.0f;
}
void AppWindow::onRelax()
{
	// Relax the surface net, reset the geometry and re-render
	m_surfaceNet->reset();
	m_surfaceNet->relax(m_relaxAttrs);
	glView->makeGeometry(m_surfaceNet);
	glView->update();
}
void AppWindow::onReset()
{
	// Reset the surface net and the geometry and re-render. Does not relax the 
	// surface net.
	m_surfaceNet->reset();
	glView->makeGeometry(m_surfaceNet);
	glView->update();
}
void AppWindow::setRelaxFactor(float factor)
{
	m_relaxAttrs.relaxFactor = factor;
	onRelax();
}
void AppWindow::setRelaxMaxDist(float maxDist)
{
	m_relaxAttrs.maxDistFromCellCenter = maxDist;
	onRelax();
}
void AppWindow::setRelaxNumIterations(float numIterations)
{
	m_relaxAttrs.numRelaxIterations = (int)numIterations;
	onRelax();
}
void AppWindow::tableCellClicked(int row, int col)
{
	m_materialTable.editTableCell(row, col);
}
void AppWindow::renderParameterChanged()
{
	std::vector<QColor> colors;
	std::vector<bool> isVisible;
	std::vector<int> materials = m_surfaceNet->labels();
	for (int i = 0; i < materials.size(); i++) {
		colors.push_back(m_materialTable.color(i));
		isVisible.push_back(m_materialTable.visibility(i));
	}
	glView->updateRenderParameters(colors, isVisible);
}

