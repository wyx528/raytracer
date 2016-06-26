#ifndef TEXTURE_H
#define TEXTURE_H

#include "geometry.h"
#include "color.h"
#include <QImage>
#include <QColor>
#include <QApplication>
#include <QFileInfo>

class Texture
{
public:
	int width,height;
	real ratio; //height / width
	Color **color;

	Texture(){}

	Texture(QString filename)
	{
		filename=QApplication::applicationDirPath()+"/resource/img/"+filename;
		QImage *image = new QImage(filename);
		image->convertToFormat(QImage::Format_RGB32);
		width=image->width();
		height=image->height();
		assert(width>0&&height>0);
		ratio=(real)height/width;
		color=new Color*[width];
		qDebug()<<filename<<width<<height;

		for (int i=0;i<width;i++) color[i]=new Color[height];
		for (int i=0;i<width;i++)
			for (int j=0;j<height;j++)
			{
				QColor tmp=image->pixel(i,j);
				color[i][j]=Color(tmp.red(),tmp.green(),tmp.blue())/255;
			}
	}

	inline Color getColorAt(real _x,real _y)
	{
		_x=_x-(int)_x;
		_y=_y-(int)_y;
		int x=_x*width;
		int y=_y*height;
		x=std::min(std::max(0,x),width-1);
		y=std::min(std::max(0,y),height-1);
		return color[x][y];
	}
};

#endif // TEXTURE_H
