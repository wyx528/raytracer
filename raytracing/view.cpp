#include "view.h"
#include "object.h"
#include "light.h"
#include "geometry.h"
#include <vector>

View::View(const Vector &_eye,const Vector &_center,real _size,int _width,int _height)
	:eye(_eye),center(_center),size(_size),width(_width),height(_height)
{
	Vector dir=(eye-center).getNormalize();
	Vector tmp;
	if (dir.x>EPS||dir.y>EPS) tmp=Vector(-dir.y,dir.x,0);
	else tmp=Vector(-dir.z,0,dir.x);
	dw=tmp;
	dh=dir.cross(dw);
//	dw.print();	dh.print();
	prepare();
}

void View::prepare()
{
	dw=dw.getNormalize()*size/width;
	dh=dh.getNormalize()*size/width;
}

Vector View::getPoint(real w,real h)
{
	return center+dw*(width-width/2-1-w)+dh*(h-height/2);
}

void View::rotate(real alpha)
{
	alpha=M_PI*alpha/180;
	dw.normalize();
	dh.normalize();
	Vector _dw=dw*cos(alpha)+dh*sin(alpha);
	Vector _dh=dh*cos(alpha)-dw*sin(alpha);
	dw=_dw,dh=_dh;
	prepare();
}
