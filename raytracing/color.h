#ifndef COLOR_H
#define COLOR_H

#include "constant.h"
#include "geometry.h"

class Color;

class Color
{
public:
	real r,g,b;

	Color():r(EPS),g(EPS),b(EPS){}

	Color(real _r,real _g,real _b):r(_r),g(_g),b(_b){}

//	Color(const Color &_color)
//	{
//		r=_color.r;
//		g=_color.g;
//		b=_color.b;
//	}

	//Color(const Vector &v):r(v.x),g(v.y),b(v.z){}

	static Color red()
	{
		return Color(1,0,0);
	}

	real mod() const
	{
		return r*r+g*g+b*b;
	}

	Color operator + (const Color &v) const
	{
		return Color(r+v.r, g+v.g, b+v.b);
	}

	Color operator - () const
	{
		return Color(-r,-g,-b);
	}

	Color operator - (const Color &v) const
	{
		return *this + (-v);
	}

	Color& operator += (const Color &v)
	{
		r+=v.r, g+=v.g, b+=v.b;
		return *this;
	}

	Color operator * (const Color &v) const
	{
		return Color(r*v.r, g*v.g, b*v.b);
	}

	Color operator * (real t) const
	{
		return Color(r*t,g*t,b*t);
	}

	Color operator *= (real t)
	{
		r*=t, g*=t, b*=t;
		return *this;
	}

	Color operator /= (real t)
	{
		return (*this)*=(1.0/t);
	}

	Color operator / (real v) const
	{
		return *this * (1.0/v);
	}

	inline real getMax()
	{
		return std::max(r,std::max(g,b));
	}

	inline void normalize()
	{
		real tmpMax = getMax();
		if (tmpMax > 1)
		{
			r/=tmpMax;
			g/=tmpMax;
			b/=tmpMax;
		}
	}

	inline void print()
	{
		qDebug()<<r<<g<<b;
	}
};

#endif // COLOR_H
