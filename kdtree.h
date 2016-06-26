#ifndef KDTREE_H
#define KDTREE_H

#include "aabb.h"
#include <vector>

using std::vector;
using std::pair;

class NumberMesh //:(
{
public:
	Mesh *mesh;
	int number;

	NumberMesh(){}

	NumberMesh(Mesh *_mesh,int _number):mesh(_mesh),number(_number){}
};

class KDTreeNode
{
public:
	vector<NumberMesh*>numberMesh;
	KDTreeNode *child[2];
	AABB aabb;
	bool isLeaf;

	KDTreeNode():isLeaf(false)
	{
		aabb.init();
		child[0]=child[1]=NULL;
		numberMesh.clear();
	}

	void clear();

	void getIntersection(const Ray &ray,pair<real,int>&res);
};

class KDTree
{
private:
	KDTreeNode *root;
	vector<NumberMesh*>numberMesh;

	KDTreeNode *build(vector<NumberMesh*>_numberMesh,int depth);

public:
	KDTree(){}

	~KDTree();

	void build(const vector<Mesh*>mesh);

	int getIntersection(const Ray &ray); //return the number of mesh
};

#endif // KDTREE_H
