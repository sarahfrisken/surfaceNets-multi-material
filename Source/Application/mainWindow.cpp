//
// mainwindow.cpp
//
// Main window with title and menu bar
//  + Contains the application window

#include "mainWindow.h"
#include "appWindow.h"

#include <QMenuBar>
#include <QMenu>

MainWindow::MainWindow()
{
	// Window widget containing an OpenGL window and Qt controls
	appWindow = new AppWindow(this);
	setCentralWidget(appWindow);

	// Menu bar
	QMenuBar *menuBar = new QMenuBar;
	setMenuBar(menuBar);

	// File menu
	QMenu *menuFile = menuBar->addMenu(tr("&File"));
	QAction *New = new QAction(menuFile);
	QAction *Open = new QAction(menuFile);
	QAction *Export = new QAction(menuFile);
	QAction *Exit = new QAction(menuFile);

	New->setText(tr("New"));
	Open->setText(tr("Open"));
	Export->setText(tr("Export"));
	Exit->setText(tr("Exit"));

	New->setShortcut(QKeySequence::New);
	Open->setShortcut(QKeySequence::Open);
	Exit->setShortcut(QKeySequence::Quit);

	menuFile->addAction(New);
	menuFile->addAction(Open);
	menuFile->addAction(Export);
	menuFile->addAction(Exit);

	connect(New, &QAction::triggered, appWindow, &AppWindow::onNew);
	connect(Open, &QAction::triggered, appWindow, &AppWindow::onOpen);
	connect(Export, &QAction::triggered, appWindow, &AppWindow::onExport);
	connect(Exit, &QAction::triggered, this, &MainWindow::onExit);

	// Edit menu
	QMenu *menuEdit = menuBar->addMenu(tr("&Edit"));
	QAction *Relax = new QAction(menuEdit);
	QAction *Reset = new QAction(menuEdit);

	Relax->setText(tr("Relax"));
	Reset->setText(tr("Reset"));

	Relax->setShortcut(Qt::Key_R);
	Reset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

	menuEdit->addAction(Relax);
	menuEdit->addAction(Reset);

	connect(Relax, &QAction::triggered, appWindow, &AppWindow::onRelax);
	connect(Reset, &QAction::triggered, appWindow, &AppWindow::onReset);
}

void MainWindow::onExit()
{
	close();
}