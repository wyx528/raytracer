#ifndef GEOMERTY_H
#define GEOMERTY_H

#include "constant.h"
#include <cmath>
#include <cassert>
#include <QDebug>

class Vector
{
public:
	real x,y,z;

	Vector():x(0),y(0),z(0){}

	explicit Vector(real _x,real _y,real _z):x(_x),y(_y),z(_z){}

	inline real sqr() const
	{
		return x*x + y*y + z*z;
	}

	inline real mod() const
	{
		return sqrt(sqr());
	}

	friend real dot(const Vector &u,const Vector &v)
	{
		return u.x*v.x+u.y*v.y+u.z*v.z;
	}

	inline real dot(const Vector &v) const
	{
		return x*v.x + y*v.y + z*v.z;
	}

	inline Vector cross(const Vector&v) const
	{
		return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}

	friend Vector cross(const Vector &v,const Vector &u)
	{
		return v.cross(u);
	}

	Vector& operator = (const Vector &v)
	{
		x = v.x, y = v.y, z = v.z;
		return *this;
	}

	void normalize()
	{
		real m = mod();
		if (fabs(m)<EPS) return;
		m = 1.0 / m;
		x *= m, y *= m, z *= m;
	}

	Vector getNormalize() const
	{
		Vector res(*this);
		res.normalize();
		return res;
	}

	inline Vector operator + (const Vector &v) const
	{
		return Vector(x+v.x, y+v.y, z+v.z);
	}

	inline Vector& operator += (const Vector &v)
	{
		x+=v.x, y+=v.y, z+=v.z;
		return *this;
	}

	inline Vector operator - () const
	{
		return Vector(-x, -y, -z);
	}

	inline Vector operator - (const Vector &v) const
	{
		return (*this) + (-v);
	}

	inline Vector operator -= (const Vector &v)
	{
		return (*this) += (-v);
	}

	inline Vector operator * (const real v) const
	{
		return Vector(x*v, y*v, z*v);
	}

	inline Vector operator * (const Vector &v) const
	{
		return Vector(x*v.x,y*v.y,z*v.z);
	}

	inline Vector operator / (const real v) const
	{
		return (*this) * (1.0/v);
	}

	inline Vector& operator *= (const real v)
	{
		x*=v, y*=v, z*=v;
		return *this;
	}

	inline Vector& operator /= (const real v)
	{
		return (*this) *= (1.0/v);
	}

	inline bool operator == (const Vector &v) const
	{
		return fabs(x-v.x)<EPS && fabs(y-v.y)<EPS && fabs(z-v.z)<EPS;
	}

	inline bool operator != (const Vector &v) const
	{
		return !((*this) == v);
	}

	inline real operator[](int p) const
	{
		return p==0?x:p==1?y:z;
	}

	static inline Vector zero()
	{
		return Vector(0,0,0);
	}

	static inline Vector eps()
	{
		return Vector(EPS,EPS,EPS);
	}

	static inline Vector inf()
	{
		return Vector(INF,INF,INF);
	}

	static inline Vector randVector()
	{
		return Vector(randReal(),randReal(),randReal());
	}

	real getMax() const
	{
		return std::max(x,std::max(y,z));
	}

	void updateMin(const Vector &v)
	{
		if (v.x<x) x=v.x;
		if (v.y<y) y=v.y;
		if (v.z<z) z=v.z;
	}

	void updateMax(const Vector &v)
	{
		if (v.x>x) x=v.x;
		if (v.y>y) y=v.y;
		if (v.z>z) z=v.z;
	}

	Vector reflection(const Vector &v) const
	{
		Vector vv=v.getNormalize();
		Vector res=(*this).getNormalize()*2*dot(vv)-vv;
		assert(fabs(res.mod()-1)<1e-10);
		return res;
	}

	Vector refraction(const Vector &v,const real &n) const
	{
		real cos1=-dot(v);
		real cos2=1.0-n*n*(1.0-cos1*cos1);
		if (cos2<1e-6) return zero();
		cos2=sqrt(cos2);
		Vector res=v*n-(*this)*(cos2-n*cos1);
//		assert(fabs(res.mod()-1)<1e-5);
		return res;
	}

	static inline Vector randInBall(const real &r)
	{
		real xita=randReal(2.0*M_PI);
		real fai=randReal(2.0*M_PI);
		return Vector(r*sin(xita)*cos(fai),r*sin(xita)*sin(fai),r*cos(xita));
	}
//	x=rsin¦Ècos¦Õ
//	y=rsin¦Èsin¦Õ
//	z=rcos¦È

	inline void print() const
	{
		qDebug()<<"x"<<x<<"y"<<y<<"z"<<z;
	}

	friend Vector noiseVector(Vector vec,real noise)
	{
		vec.normalize();
		Vector u,v;
		if (fabs(vec.x)>0.1) u=Vector(0,1,0);
		else u=Vector(1,0,0);
		u=u.cross(vec).getNormalize();
		v=vec.cross(u).getNormalize();
		real alpha=randReal(2.0*M_PI);
		real s=sin(alpha),c=cos(alpha);
		Vector res=vec*(1-noise*noise)+(u*s+v*c)*noise;
		return res.getNormalize();
	}
};

#define INSIDE 0
#define OUTSIDE 1

class Ray
{
public:
	Vector origin,direction;
	real distance;
	bool side;

	Ray()
	{
		origin=direction=Vector(0,0,0);
		distance=0;
		side=OUTSIDE;
	}

	Ray(const Vector &_origin,const Vector &_direction,const real &_distance,const bool &_side)
		:origin(_origin),direction(_direction.getNormalize()),distance(_distance),side(_side){}

	Vector getPoint(real t) const
	{
		return origin+direction*t;
	}
};

struct Face
{
	int p[3];
	Face(){}
	Face(int p0,int p1,int p2)
	{
		p[0]=p0,p[1]=p1,p[2]=p2;
	}
};

class Shape
{
public:
//	virtual Vector getPosition()
//	{
//		return Vector(0,0,0);
//	}

//	virtual real getRadius()
//	{
//		return 0;
//	}

	virtual Vector getNormalAt(const Vector &)=0;

	virtual real getIntersection(const Ray &)=0;

//	virtual bool testInside(const Vector &)=0;
// 	   \   in       n1
//    --------------------   n = n1 / n2
//		 \ out   	n2
	inline real getRefractiveIndex(const Ray &ray)
	{
		if (ray.side==INSIDE) return DEFAULT_REFRACTIVE_INDEX;
		return 1.0/DEFAULT_REFRACTIVE_INDEX;
	}
};

class Sphere:public Shape
{
private:
	Vector center;
	real radius;

public:
	Sphere(){}

	Vector getCenter()
	{
		return center;
	}

	real getRadius()
	{
		return radius;
	}

	Sphere(Vector _center,real _radius):center(_center),radius(_radius){}

	Vector getNormalAt(const Vector &vec)
	{
		assert(fabs((vec-center).mod()-radius)<1e-5);
		return (vec-center).getNormalize();
	}

	real getIntersection(const Ray &ray)
	{
		Vector AO=ray.origin-center;
		real a=ray.direction.sqr();
		real b=2*dot(ray.direction,AO);
		real c=AO.sqr()-radius*radius;
		real delta=b*b-4*a*c;
		if (delta<EPS) return -1;
		real x1=(-b-sqrt(delta))/(2*a);
		real x2=(-b+sqrt(delta))/(2*a);
		#define eps 1e-10
		if (x2<eps) return -1;
		if (x1>eps) return x1;
		#undef eps
		return x2;
	}

//	bool testInside(const Vector &vec)
//	{
//		return (vec-center).mod()<radius;
//	}

//	real getRefractiveIndex(const Ray &ray)
//	{
//		//the result is n1 / n2
////		if (testInside(point)) qDebug()<<"Inside";
////		else qDebug()<<"Outside";
//		if (ray.side==INSIDE) return DEFAULT_REFRACTIVE_INDEX;
//		return 1.0/DEFAULT_REFRACTIVE_INDEX;
//	}
};


class Plane:public Shape
{
friend class Mesh;
friend class MeshObject;

private:
	Vector norm; //(X, Y, Z) where n.x *X + n.y *Y + n.z *Z + distance = 0;
	real distance;

public:
	Plane(){}

	Plane(const Vector &_norm,const real &_distance)
		:norm(_norm.getNormalize()),distance(_distance){}

	Vector getNormalAt(const Vector &vec)
	{
		return norm;
	}

	real getIntersection(const Ray &ray)
	{
		real tmp=dot(norm,ray.direction);
		if (fabs(tmp)<EPS) return -1; // ray parallels the plane
		real t=-(distance+dot(norm,ray.origin))/dot(norm,ray.direction);
		if (t>0) return t;
		return -1;
	}

	void print()
	{
		qDebug()<<norm.x<<norm.y<<norm.z<<distance;
	}

//	bool testInside(const Vector &vec)
//	{
//		// :( nothing to do
//		return false;
//	}

//	real getRefractiveIndex(const Ray &ray)
//	{
//		if (ray.side==INSIDE) return DEFAULT_REFRACTIVE_INDEX;
//		return 1.0/DEFAULT_REFRACTIVE_INDEX;
//	}
};

class Mesh:public Shape
{
friend class AABB;
friend class MeshObject;

private:
	Vector p[3],n[3];
	Plane plane;
	real area; // 2*S(ABC)
	bool useSmooth;

public:
	Mesh(){}

	Mesh(const Vector &p0,const Vector &p1,const Vector &p2):useSmooth(false)
	{
		p[0]=p0,p[1]=p1,p[2]=p2;
		plane.norm=cross(p[0]-p[1],p[0]-p[2]);
		plane.norm.normalize();
		plane.distance=-p[0].dot(plane.norm);
		Vector tmp=cross(p[1]-p[0],p[2]-p[0]);
		area=tmp.mod();
	}

	void setSmoothNorm(const Vector &n0,const Vector &n1,const Vector &n2)
	{
		n[0]=n0;
		n[1]=n1;
		n[2]=n2;
	}

	void setUseSmooth()
	{
		useSmooth=true;
	}

	void print()
	{
		plane.norm.print();
		qDebug()<<plane.distance;
		qDebug()<<area;
	}

	Vector getNormalAt(const Vector &vec)
	{
		if (useSmooth)
		{
			real a=cross(vec-p[0],vec-p[1]).mod()/area;
			real b=cross(vec-p[0],vec-p[2]).mod()/area;
//			assert(a>-EPS&&a<1+EPS);
//			assert(b>-EPS&&b<1+EPS);
//			assert((1.0-a-b)>-EPS&&(1.0-a-b)<1+EPS);
			Vector res=n[0]*(1.0-a-b)+n[1]*b+n[2]*a;
			return res.getNormalize();
		}
		else return plane.norm;
	}

	inline real getMaxValue(int axis)
	{
		return std::max(p[0][axis],std::max(p[1][axis],p[2][axis]));
	}

	real getIntersection(const Ray &ray)
	{
		real res=plane.getIntersection(ray);
		if (res<0) return -1;
		Vector p0=ray.getPoint(res);
		real S=0;
		Vector tmp=cross(p0-p[0],p0-p[1]);
		S+=tmp.mod();
		tmp=cross(p0-p[0],p0-p[2]);
		S+=tmp.mod();
		tmp=cross(p0-p[1],p0-p[2]);
		S+=tmp.mod();
		if (fabs(S-area)<EPS) return res;
		return -1;
	}

//	bool testInside(const Vector &vec)
//	{
//		// :( nothing to do
//		return false;
//	}

//	real getRefractiveIndex(const Ray &ray)
//	{
//		if (ray.side==OUTSIDE) return DEFAULT_REFRACTIVE_INDEX;
//		return 1.0/DEFAULT_REFRACTIVE_INDEX;
//	}
};

#endif // GEOMERTY_H
