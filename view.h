#ifndef VIEW_H
#define VIEW_H

#include "geometry.h"
#include <QLabel>
#include <QPixmap>
#include <QImage>

class View
{
public:
	Vector eye,center,dw,dh; //position  the center of the screen
	real size; //to the real world
	int width,height;

	View(){}

	View(const Vector &_eye,const Vector &_center,real _size,int _width,int _height);

	void prepare();

	Vector getPoint(real x,real y);

	void rotate(real alpha);
};

#endif // VIEW_H
