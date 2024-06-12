//
// main.cpp
//
// Main entry point
//  + Contains the main window

#include "mainWindow.h"

#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow mainWindow;

	mainWindow.resize(mainWindow.sizeHint());
	int desktopArea = QApplication::desktop()->width() * QApplication::desktop()->height();
	int widgetArea = mainWindow.width() * mainWindow.height();
	if (((float)widgetArea / (float)desktopArea) < 0.75f) {
		mainWindow.show();
	}
	else {
		mainWindow.showMaximized();
	}

	return app.exec();
}