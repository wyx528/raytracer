#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "geometry.h"
#include "view.h"
#include "light.h"
#include "geometry.h"
#include "global.h"
#include "phong.h"
#include "thread.h"
#include "meshobject.h"
#include "pathtracing.h"

#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QDesktopWidget>
#include <QTimer>
#include <QTime>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

	void closeEvent(QCloseEvent *event);

	void refresh();

	void traceRefraction();

	void traceMesh();

	void traceDragon();

	void traceDepthOfField();

	void tracePathTracing();

private:
	Ui::MainWindow *ui;
	QLabel *label;
	QImage *image;
	Render *render;
	QTimer *refreshTimer;
	QTime runTime;
	Thread thread[10];

	void finish();
};

#endif // MAINWINDOW_H
