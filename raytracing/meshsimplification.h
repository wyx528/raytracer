#ifndef MESHSIMPLIFICATION_H
#define MESHSIMPLIFICATION_H

#include "constant.h"
#include "geometry.h"
#include <set>
#include <cstdlib>
#include <utility>
#include <vector>

using std::pair;

class Matrix4X4
{
public:
	real value[4][4];

	Matrix4X4();

	friend Matrix4X4 operator + (const Matrix4X4& a,const Matrix4X4& b);

	bool getInverseMatrix4X4(Matrix4X4 &res);

};

struct edgeNode
{
	int x,y;
	real cost;
	Vector newPoint;

	edgeNode(int _x,int _y,real _cost,Vector _newPoint):x(_x),y(_y),cost(_cost),newPoint(_newPoint){}

	friend bool operator < (const edgeNode& a,const edgeNode& b)
	{
		if (a.cost!=b.cost) return a.cost<b.cost;
		return std::make_pair(a.x,a.y)<std::make_pair(b.x,b.y);
	}
};

class MeshSimplification
{
private:
	std::vector<Vector>point;
	std::vector<Face>face;
	real rate;

	real getCost(const Matrix4X4& m,Vector &v);

	pair<real,Vector> getNewPoint(Vector &a,Vector &b,const Matrix4X4& A,const Matrix4X4& B);

public:
	MeshSimplification(){}

	MeshSimplification(std::vector<Vector>_point,std::vector<Face>_face,real _rate):point(_point),face(_face),rate(_rate){}

	void simplify();

	std::pair< std::vector<Vector>,std::vector<Face> > getResult();
};

#endif // MESHSIMPLIFICATION_H
