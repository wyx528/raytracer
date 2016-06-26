#include "render.h"

Render::Render()
	:useSoftShadow(false),useDepthOfField(false),
	  MAX_TRACE_DEPTH(3),
	  SOFT_SHADOW_LIGHT(50),SOFT_SHADOW_LIGHT_RADIUS(5),
	  PATH_TRACING_SAMPLE_CNT(20)
{
	object.clear();
	sphereLight.clear();
	light.clear();
	meshObject.clear();
}

Render::~Render()
{
	for (int i=0;i<(int)object.size();i++) delete object[i];
	for (int i=0;i<(int)light.size();i++) delete light[i];
	for (int i=0;i<(int)meshObject.size();i++) delete meshObject[i];
	delete view;
	delete image;
	for (int i=0;i<view->width;i++)
	{
		delete[] imageColor[i];
		delete[] isDonePoint[i];
	}
	delete[]imageColor;
	delete[]isDonePoint;
}

void Render::setMaxTraceDepth(int _depth)
{
	MAX_TRACE_DEPTH=_depth;
}

void Render::setSoftShadowConfig(int _cnt, real _radius)
{
	SOFT_SHADOW_LIGHT=_cnt;
	SOFT_SHADOW_LIGHT_RADIUS=_radius;
}

void Render::setDepthOfFieldConfig(real _radius,real _focus,int _cnt)
{
	APERTURE_RADIUS=_radius;
	CAMERA_FOCUS=_focus;
	DOF_SAMPLE_CNT=_cnt;
}

void Render::setView(View *_view)
{
	view=_view;
	view->prepare();
	int width=view->width;
	int height=view->height;

	image=new QImage(width,height,QImage::Format_RGB32);
	imageColor=new Color*[width];
	isDonePoint=new bool*[width];
	belong=new int*[width];

	for (int i=0;i<width;i++)
	{
		imageColor[i]=new Color[height];
		isDonePoint[i]=new bool[height];
		belong[i]=new int[height];
	}

	while(queue.size()) queue.pop();
	int s=0,t=1;
	for (int i=0;i<width;i++)
		for (int j=0;j<height;j++)
		{
			isDonePoint[i][j]=false;
			belong[i][j]=(s++)%TOTAL_THREAD;
			if (belong[i][j]==0&&randReal()<0.05) belong[i][j]=t++;
			if (t==TOTAL_THREAD) t=1;
			queue.push(make_pair(i,j));
		}
}

void Render::addObject(Object *obj)
{
	object.push_back(obj);
}

void Render::addLight(Light* lt)
{
	if (!useSoftShadow)
	{
		light.push_back(lt);
		return;
	}
	lt->intensity/=SOFT_SHADOW_LIGHT;
	light.push_back(lt);
	for (int i=1;i<SOFT_SHADOW_LIGHT;i++)
	{
		Light *now=new Light(*lt);
		real theta=randReal(2*M_PI),radius=randReal(SOFT_SHADOW_LIGHT_RADIUS);
		Vector move=view->dw.getNormalize()*cos(theta)+view->dh.getNormalize()*sin(theta);
		now->position+=move*radius;
//		now->position.print();
		light.push_back(now);
	}
}

void Render::addSphereLight(Vector _position, real _radius, Color _color, real _intensity)
{
	Material *material=new Material(COLOR_NONE,0,0,1,0,0,_color*_intensity);
	sphereLight.push_back(new Object(new Sphere(_position,_radius),material));
}

void Render::addMeshObject(MeshObject *_meshObject)
{
	meshObject.push_back(_meshObject);
}

void Render::setSoftShadow(bool flag)
{
	useSoftShadow=flag;
}

void Render::setDepthOfField(bool flag)
{
	useDepthOfField=flag;
}

void Render::setPathTracingSampleCount(int _cnt)
{
	PATH_TRACING_SAMPLE_CNT=_cnt;
}

void Render::rotateView(real alpha)
{
	view->rotate(alpha);
}

Object* Render::getNearestObject(const Ray &ray, bool includeLight, real limit)
{
	Object *nearest=NULL;
	real minDistance=1e100;
	for (int i=0;i<(int)object.size();i++)
	{
		Object *obj=object[i];
		real distance=obj->getIntersectionDistance(ray);
		if (distance<0||distance>limit) continue;
		if (distance<minDistance)
		{
			minDistance=distance;
			nearest=obj;
		}
	}
	for (int i=0;i<(int)meshObject.size();i++)
	{
		Object *obj=meshObject[i]->getIntersection(ray);
		if (obj==NULL) continue;
		real distance=obj->getIntersectionDistance(ray);
		if (distance<0||distance>limit) continue;
		if (distance<minDistance)
		{
			minDistance=distance;
			nearest=obj;
		}
//		pair<real,int> intersection=meshobject[i]->getIntersecion(ray);
//		real distance=intersection.first;
//		int number=intersection.second;
//		if (number<0||distance>limit) continue;
//		if (distance<minDistance)
//		{
//			minDistance=distance;
//			nearest=meshobject[i]->getObject(number);
//		}
	}
	if (!includeLight) return nearest;
	for (int i=0;i<(int)sphereLight.size();i++)
	{
		Object *obj=sphereLight[i];
		real distance=obj->getIntersectionDistance(ray);
		if (distance<0||distance>limit) continue;
		if (distance<minDistance)
		{
			minDistance=distance;
			nearest=obj;
		}
	}
	return nearest;
}

//void Render::setImage(QImage *&_image)
//{
//	image=_image;
////	image=new QImage(view->width,view->height,QImage::Format_RGB32);
//	for (int i=0;i<image->width();i++)
//		for (int j=0;j<image->height();j++)
//		{
//			for (int k=0;k<100000;k++);
//			image->setPixel(i,j,qRgb(255,0,0));
//			QApplication::processEvents();
//		}
//}

QImage* Render::getImage()
{
	return image;
}

QImage * Render::getResult()
{
	int width=view->width;
	int height=view->height;
	for (int i=0;i<width;i++)
		for (int j=0;j<height;j++)
		{
			Color color=imageColor[i][j]*255;
			image->setPixel(i,j,qRgb(color.r,color.g,color.b));
		}
	return image;
}

