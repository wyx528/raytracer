#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
	QApplication a(argc, argv);
	MainWindow w;
    w.show();

    return a.exec();
}
