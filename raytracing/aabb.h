#ifndef AABB_H
#define AABB_H

#include "geometry.h"
#include <vector>

class AABB
{
public:
	Vector min,max;

	AABB():min(Vector::inf()),max(-Vector::inf()){}

	AABB(const Vector &_min,const Vector &_max):min(_min),max(_max)
	{
		for (int i=0;i<3;i++) assert(_max[i]>_min[i]);
	}

	AABB(const std::vector<Vector> &point):min(Vector::inf()),max(-Vector::inf())
	{
		for (int i=0;i<(int)point.size();i++)
		{
			min.updateMin(point[i]);
			max.updateMax(point[i]);
		}
	}

	AABB(const std::vector<Mesh*> &mesh):min(Vector::inf()),max(-Vector::inf())
	{
		for (int i=0;i<(int)mesh.size();i++) update(mesh[i]);
	}

	void init()
	{
		min=Vector::inf();
		max=-Vector::inf();
	}

	void update(Mesh *mesh)
	{
		min.updateMin(mesh->p[0]);
		min.updateMin(mesh->p[1]);
		min.updateMin(mesh->p[2]);
		max.updateMax(mesh->p[0]);
		max.updateMax(mesh->p[1]);
		max.updateMax(mesh->p[2]);
	}

//	inline bool contain(const Vector &v)
//	{
//		return v[0]>min[0]-1e-3&&v[0]<max[0]+1e-3
//			   &&v[1]>min[1]-1e-3&&v[1]<max[1]+1e-3
//			   &&v[2]>min[2]-1e-3&&v[2]<max[2]+1e-3;
//	}

	inline Vector size() const
	{
		return max-min;
	}

	inline void loose()
	{
		min-=Vector::eps()*10;
		max+=Vector::eps()*10;
	}

	inline real getSurfaceArea() //only used in buliding KDTree
	{
		Vector now=size();
		return now[0]*now[1]+now[1]*now[2]+now[2]*now[0];
	}

	void print()
	{
		qDebug()<<"AABB boundbox:";
		qDebug()<<"Min:";
		min.print();
		qDebug()<<"Max:";
		max.print();
	}

	real getIntersection(const Ray &ray)
	{
		Vector inv=Vector(1.0/ray.direction.x,1.0/ray.direction.y,1.0/ray.direction.z);
		real tmin=-INF,tmax=INF;
		real _tmin,_tmax;
		bool sign;


#define TEST(t)\
	if (fabs(ray.direction.t)>EPS)\
	{\
		sign=(inv.t<0);\
		_tmin=((sign?max.t:min.t)-ray.origin.t)*inv.t;\
		_tmax=((sign?min.t:max.t)-ray.origin.t)*inv.t;\
		if (_tmin>tmin) tmin=_tmin;\
		if (_tmax<tmax) tmax=_tmax;\
		if (tmin+EPS>tmax) return -1;\
	}
	TEST(x); TEST(y); TEST(z);
#undef UPDATE


		if (tmax<0) return -1;
		if (tmin<0) return tmax;
		return tmin;
//		Plane a,b;
//		real res=INF;
//		for (int i=0;i<3;i++)
//		{
//			a=Plane(Vector(i==0,i==1,i==2),-min[i]);
//			b=Plane(Vector(i==0,i==1,i==2),-max[i]);
//			real t1=a.getIntersection(ray),t2=b.getIntersection(ray);
//			if (t1>0&&t1<res&&contain(ray.getPoint(t1))) res=t1;
//			if (t2>0&&t2<res&&contain(ray.getPoint(t2))) res=t2;
//		}
//		if (res>INF/2) res=-1;
//		return res;
//		Plane a,b;
//		real tmin=-INF,tmax=INF;
//		//tmin=max(1,2,3min) tmax=min(1,2,3max)

//		//the origin of the ray is in the box
//		if (contain(ray.origin))
//		{
//			real res=INF;
//			for (int i=0;i<3;i++)
//			{
//				a=Plane(Vector(i==0,i==1,i==2),-min[i]);
//				b=Plane(Vector(i==0,i==1,i==2),-max[i]);
//				real t1=a.getIntersection(ray),t2=b.getIntersection(ray);
//				if (t1>0&&t1<res) res=t1;
//				if (t2>0&&t2<res) res=t2;
//			}
//			return res;
//		}
//		for (int i=0;i<3;i++)
//		{
//			a=Plane(Vector(i==0,i==1,i==2),-min[i]);
//			b=Plane(Vector(i==0,i==1,i==2),-max[i]);
//			real _tmin=a.getIntersection(ray),_tmax=b.getIntersection(ray);
////			assert((_tmin>-EPS&&_tmax>-EPS)||(_tmin==-1&&_tmax==-1));
//			if (_tmin<0||_tmax<0) continue;
//			if (_tmin>_tmax) swap(_tmin,_tmax);
//			if (_tmin>tmin) tmin=_tmin;
//			if (_tmax<tmax) tmax=_tmax;
//		}
//		if (tmin<tmax) return tmin;
//		return -1;
	}
};

#endif // AABB_H
