#include "pathtracing.h"

PathTracing::PathTracing(){}

Color PathTracing::pathTracing(const Ray &ray,int depth)
{
	if (depth>MAX_TRACE_DEPTH) return COLOR_BLACK;

	Object* obj=getNearestObject(ray,true);
	if (obj==NULL) return COLOR_BLACK;

	IntersectionInfo info=obj->getIntersecionInfo(ray);

	Color color=info.material->getColorAt(info.point);
	Color emission=info.material->emission;

	real maxColorComponent=color.getMax();

	if (depth>MAX_TRACE_DEPTH/2||fabs(maxColorComponent)<EPS)
	{
		if (randReal()<maxColorComponent) color/=maxColorComponent;
		else return emission;
	}

	Color finalColor=emission;
	real diffuseRate=info.material->diffuse;
	real reflectionRate=info.material->specular;
	real refractionRate=info.material->transparency;
	real totalRate=diffuseRate+reflectionRate+refractionRate;

	if (randReal(totalRate)<diffuseRate)
	{
		finalColor+=color*getDiffuseColor(info,ray,depth)*diffuseRate;
	}
	else if (randReal(totalRate)<diffuseRate+reflectionRate)
	{
		finalColor+=color*getReflectionColor(info,ray,depth)*reflectionRate;
	}
	else
	{
		finalColor+=color*getRefractionColor(info,ray,depth)*refractionRate;
	}
	return finalColor;
}

Color PathTracing::getDiffuseColor(const IntersectionInfo &info,const Ray &ray,int depth)
{
	if (info.material->diffuse<EPS) return COLOR_BLACK;

	real r1=M_PI*2*randReal(),r2=randReal();

	Vector u,v;
	if (fabs(info.norm.x)>0.1) u=Vector(0,1,0);
	else u=Vector(1,0,0);
	u=u.cross(info.norm).getNormalize();
	v=info.norm.cross(u);

	Vector sampleDirection=(u*cos(r1)+v*sin(r1))*sqrt(r2)+info.norm*sqrt(1-r2);
	assert(fabs(sampleDirection.mod()-1)<1e-3);
	assert(sampleDirection.dot(info.norm)>-1e-3);

	Ray newRay=Ray(info.point+sampleDirection*EPS*100,sampleDirection,0,ray.side);
	return pathTracing(newRay,depth+1);
}

Color PathTracing::getReflectionColor(const IntersectionInfo &info,const Ray &ray,int depth)
{
	if (info.material->specular<EPS) return COLOR_BLACK;
	Ray newRay;
	newRay.direction=-info.norm.reflection(ray.direction);
//	newRay.distance=ray.distance;
	newRay.origin=info.point+newRay.direction*EPS*100;
	newRay.side=ray.side;
	return pathTracing(newRay,depth+1);
}

Color PathTracing::getRefractionColor(const IntersectionInfo &info,const Ray &ray,int depth)
{
	if (info.material->transparency<EPS) return COLOR_BLACK;
	Vector reflectionDirection=-info.norm.reflection(ray.direction);
	Ray reflectionRay=Ray(info.point+reflectionDirection*EPS*100,reflectionDirection,0,ray.side);

	Vector refractionDirection=info.norm.refraction(ray.direction,info.refractiveIndex);
	if (refractionDirection.mod()>0.1)
	{
		Ray refractionRay=Ray(info.point+refractionDirection*EPS*100,refractionDirection,0,ray.side^1);
//		real F0=(info.refractiveIndex-1)*(info.refractiveIndex+1);
//		F0*=F0;
//		real theta=ray.side==INSIDE?refractionDirection.dot(info.norm):ray.direction.dot(info.norm);
//		real Fr=F0+(1-F0)*pow(1.0+theta,5);
//		real P=0.25+Fr*0.5;
		return pathTracing(refractionRay,depth+randReal()<0.1);
//		if (randReal()<P) return pathTracing(reflectionRay,depth+1);
//		else return pathTracing(refractionRay,depth+randReal()<0.1);
	}
	else return pathTracing(reflectionRay,depth+1);
}

Color PathTracing::pathTracingEL(const Ray &ray,int depth,bool useEmission)
{
	if (depth>MAX_TRACE_DEPTH) return COLOR_BLACK;

	Object* obj=getNearestObject(ray,true);
	if (obj==NULL) return COLOR_BLACK;

	IntersectionInfo info=obj->getIntersecionInfo(ray);

	Color color=info.material->getColorAt(info.point);
	Color emission=info.material->emission*useEmission;

	real maxColorComponent=color.getMax();

	if (depth>MAX_TRACE_DEPTH/2||fabs(maxColorComponent)<EPS)
	{
		if (randReal()<maxColorComponent) color/=maxColorComponent;
		else return emission;
	}

	Color finalColor=emission;
	real diffuseRate=info.material->diffuse;
	real reflectionRate=info.material->specular;
	real refractionRate=info.material->transparency;
	real totalRate=diffuseRate+reflectionRate+refractionRate;

	if (randReal(totalRate)<diffuseRate)
	{
		real r1=randReal(M_PI*2),r2=randReal();

		Vector w=info.norm,u,v;
		assert(fabs(w.mod()-1)<1e-5);
		if (fabs(w.x)>0.1) u=Vector(0,1,0);
		else u=Vector(1,0,0);
		u=u.cross(w).getNormalize();
		v=w.cross(u).getNormalize();
		assert(fabs(w.mod()-1)<1e-5&&fabs(u.mod()-1)<1e-5&&fabs(v.mod()-1)<1e-5);
		Vector d=(u*cos(r1)+v*sin(r1))*sqrt(r2)+w*sqrt(1-r2);

//		d=noiseVector(w,sqrt(1-randReal()));

		Ray diffuseRay=Ray(info.point+d*EPS*100,d,0,ray.side);
		real diffuseDecay=w.dot(d);
		Color lightColor=COLOR_NONE;
/*
		for (int i=0;i<(int)sphereLight.size();i++)
		{
			Object* light=sphereLight[i];
			Sphere* sphere=dynamic_cast<Sphere*>(light->shape);
			real r=sphere->getRadius();
			Vector sw=sphere->getCenter()-info.point;
			real d2=sw.sqr();
			real cos_min=sqrt(1.0-r*r/d2);
			real cos_alpha=1-randReal(1-cos_min);
			real sin_alpha=sqrt(1-cos_alpha*cos_alpha);
			Vector l=noiseVector(sw,sin_alpha);
//			Vector su=((fabs(sw.x)>0.1?Vector(0,1,0):Vector(1,0,0))).cross(sw);
//			Vector sv=sw.cross(su);
//			real cos_a_max=sqrt(1-r*r/sw.dot(sw));
//			real eps1=randReal(),eps2=randReal();
//			real cos_a = 1-eps1+eps1*cos_a_max;
//			real sin_a = sqrt(1-cos_a*cos_a);
//			real phi = 2.0*M_PI*eps2;
//			sw.normalize(),su.normalize(),sv.normalize();
//			Vector l = su*cos(phi)*sin_a + sv*sin(phi)*sin_a + sw*cos_a;
//			l.normalize();
			if (getNearestObject(Ray(info.point,l,0,ray.side),true)==light)
			{
				real omega = 2*(1-cos_min);
				lightColor+=light->material->emission*l.dot(w)*omega;
			}
		}
*/
		Color diffuseColor=pathTracingEL(diffuseRay,depth+1,true)*diffuseDecay;
		finalColor+=color*(lightColor+diffuseColor);
		return finalColor;
	}
	else if (randReal(totalRate)<diffuseRate+reflectionRate)
	{
		Ray newRay;
		newRay.direction=-info.norm.reflection(ray.direction);
		newRay.direction=noiseVector(newRay.direction,info.material->reflectionNoise);
		newRay.origin=info.point+newRay.direction*EPS*100;
		newRay.side=ray.side;
		finalColor+=color*pathTracingEL(newRay,depth+1);
		return finalColor;
	}
	else
	{
		Vector reflectionDirection=-info.norm.reflection(ray.direction);
		Vector refractionDirection=info.norm.refraction(ray.direction,info.refractiveIndex);
		Ray refractionRay=Ray(info.point+refractionDirection*EPS*100,refractionDirection,0,ray.side^1);
		Ray reflectionRay=Ray(info.point+reflectionDirection*EPS*100,reflectionDirection,0,ray.side);
		if (refractionDirection.mod()>0.1)
		{
//			real n=info.refractiveIndex; // n1/n2
//			real cosi=dot(info.norm,reflectionDirection);
//			real sini=sqrt(1-cosi*cosi);
//			real x=sqrt(1-n*n*sini*sini);
//			real rs=pow((n*cosi-x)/(n*cosi+x),2);
//			real rp=pow((n*x-cosi)/(n*x+cosi),2);
//			real r=(rs+rp)/2;
//			if (randReal()<r) finalColor+=color*pathTracingEL(reflectionRay,depth+1);
//			else finalColor+=color*pathTracingEL(refractionRay,depth+1);
			real R0=(info.refractiveIndex-1)*(info.refractiveIndex+1);
			R0*=R0;
			real coef=1+(ray.side==INSIDE?dot(info.norm,reflectionDirection):dot(info.norm,refractionDirection));
			real Re=R0+(1-R0)*pow(1.0-coef,5);
			if (randReal()<Re/6&&ray.side==OUTSIDE) finalColor+=color*pathTracingEL(reflectionRay,depth+1);
			else finalColor+=color*pathTracingEL(refractionRay,depth+1);
//			finalColor+=color*pathTracingEL(refractionRay,depth+1);
		}
		else finalColor+=color*pathTracingEL(reflectionRay,depth+1)*0; //total reflection
		return finalColor;
	}
}

Color PathTracing::multiSampleTrace(int x, int y)
{
	Color color=COLOR_NONE;
	if (!useDepthOfField)
	{
		for (int i=0;i<PATH_TRACING_SAMPLE_CNT;i++)
		{
			real rx = randReal(2), dx = rx < 1 ? sqrt(rx) - 1 : 1 - sqrt(2 - rx);
			real ry = randReal(2), dy = ry < 1 ? sqrt(ry) - 1 : 1 - sqrt(2 - ry);
			Vector eye=view->eye,destination=view->getPoint(dx+x,dy+y);
			Ray ray=Ray(eye,destination-eye,0,OUTSIDE);
			color+=pathTracingEL(ray,0);
		}
		color/=PATH_TRACING_SAMPLE_CNT;
		color.normalize();
	}
	else
	{
		Vector eye=view->eye;
		for (int i=0;i<PATH_TRACING_SAMPLE_CNT;i++)
		{
			real rx = randReal(2), dx = rx < 1 ? sqrt(rx) - 1 : 1 - sqrt(2 - rx);
			real ry = randReal(2), dy = ry < 1 ? sqrt(ry) - 1 : 1 - sqrt(2 - ry);
			Vector destination=view->getPoint(x+dx,y+dy);
			Vector focusPoint=eye+(destination-eye).getNormalize()*CAMERA_FOCUS;
			real theta=randReal(M_PI*2);
			Vector move=view->dw.getNormalize()*cos(theta)+view->dh.getNormalize()*sin(theta);
			move.normalize();
			move*=sqrt(randReal())*APERTURE_RADIUS;
			Vector nowEye=eye+move;
			Ray ray=Ray(nowEye,focusPoint-nowEye,0,OUTSIDE);
			color+=pathTracingEL(ray,0);
		}
		color/=PATH_TRACING_SAMPLE_CNT;
		color.normalize();
	}
	return color;
}

void PathTracing::trace(int numThread, int totalThread)
{
	int width=view->width;
	int height=view->height;
	for (int i=0;i<width;i++)
	{
		for (int j=0;j<height;j++)
		{
			if (belong[i][j]!=numThread) continue;
			Color color=multiSampleTrace(i,j);
			imageColor[i][j]=color;
			isDonePoint[i][j]=true;
		}
//		if (numThread==0&&(i%(width/10)==(width/10-1))) qDebug()<<100.0*(i+1)/width<<"%    done!";
		if (numThread==0)
		{
			while (queue.size())
			{
				PII point=queue.top();
				int x=point.first,y=point.second;
				if (!isDonePoint[x][y]) break;
				queue.pop();
				Color color=imageColor[x][y];
				color*=255;
				image->setPixel(x,y,qRgb(color.r,color.g,color.b));
			}
			QApplication::processEvents();
			if (i%10==0) qDebug()<<i+1<<width<<100.0*(i+1)/width<<"%";
		}
	}
}
