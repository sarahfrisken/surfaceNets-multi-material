//
// mainwindow.h
//
// Main window with title and menu bar
//  + Contains the application window

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class AppWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

	private slots:
	void onExit();

private:
	AppWindow *appWindow;
};

#endif