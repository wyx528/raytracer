#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include "geometry.h"
#include "material.h"
#include "object.h"
#include "kdtree.h"
#include <vector>
#include <set>

typedef pair<int,int> PII;

class MeshObject
{
private:
	Vector center;
	real size;
	Material *material;
	QString filename;
	AABB bound;
	KDTree tree;
	bool useSmooth;

	vector<Vector>point;
	vector<Face>face;
	vector<Mesh*>mesh;
	vector<Object*>object;

	static Vector smoothNorm[300000];

	void read();

	void adjust();

	void simplify(real rate);

	void finish();

	Mesh* getMesh(int num);

public:
	MeshObject(){}

	~MeshObject();

	MeshObject(const Vector &_center,real _size,Material *_material,const QString &_filename,real _useSmooth=true,real rate=1);

	Object* getObject(int num);

	Object* getIntersection(const Ray &ray);
};

#endif // MESHOBJECT_H
