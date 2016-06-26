#ifndef PATHTRACING_H
#define PATHTRACING_H

#include "render.h"

class PathTracing:public Render
{
private:
	Color getDiffuseColor(const IntersectionInfo &info,const Ray &ray,int depth);

	Color getReflectionColor(const IntersectionInfo &info,const Ray &ray,int depth);

	Color getRefractionColor(const IntersectionInfo &info,const Ray &ray,int depth);

	Color pathTracing(const Ray &ray,int depth);

	Color pathTracingEL(const Ray &ray,int depth,bool useEmission=true);

	Color multiSampleTrace(int x,int y);

public:
    PathTracing();

	void trace(int numThread,int totalThread);
};

#endif // PATHTRACING_H
