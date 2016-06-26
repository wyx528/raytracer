#include "phong.h"

Color Phong::rayTracing(Ray &ray,int depth)
{
	if (depth>MAX_TRACE_DEPTH) return COLOR_BLACK;
	Object *obj=getNearestObject(ray);

	if (obj==NULL) return COLOR_BLACK;

	IntersectionInfo info=obj->getIntersecionInfo(ray);

	//ambient
	Color nowAmbient=ambient*(info.material->getColorAt(info.point))*info.material->ambient;

	//diffuse + specular
	Color nowColor=getPhongColor(info,ray);

	//Beer-Lambert's Law
	ray.distance+=info.distance;
	nowColor*=pow(M_E,-ray.distance*AIR_BEER_DENSITY);

	//reflection
	Color nowReflection=getReflectionColor(info,ray,depth);

	//refraction
	Color nowRefraction=getRefractionColor(info,ray,depth);
	Color res=nowAmbient+nowColor+nowReflection+nowRefraction;

	res.normalize();
	return res;
}


Color Phong::getPhongColor(const IntersectionInfo &info, const Ray &ray)
{
	Color res=COLOR_BLACK;

	for (int i=0;i<(int)light.size();i++)
	{
		Light *lt=light[i];
		Vector directionToLight=lt->position-info.point; //without being normalized

		Object *block=getNearestObject(
					Ray(info.point+directionToLight*EPS,directionToLight,0,ray.side),directionToLight.mod());
		//move a little to the light

		if (block!=NULL) continue;
		Color now=COLOR_BLACK;

		directionToLight.normalize();

		//diffuse
		real nowDot=dot(directionToLight,info.norm); //attention!!!
		if (nowDot>EPS) now+=info.material->getColorAt(info.point)*lt->color*lt->intensity*nowDot*info.material->diffuse;

		//specular
		Vector directionToEye=(ray.origin-info.point).getNormalize();
		real half=info.norm.dot((directionToLight+directionToEye)/2);
		if (half>0) now+=lt->color*lt->intensity*info.material->specular*pow(half,info.material->shininess);

		//Beer-Lambert's Law
		real distanceToLight=(lt->position-info.point).mod();
		res+=now*pow(M_E,-distanceToLight*AIR_BEER_DENSITY);
	}
	return res;
}

Color Phong::getReflectionColor(const IntersectionInfo &info, const Ray &ray, int depth)
{
	Ray newRay;
	newRay.direction=-info.norm.reflection(ray.direction);
	newRay.origin=info.point+newRay.direction*EPS;
	newRay.distance=ray.distance;
	newRay.side=ray.side;
	Color res=rayTracing(newRay,depth+1);
	return res*info.material->specular;
//	real cosxita=fabs(newRay.direction.dot(info.norm));
//	return res*(REFL_DECAY*info.material->specular*info.material->shininess*cosxita);
//	return (refl+refl*info.material->diffuse*REFL_DIFFUSE_FACTOR)*
//			(REFL_DECAY*info.material->specular*info.material->shininess*cosxita);
}

Color Phong::getRefractionColor(const IntersectionInfo &info, const Ray &ray, int depth)
{
	if (info.material->transparency<EPS) return COLOR_BLACK;
	Vector dir=info.norm.refraction(ray.direction,info.refractiveIndex);
	if (dir.mod()<EPS) return COLOR_BLACK; //total reflection
//	assert(fabs(dir.mod()-1)<EPS);
	Ray newRay;
	newRay.direction=dir;
	newRay.origin=info.point+newRay.direction*EPS;
	newRay.distance=ray.distance;
	newRay.side=ray.side^1;
	Color res=rayTracing(newRay,depth+1);
	return res*info.material->transparency;
//	return (refra+refra*info.material->diffuse*TRANSM_DIFFUSE_FACTOR)*info.material->transparency;
}

void Phong::finish()
{
	for (int i=0;i<(int)light.size();i++)
	{
		Light *lt=light[i];
		ambient+=lt->color*lt->intensity;
	}
	ambient*=AMBIENT_FACTOR;
}

Color Phong::traceSimpleRay(Ray &ray)
{
	Color res=rayTracing(ray,0);
	res.normalize();
	return res;
}

void Phong::trace(int numThread,int totalThread)
{
	int width=view->width;
	int height=view->height;
	int done=0;
	for (int i=0;i<width;i++)
	{
		for (int j=0;j<height;j++)
		{
			if ((done++)%totalThread!=numThread) continue;
			Vector eye=view->eye,destination=view->getPoint(i,j);
			/*if ((i==0||i==width-1)&&(j==0||j==height-1)&&!i&&!j)
			{
				qDebug()<<i<<j;
				view->dw.print();
				view->dh.print();
				destination.print();
			}*/
			Ray ray;
			Color color;
			if (!useDepthOfField)
			{
				ray=Ray(eye,destination-eye,0,OUTSIDE);
				color=traceSimpleRay(ray);
			}
			else
			{
				color=COLOR_NONE;
				for (int k=0;k<DOF_SAMPLE_CNT;k++)
				{
					real rx = randReal(2), dx = rx < 1 ? sqrt(rx) - 1 : 1 - sqrt(2 - rx);
					real ry = randReal(2), dy = ry < 1 ? sqrt(ry) - 1 : 1 - sqrt(2 - ry);
					Vector focusPoint=eye+(view->getPoint(i+dx,j+dy)-eye).getNormalize()*CAMERA_FOCUS;
					destination=view->getPoint(i+dx,j+dy);
					real theta=randReal(M_PI*2);
					Vector move=view->dw.getNormalize()*cos(theta)+view->dh.getNormalize()*sin(theta);
					move.normalize();
					move*=sqrt(randReal())*APERTURE_RADIUS;
					Vector nowEye=eye+move;
					ray=Ray(nowEye,focusPoint-nowEye,0,OUTSIDE);
					color+=traceSimpleRay(ray);
				}
//				Vector nowEye=eye+(destination-eye)*DOF_SCREEN_DIST_FACTOR;
//				for (int i=0;i<DOF_SAMPLE_CNT;i++)
//				{
//					real theta=randReal(M_PI*2);
//					Vector move=view->dw.getNormalize()*cos(theta)+view->dh.getNormalize()*sin(theta);
//					move.normalize();
//					move*=randReal(DOF_SAMPLE_RADIUS);
//					Vector tmpEye=nowEye+move;
//					ray=Ray(tmpEye,destination-tmpEye,(eye-tmpEye).mod(),OUTSIDE);
//					color+=traceSimpleRay(ray);
//				}
				color/=DOF_SAMPLE_CNT;
			}
			imageColor[i][j]=color;
			isDonePoint[i][j]=true;
		}
//		if (numThread==0&&(i%(width/10)==(width/10-1))) qDebug()<<100.0*(i+1)/width<<"%    done!";
		if (numThread==0&&i%5==0)
		{
			while (queue.size())
			{
				PII point=queue.top();
				int x=point.first,y=point.second;
				if (!isDonePoint[x][y]) break;
				queue.pop();
				Color color=imageColor[x][y]*255;
				image->setPixel(x,y,qRgb(color.r,color.g,color.b));
			}
			QApplication::processEvents();
		}
	}
}
