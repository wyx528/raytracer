#ifndef MATERIAL_H
#define MATERIAL_H

#include "geometry.h"
#include "color.h"
#include "texture.h"

class Material
{
private:
	Color color;
public:
	real shininess;
	real ambient,diffuse,specular;
	real transparency;
	Color emission; //only use in PATH TRACING, indicate this is a light
	real reflectionNoise;

	Material(){}

	Material(const Color &_color,real _shininess,
			 real _ambient,real _diffuse,real _specular,real _transparency,
			 const Color &_emission=Color(0,0,0),real _reflectionNoise=0)
		:color(_color),shininess(_shininess),
		  ambient(_ambient),diffuse(_diffuse),specular(_specular),transparency(_transparency),
		  emission(_emission),reflectionNoise(_reflectionNoise)
	{
		real tmp=diffuse+specular+transparency;
		diffuse/=tmp;
		specular/=tmp;
		transparency/=tmp;
	}

	virtual Color getColorAt(const Vector &p)
	{
		return color;
	}
};

class TextureMaterial:public Material
{
public:
	Texture texture;
	real scale;

	TextureMaterial(real _shininess,real _ambient,real _diffuse,real _specular,real _transparency,
					QString filename,real _scale)
	{
		shininess=_shininess;
		ambient=_ambient;
		diffuse=_diffuse;
		specular=_specular;
		transparency=_transparency;
		texture=Texture(filename);
		scale=_scale;
	}

	Color getColorAt(const Vector &p)
	{
//		real x=p[0]+10058,y=-p[1]+10087; //for dragon on the sea
//		real x=p[0]+10000,y=-p[1]+10022; //for snowtable
		real x=p[0],y=-p[1];
		x/=scale,y/=scale*texture.ratio;
		x-=floor(x),y-=floor(y);
		assert(x>-0.1&&y>-0.1&&x<1.1&&y<1.1);
		return texture.getColorAt(x,y);
	}
};

#endif // MATERIAL_H
