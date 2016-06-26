#include <QTime>
#include <QFile>
#include <QIODevice>
#include "meshobject.h"
#include "meshsimplification.h"

using std::min;
using std::max;

Vector MeshObject::smoothNorm[300000];

MeshObject::MeshObject(const Vector &_center,real _size,Material *_material,const QString &_filename,
					   real _useSmooth,real rate)
:center(_center),size(_size),material(_material),filename(_filename),useSmooth(_useSmooth)
{
	read();
	adjust();
	simplify(rate);
	finish();
}

MeshObject::~MeshObject()
{
	point.clear();
	face.clear();
	for (int i=0;i<(int)object.size();i++) delete object[i];
	object.clear();
	for (int i=0;i<(int)mesh.size();i++) delete mesh[i];
	mesh.clear();
}


void MeshObject::read()
{
	qDebug()<<filename;
	filename=QApplication::applicationDirPath()+"/resource/obj/"+filename;
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
	{
		qDebug()<<"Can not open obj file!";
		return;
	}
	QTextStream in(&file);
	int minNumber=100000000;
	while (!in.atEnd())
	{
		QString now;
		in>>now;
		if (now=="v")
		{
			real x,y,z;
			in>>x>>y>>z;
			point.push_back(Vector(x,y,z));
		}
		else if (now=="f")
		{
			int x,y,z;
			in>>x>>y>>z;
			face.push_back(Face(x,y,z));
			minNumber=min(minNumber,min(x,min(y,z)));
		}
	}
	for (int i=0;i<(int)face.size();i++)
		for (int j=0;j<3;j++) face[i].p[j]-=minNumber; //number starts at 0
}

void MeshObject::adjust()
{
	bound=AABB(point);
	real mult=size/bound.size().getMax();
	Vector total=Vector::zero();
	for (int i=0;i<(int)point.size();i++) total+=point[i];
	total/=point.size();
	for (int i=0;i<(int)point.size();i++) point[i]=center+(point[i]-total)*mult;
	bound=AABB(point);
	bound.print();
}

void MeshObject::simplify(real rate)
{
	qDebug()<<"mesh simplification start! nVertex:"<<point.size()<<"nFace:"<<face.size();
	MeshSimplification *simp=new MeshSimplification(point,face,rate);
	simp->simplify();
	pair< vector<Vector>, vector<Face> >res=simp->getResult();
	delete simp;
	point=res.first;
	face=res.second;
	qDebug()<<"mesh simplification end! nVertex:"<<point.size()<<"nFace:"<<face.size();
}

void MeshObject::finish()
{
	//prepare mesh and object
	for (int i=0;i<(int)face.size();i++)
	{
		int *p=face[i].p;
		mesh.push_back(new Mesh(point[p[0]],point[p[1]],point[p[2]]));
		object.push_back(new Object(mesh[i],material));
	}
	bound.loose(); //very important!

	QTime runTime;
	runTime.start();

	tree.build(mesh);

	qDebug()<<"KDTree Build! nMeshes:"<<mesh.size()<<"cost time:"<<(real)runTime.elapsed()/1000<<"s.";

	if (useSmooth)
	{
		int n=(int)point.size(),m=(int)face.size();
		for (int i=0;i<n;i++) smoothNorm[i]=Vector::zero();
		for (int i=0;i<m;i++)
		{
			Vector norm=mesh[i]->plane.norm;
			for (int j=0;j<3;j++)
			{
				int p0=face[i].p[j],p1=face[i].p[(j+1)%3],p2=face[i].p[(j+2)%3];
				Vector u=point[p1]-point[p0],v=point[p2]-point[p0];
			//	u.normalize(),v.normalize();
			//	real angle=acos(dot(u,v));
				real area=cross(u,v).mod();
				smoothNorm[face[i].p[j]]+=norm*area;
			}
		}
		for (int i=0;i<n;i++) smoothNorm[i].normalize();
		for (int i=0;i<m;i++)
		{
			mesh[i]->setUseSmooth();
			mesh[i]->setSmoothNorm(smoothNorm[face[i].p[0]],smoothNorm[face[i].p[1]],smoothNorm[face[i].p[2]]);
		}
	}
}

Object* MeshObject::getObject(int num)
{
	return object[num];
}

Object* MeshObject::getIntersection(const Ray &ray)
{
	//need to use KDTree to improve!!!
	int res=tree.getIntersection(ray);
	if (res<0) return NULL;
	return getObject(res);
//	if (bound.getIntersection(ray)<0) return NULL;
//	Object *res=NULL;
//	real minDistance=1e100;
//	for (int i=0;i<(int)object.size();i++)
//	{
//		Object *obj=object[i];
//		real distance=obj->shape->getIntersection(ray);
//		if (distance<0||distance>=minDistance) continue;
//		minDistance=distance;
//		res=obj;
	//	}
}

