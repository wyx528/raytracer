#ifndef LIGHT_H
#define LIGHT_H

#include "geometry.h"
#include "color.h"
#include "material.h"
#include "global.h"

class Light
{
/*
private:
	Vector position;
	Color color;

public:
	Light(const Vector &_position,const Color &_color)
		:position(_position),color(_color){}

	Vector getPosition()
	{
		return position;
	}

	Vector getColor()
	{
		return color;
	}
*/
public:
	Vector position;
	Color color;
	real intensity;

	Light(const Light &_light)
	{
		*this=_light;
	}

	Light(const Vector &_position,const Color &_color,const real &_intensity)
		:position(_position),color(_color),intensity(_intensity){}
//	Light(const Vector &_position,const Color &_ambient,const Color &_diffuse,const Color &_specular)
//		:position(_position),ambient(_ambient),diffuse(_diffuse),specular(_specular){}

//	Color evalAmbient(Material *material)
//	{
//		return ambient*material->ambient*material->diffuse;
//	}

//	Color evalDiffuse(Material *material,const Vector &norm,Vector hitDirection)
//	//norm must be normalize hitDirection: from object to light
//	{
//		hitDirection.normalize();
//		Color res=diffuse*material->diffuse;
//		real d=std::max(norm.dot(hitDirection),0.0);
//		return res*d;
//	}

//	Color evalSpecular(Material *material,const Vector &norm, //norm must be normalize
//						Vector dirToLight,Vector dirToEye,const real &materialShininess)
//	{
//		dirToLight.normalize();
//		dirToEye.normalize();
//		Color res=COLOR_WHITE*specular*material->specular;
//		real tmp=norm.dot((dirToLight+dirToEye)/2);
//		if (tmp<EPS) tmp=0;
//		res*=pow(tmp,materialShininess);
//		return res;
//	}
};

#endif // LIGHT_H
