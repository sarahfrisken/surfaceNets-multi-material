//
// appWindow.h
//
// Application window
//  + Contains the an OpenGl window and a GUI control panel

#ifndef WINDOW_H
#define WINDOW_H

#include "MMSurfaceNet.h"

#include "MaterialTable.h"
#include "SetValueGroup.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QRadioButton>
#include <QGroupBox>
#include <QSlider>
#include <QTextEdit>

class GLView;
class MainWindow;

class AppWindow : public QWidget
{
	Q_OBJECT

public:
	AppWindow(MainWindow *mw);

public slots:
	void onNew();
	void onOpen();
	void onExport();
	void onRelax();
	void onReset();

private slots:
	void setRelaxFactor(float factor);
	void setRelaxMaxDist(float maxDist);
	void setRelaxNumIterations(float numIterations);
	void tableCellClicked(int, int);
	void renderParameterChanged();

private:
	void keyPressEvent(QKeyEvent* event) override;

	// Main window contains an OpenGL window and a control panel 
	MainWindow *m_mainWindow;
	QHBoxLayout m_mainLayout;
	QWidget m_controlWidget;
	GLView* glView;

	// Control panel contains a group of sliders to control surface relaxation and a
	// table for setting material visibility and color
	QVBoxLayout m_controlLayout;
	QGroupBox m_surfaceFairingGroupBox;
	QVBoxLayout m_surfaceFairingLayout;

	SetValueGroup *m_setFactorWidget;
	SetValueGroup *m_setNumIterWidget;
	SetValueGroup *m_setMaxOffsetWidget;

	QGroupBox m_renderingGroupBox;
	QVBoxLayout m_renderingLayout;

	MaterialTable m_materialTable;

	// Model generation and input
	void makeSpheres(int numSpheres, int arraySize[3], float voxelSize[3]);
	void importRaw(const char* rawFilename, int bytesPerVoxel, int arraySize[3], float voxelSize[3]);
	void onNewData(unsigned short*data, int arraySize[3], float voxelSize[3]);

	// SurfaceNet
	MMSurfaceNet *m_surfaceNet;
	MMSurfaceNet::RelaxAttrs m_relaxAttrs;
	void initRelaxAttrs();
};

#endif