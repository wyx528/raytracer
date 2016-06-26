#ifndef RENDER_H
#define RENDER_H

#include "geometry.h"
#include "light.h"
#include "object.h"
#include "color.h"
#include "view.h"
#include "global.h"
#include "meshobject.h"
#include <vector>
#include <queue>
#include <QImage>
#include <QColor>
#include <QtCore>
#include <QtGui>

using namespace std;

typedef pair<int,int> PII;

class Render
{
protected:
	bool useSoftShadow; //only use in PHONG
	bool useDepthOfField;

	int MAX_TRACE_DEPTH;
	int SOFT_SHADOW_LIGHT;
	real SOFT_SHADOW_LIGHT_RADIUS;
	int DOF_SAMPLE_CNT;
	real APERTURE_RADIUS;
	real CAMERA_FOCUS;
	int PATH_TRACING_SAMPLE_CNT; //only use in PATH TRACING

	vector<Object*>object;
	vector<Object*>sphereLight; //only use in PATH TRACING
	vector<Light*>light;
	vector<MeshObject*>meshObject;
	View *view;
	QImage *image;
	Color **imageColor;
	bool **isDonePoint;
	int **belong;
	priority_queue< PII,vector<PII>,greater<PII> >queue;

	Object* getNearestObject(const Ray &ray,bool includeLight=false,real limit=1e100); //only need result<limit

public:
	Render();

	~Render();

	void setMaxTraceDepth(int _depth);

	void setSoftShadowConfig(int _cnt,real _radius);

	void setDepthOfFieldConfig(real _radius,real _focus,int _cnt=0);

	void setPathTracingSampleCount(int _cnt);


	void setView(View *_view);

	QImage* getImage();

	QImage* getResult();

	void addObject(Object *obj);

	void addLight(Light* lt);

	void addSphereLight(Vector _position,real _radius,Color _color,real _intensity);

	void addMeshObject(MeshObject *_meshobject);

	void setSoftShadow(bool);

	void setDepthOfField(bool);

	void rotateView(real alpha); //rotate clockwise alpha(-180 ~ 180)

	virtual void finish(){}

	virtual void trace(int numThread,int totalThread){}

};

#endif // RENDER_H
