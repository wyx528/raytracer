#ifndef PHONG_H
#define PHONG_H

#include "render.h"

class Phong : public Render
{
private:
	Color ambient;

	Color getPhongColor(const IntersectionInfo &info,const Ray &ray);//diffuse + specular

	Color getReflectionColor(const IntersectionInfo &info,const Ray &ray,int depth);

	Color getRefractionColor(const IntersectionInfo &info,const Ray &ray,int depth);

	Color rayTracing(Ray &ray,int depth);

	Color traceSimpleRay(Ray &ray);

public:

	Phong(){}

	void finish();

	void trace(int numThread,int totalThread);
};

#endif // PHONG_H
