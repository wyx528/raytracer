#ifndef CONSTANT_H
#define CONSTANT_H

typedef double real;

const real EPS=1e-8;
const real INF=1e50;
const real M_E=2.718281828459045;
const real M_PI=3.14159265358979;

const int MAX_KDTREE_DEPTH=18;
const int LIMIT_KDTREE_SIZE=18;

const int TOTAL_THREAD=6;
//const int MAX_TRACE_DEPTH=3;
//const int SOFT_SHADOW_LIGHT=50;
//const real SOFT_SHADOW_LIGHT_RADIUS=5;

const real AIR_BEER_DENSITY=0.02;
const real AMBIENT_FACTOR=0.01;

const real DEFAULT_REFRACTIVE_INDEX=1.2;

//const int DOF_SAMPLE_CNT=50;
//const real DOF_SAMPLE_RADIUS=0.6;
//const real DOF_SCREEN_DIST_FACTOR=0.6;

inline real randReal(real d=1.0)//[0,d)
{
//	return d*qrand()/32768;
	static unsigned long long seed=1;
	static unsigned long long a=273673163155LL;
	static unsigned long long c=138;
	static unsigned long long m=1LL<<48;
	seed=(a*seed+c)%m;
	return d*seed/m;
}

#endif // CONSTANT_H
