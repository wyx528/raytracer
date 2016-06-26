#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	assert(TOTAL_THREAD>=1&&TOTAL_THREAD<=8);
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	ui->setupUi(this);
	setWindowTitle("Ray Tracing writted by Wang Yingxu CST 42 Tsinghua University");
	resize(920,920);
	QDesktopWidget *desktop=QApplication::desktop();
	int w=desktop->width();
	int h=desktop->height();
	move((w-width())/2,(h-height())/2);

	label=new QLabel();
	ui->scrollImage->setMinimumHeight(800);
	ui->scrollImage->setMinimumWidth(800);

	connect(ui->buttonRefraction,SIGNAL(clicked()),this,SLOT(traceRefraction()));
	connect(ui->buttonMesh,SIGNAL(clicked()),this,SLOT(traceMesh()));
	connect(ui->buttonDragon,SIGNAL(clicked()),this,SLOT(traceDragon()));
	connect(ui->buttonDepthOfField,SIGNAL(clicked()),this,SLOT(traceDepthOfField()));
	connect(ui->buttonPathTracing,SIGNAL(clicked()),this,SLOT(tracePathTracing()));

	refreshTimer=new QTimer(this);
	connect(refreshTimer,SIGNAL(timeout()),this,SLOT(refresh()));
}

MainWindow::~MainWindow()
{
    delete ui;
	delete label;
	delete image;
	delete render;
	delete refreshTimer;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	for (int i=1;i<TOTAL_THREAD;i++)
		if (thread[i].isRunning()) thread[i].terminate();
	exit(0);
}

void MainWindow::refresh()
{
	QString str="Runtime: "+QString::number((real)runTime.elapsed()/1000,'f',3)+"s.";
	ui->timeLabel->setText(str);
	label->resize(image->width(),image->height());
	label->setPixmap(QPixmap::fromImage(*image));
	ui->scrollImage->setWidget(label);
}

void MainWindow::finish()
{
	ui->timeLabel->clear();

	runTime.start();

	image=render->getImage();

	for (int i=1;i<TOTAL_THREAD;i++) thread[i].setThread(render,i,TOTAL_THREAD);
	for (int i=1;i<TOTAL_THREAD;i++) thread[i].start();
	refreshTimer->start(100);
	render->trace(0,TOTAL_THREAD);
	for (int i=1;i<TOTAL_THREAD;i++) thread[i].wait();
	refreshTimer->stop();

	image=render->getResult();
	label->resize(image->width(),image->height());
	label->setPixmap(QPixmap::fromImage(*image));
	ui->scrollImage->setWidget(label);

	qDebug()<<"Tracing time:"<<(real)runTime.elapsed()/1000<<"s.";

	QDateTime time=QDateTime::currentDateTime();
//	QString name="90.png";
	QString name=time.toString("yyyy-MM-dd_hh-mm-ss")+".bmp";
	image->save(name);

	delete render;
}

void MainWindow::traceRefraction()
{
//	TextureMaterial* TM_BLACK_WHITE=new TextureMaterial(40,0.1,0.9,0.05,0,"grid.bmp",8);
	render=new Phong;
	render->setSoftShadow(true);
	render->setSoftShadowConfig(100,6);
	render->setView(new View(Vector(38,22,50),Vector(38,22,0),35,800,800));
	render->addLight(new Light(Vector(95,22,60),COLOR_WHITE,20));
	render->addObject(new Object(new Plane(Vector(0,0,1),0),MATERIAL_LIGHT_RED));
	for (int i=28;i<=50;i+=4.5)
		for (int j=12;j<=32;j+=4.5) render->addObject(new Object(new Sphere(Vector(i,j,2),1.5),MATERIAL_GREEN));
	render->addObject(new Object(new Sphere(Vector(39,23,20),3),MATERIAL_CRYSTAL));
	render->finish();
	finish();
}

void MainWindow::traceMesh()
{
	//TextureMaterial* TM_BLACK_WHITE=new TextureMaterial(40,0.1,0.9,0.05,0,"grid.bmp",4);
	TextureMaterial* TM_SNOW=new TextureMaterial(40,0.1,0.9,0.05,0,"snow.jpg",60);
	render=new Phong;
	render->setSoftShadow(true);
	render->setSoftShadowConfig(20,4);
	render->setView(new View(Vector(38,22,50),Vector(38,22,0),30,800,800));
	render->addLight(new Light(Vector(78,72,60),COLOR_WHITE,25));
	render->addObject(new Object(new Plane(Vector(0,0,1),3.2),TM_SNOW));
	render->addMeshObject(new MeshObject(Vector(43,14,0),8,MATERIAL_GREEN,"Arma.obj"));
	render->addMeshObject(new MeshObject(Vector(45,28,0),8,MATERIAL_GRAY,"bunny.fine.obj"));
	render->addMeshObject(new MeshObject(Vector(33,30,0),10,MATERIAL_LIGHT_RED,"teapot.obj"));//10
	render->addMeshObject(new MeshObject(Vector(31,17,0),12,MATERIAL_CORNSILK,"Buddha.obj"));
	render->finish();
	finish();
}

void MainWindow::traceDragon()
{
	TextureMaterial* TM_SEA=new TextureMaterial(40,0,1,0,0,"sea.jpg",45);
	render=new Phong;
	render->setMaxTraceDepth(10);
	render->setView(new View(Vector(23,12.7,50),Vector(23,12.7,0),40,1920*2,1080*2));
	render->addLight(new Light(Vector(83,72.7,60),COLOR_WHITE,50));
	render->addObject(new Object(new Plane(Vector(0,0,1),1.1),TM_SEA));
	render->addMeshObject(new MeshObject(Vector(26,11.7,20),10,MATERIAL_CRYSTAL,"fixed.perfect.dragon.100K.0.07.obj",true,0.6));
	render->finish();
	finish();
}

void MainWindow::traceDepthOfField()
{
	TextureMaterial* TM_GROUND=new TextureMaterial(40,0,1,0,0,"grid.bmp",6);
	render=new Phong;
//	render->setSoftShadow(true);
	render->setSoftShadowConfig(30,3);
	render->setDepthOfField(true);
	render->setDepthOfFieldConfig(1,77,300);
	Vector start=Vector(0,-50,50),direction=Vector(0,50,-40);
	render->setView(new View(start,start+direction*1.4,70,800,800));
	render->addObject(new Object(new Plane(Vector(0,0,1),0),TM_GROUND));
	render->addLight(new Light(Vector(0,20,40),COLOR_YELLOW,8));
	render->addLight(new Light(Vector(20,60,40),COLOR_WHITE,8));
	render->addLight(new Light(Vector(-20,60,40),COLOR_WHITE,8));
	render->addObject(new Object(new Sphere(Vector(-10,4,4),3.5),MATERIAL_CYAN));
	render->addObject(new Object(new Sphere(Vector(6,18,6),5.5),MATERIAL_CYAN)); //center
	render->finish();
	finish();
}

void MainWindow::tracePathTracing()
{

	TextureMaterial* TM_GROUND=new TextureMaterial(40,0,1,0,0,"thu.jpg",43);
	render=new PathTracing;
	render->setMaxTraceDepth(10);
	render->setPathTracingSampleCount(15000); //10000 200 1500s
//	render->addSphereLight(Vector(200,150,150),100,COLOR_WHITE,30);
	render->addSphereLight(Vector(500,150,150),200,COLOR_WHITE,55);
	render->setView(new View(Vector(22,12.7,50),Vector(22,12.7,0),40,1920,1080));
	render->addObject(new Object(new Plane(Vector(0,0,1),0),TM_GROUND));
	real m=22.5,d=10.5,z=4;

	render->addMeshObject(new MeshObject(Vector(m,8,z),10,new Material(Color(1,1,1),0,0,0,0,1),"dragon2.obj",true));
	render->addMeshObject(new MeshObject(Vector(m-d,8,z),10,new Material(Color(1,1,1),0,0,0,0,1),"dragon2.obj",true));
	render->addMeshObject(new MeshObject(Vector(m+d,8,z),10,new Material(Color(1,1,1),0,0,0,0,1),"dragon2.obj",true));

	render->finish();
	finish();

	/*
	TextureMaterial* TM_FLOOR=new TextureMaterial(40,0,1,0,0,"table30.jpg",200);
	render=new PathTracing;
	render->setPathTracingSampleCount(2000);
	render->setMaxTraceDepth(10);
	real dx=65,dy=45;
	render->setView(new View(Vector(dx+0,dy-150,100),Vector(dx+0,dy+0,0),100,1920,1080));
	render->addSphereLight(Vector(dx,dy-11.5,1000),500,COLOR_WHITE,6);
	render->addObject(new Object(new Plane(Vector(0,0,1),0),TM_FLOOR));
	render->addMeshObject(new MeshObject(Vector(dx+18,dy-11.5,8.8),30,new Material(Color(1,1,1),0,0,0,0,1),"dragon.obj",true));
	render->addMeshObject(new MeshObject(Vector(dx-18,dy-11.5,8.8),30,new Material(Color(1,1,1),0,0,0,0,1),"dragon_.obj",true));
//	render->addObject(new Object(new Sphere(Vector(dx,dy-13.5,20),3),new Material(Color(1,1,0.8),0,0,0,0,1)));
	render->finish();
	finish();
	*/
}
