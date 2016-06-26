#ifndef OBJECT_H
#define OBJECT_H

#include "geometry.h"
#include "material.h"

class IntersectionInfo
{
public:
	real distance;
	Vector point,norm;
	Material *material;
	real refractiveIndex;

	IntersectionInfo(){}

	IntersectionInfo(const real &_distance,const Vector &_point,const Vector &_norm,Material *_material,const real &_refractiveIndex)
		:distance(_distance),point(_point),norm(_norm),material(_material),refractiveIndex(_refractiveIndex){}

	static IntersectionInfo miss()
	{
		return IntersectionInfo(-1,Vector(0,0,0),Vector(0,0,0),NULL,0);
	}
//	IntersectionInfo(const ShapeIntersectionInfo &info,Material *_material):
//		material(_material)
//	{
//		distance=info.distance;
//		point=info.point;
//		norm=info.norm;
//	}
};

class Object
{
public:
	Shape *shape;
	Material *material;

	Object(Shape *_shape,Material *_material)
		:shape(_shape),material(_material){}

	bool isLight()
	{
		return (material->emission).mod()>0.1;
	}

	real getIntersectionDistance(const Ray &ray)
	{
		return shape->getIntersection(ray);
	}

	IntersectionInfo getIntersecionInfo(const Ray &ray)
	{
		real distance=shape->getIntersection(ray);

		if (distance<0) return IntersectionInfo::miss();

		Vector point=ray.getPoint(distance);
		Vector norm=shape->getNormalAt(point);
		if (norm.dot(ray.direction)>0) norm*=-1;

		real refractiveIndex=shape->getRefractiveIndex(ray);
//		if (refractiveIndex>EPS) qDebug()<<refractiveIndex;
		return IntersectionInfo(distance,point,norm,material,refractiveIndex);
	}
};

#endif // OBJECT_H
