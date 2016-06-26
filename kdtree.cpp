#include "kdtree.h"
#include <algorithm>
#include <vector>
#include <QTime>

using std::vector;
using std::pair;

KDTree::~KDTree()
{
	root->clear();
	delete root;
	for (int i=0;i<(int)numberMesh.size();i++) delete numberMesh[i];
	numberMesh.clear();
}

void KDTree::build(const vector<Mesh*>mesh)
{
	for (int i=0;i<(int)mesh.size();i++) numberMesh.push_back(new NumberMesh(mesh[i],i));
	root=build(numberMesh,1);
}

int KDTree::getIntersection(const Ray &ray)
{
	pair<real,int>res=std::make_pair(INF,-1);
	root->getIntersection(ray,res);
	return res.second;
}

//inline bool cmp0(NumberMesh *a,NumberMesh *b)
//{
//	return a->mesh->getMaxValue(0) < b->mesh->getMaxValue(0);
//}

//inline bool cmp1(NumberMesh *a,NumberMesh *b)
//{
//	return a->mesh->getMaxValue(1) < b->mesh->getMaxValue(1);
//}

//inline bool cmp2(NumberMesh *a,NumberMesh *b)
//{
//	return a->mesh->getMaxValue(2) < b->mesh->getMaxValue(2);
//}

KDTreeNode* KDTree::build(vector<NumberMesh*>_numberMesh,int depth)
{
//	qDebug()<<"!!!!!!!!!!!!!!"<<_numberMesh.size();
	int n=(int)_numberMesh.size();
	if (!n) return NULL;
	KDTreeNode *node=new KDTreeNode();
	node->numberMesh=_numberMesh;
	for (int i=0;i<n;i++) node->aabb.update(_numberMesh[i]->mesh);
	node->aabb.loose();
	if (n<LIMIT_KDTREE_SIZE||depth>MAX_KDTREE_DEPTH) //stop
	{
		node->isLeaf=true;
		return node;
	}

	Vector size=node->aabb.size();
	int axis=0;
	if (size[1]>size[0]) axis=1;
	if (size[2]>size[axis]) axis=2;
//	sort(_numberMesh.begin(),_numberMesh.end(),axis==0?cmp0:axis==1?cmp1:cmp2);
	static pair<real,int>tmp[1000000];
	for (int i=0;i<n;i++) tmp[i]=std::make_pair(_numberMesh[i]->mesh->getMaxValue(axis),i);
	sort(tmp,tmp+n);
	static vector<NumberMesh*>tmpVector(_numberMesh);
	tmpVector=_numberMesh;
	for (int i=0;i<n;i++) _numberMesh[i]=tmpVector[tmp[i].second];
//	int cutPosition=n/2;

	static AABB dp[2][300000];
	for (int i=0;i<n;i++)
	{
		if (i==0) dp[0][i]=AABB();
		else dp[0][i]=dp[0][i-1];
		dp[0][i].update(_numberMesh[i]->mesh);
	}
	for (int i=n-1;i>=0;i--)
	{
		if (i==n-1) dp[1][i]=AABB();
		else dp[1][i]=dp[1][i+1];
		dp[1][i].update(_numberMesh[i]->mesh);
	}

	int cutPosition=0;
	real minArea=INF;
	for (int i=1;i<n-1;i++)
	{
		real nowArea=dp[0][i].getSurfaceArea()*(i+1)+dp[1][i+1].getSurfaceArea()*(n-i-1);
		if (nowArea<minArea) minArea=nowArea,cutPosition=i+1;
	}
	vector<NumberMesh*>left(_numberMesh.begin(),_numberMesh.begin()+cutPosition);
	vector<NumberMesh*>right(_numberMesh.begin()+cutPosition,_numberMesh.end());
	node->child[0]=KDTree::build(left,depth+1);
	node->child[1]=KDTree::build(right,depth+1);
	return node;
}

void KDTreeNode::getIntersection(const Ray &ray,pair<real,int>&res) //res(distance,number)
{
	if (isLeaf)
	{
		for (int i=0;i<(int)numberMesh.size();i++)
		{
			Mesh *now=numberMesh[i]->mesh;
			real nowDistance=now->getIntersection(ray);
			if (nowDistance<0||nowDistance>=res.first) continue;
			res.first=nowDistance;
			res.second=numberMesh[i]->number;
		}
		return;
	}
	real nowDistance=aabb.getIntersection(ray);
	if (nowDistance<0||nowDistance>=res.first) return;
	if (child[0]) child[0]->getIntersection(ray,res);
	if (child[1]) child[1]->getIntersection(ray,res);
}

void KDTreeNode::clear()
{
	if (child[0])
	{
		child[0]->clear();
		delete child[0];
	}
	if (child[1])
	{
		child[1]->clear();
		delete child[1];
	}
}
