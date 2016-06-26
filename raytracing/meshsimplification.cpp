#include "meshsimplification.h"

#include <set>
#include <map>
#include <algorithm>

Matrix4X4::Matrix4X4()
{
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++) value[i][j]=0;
}

Matrix4X4 operator +(const Matrix4X4 &a, const Matrix4X4 &b)
{
	Matrix4X4 res;
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++) res.value[i][j]=a.value[i][j]+b.value[i][j];
	return res;
}

bool Matrix4X4::getInverseMatrix4X4(Matrix4X4 &res)
{
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++) res.value[i][j]=0;
	for (int i=0;i<4;i++) res.value[i][i]=1;
	for (int i=0;i<4;i++)
	{
		int w=i;
		for (int j=i+1;j<4;j++)
			if (fabs(value[j][i])>fabs(value[w][i])) w=j;
		if (fabs(value[w][i])<1e-5) return false;
		for (int j=0;j<4;j++)
		{
			std::swap(value[i][j],value[w][j]);
			std::swap(res.value[i][j],res.value[w][j]);
		}
		real d=value[i][i];
		for (int j=0;j<4;j++) value[i][j]/=d,res.value[i][j]/=d;
		for (int j=i+1;j<4;j++)
		{
			d=value[j][i];
			for (int k=0;k<4;k++) value[j][k]-=d*value[i][k],res.value[j][k]-=d*res.value[i][k];
		}
	}
	for (int i=3;i>=0;i--)
		for (int j=i+1;j<4;j++)
		{
			real d=value[i][j];
			for (int k=0;k<4;k++) value[i][k]-=d*value[j][k],res.value[i][k]-=d*res.value[j][k];
		}
	return true;
}

real MeshSimplification::getCost(const Matrix4X4& m, Vector &v)
{
	real a[4]={v.x,v.y,v.z,1};
	real b[4]={0};
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++) b[i]+=a[j]*m.value[j][i];
	real res=0;
	for (int i=0;i<4;i++) res+=a[i]*b[i];
	return res;
}

pair<real,Vector> MeshSimplification::getNewPoint(Vector &a,Vector &b,const Matrix4X4& A,const Matrix4X4& B)
{
	Matrix4X4 m=A+B,n=m;
	m.value[3][0]=m.value[3][1]=m.value[3][2]=0;
	m.value[3][3]=1;
	Matrix4X4 inv;
	real cost;
	Vector newPoint;
	bool checkInv=m.getInverseMatrix4X4(inv);
	if (!checkInv)
	{
		newPoint=a,cost=getCost(n,newPoint);
		for (int i=1;i<=6;i++)
		{
			Vector tmpPoint=a+(b-a)/6*i;
			real tmpCost=getCost(n,tmpPoint);
			if (tmpCost<cost) cost=tmpCost,newPoint=tmpPoint;
		}
	}
	else
	{
		newPoint=Vector(inv.value[0][3],inv.value[1][3],inv.value[2][3]);
		cost=getCost(n,newPoint);
	}
	return std::make_pair(cost,newPoint);
}

void MeshSimplification::simplify()
{
	if (fabs(rate-1)<EPS) return;
	int nV=point.size(),nF=face.size();
	int nowV=nV,totalV=nV;
	std::map<int,int>mapping;
	std::map<int,std::set<int> >connect;
	std::map<int,bool>del;
	std::set<edgeNode>edge;
	std::vector<Matrix4X4>matrix(nV);
	for (int i=0;i<nV;i++) mapping[i]=i;
	for (int i=0;i<nF;i++)
	{
		int a=face[i].p[0],b=face[i].p[1],c=face[i].p[2];
		Vector norm=cross(point[b]-point[a],point[c]-point[a]);
		norm.normalize();
		real value[4]={norm.x,norm.y,norm.z,-norm.dot(point[a])};
		for (int j=0;j<3;j++)
		{
			int x=face[i].p[j],y=face[i].p[(j+1)%3];
			connect[x].insert(y);
			connect[y].insert(x);
			int X=x,Y=y;
			if (X>Y) std::swap(X,Y);
			for (int ii=0;ii<4;ii++)
				for (int jj=0;jj<4;jj++) matrix[x].value[ii][jj]+=value[ii]*value[jj];
		}
	}
	for (int i=0;i<nF;i++)
		for (int j=0;j<3;j++)
		{
			int X=face[i].p[j],Y=face[i].p[(j+1)%3];
			if (X>Y) std::swap(X,Y);
			pair<real,Vector>info=getNewPoint(point[X],point[Y],matrix[X],matrix[Y]);
//			edge.insert(edgeNode(X,Y,(point[X]-point[Y]).dot(point[X]-point[Y]),(point[X]+point[Y])/2));
			edge.insert(edgeNode(X,Y,info.first,info.second));
		}
	while (nowV>rate*nV)
	{
		edgeNode nowEdge=*edge.begin();
		edge.erase(edge.begin());
		int x=nowEdge.x,y=nowEdge.y;
		while (del[x]||del[y])
		{
			nowEdge=*edge.begin();
			edge.erase(edge.begin());
			x=nowEdge.x,y=nowEdge.y;
		}
		del[x]=del[y]=true;
		nowV--;
		point.push_back(nowEdge.newPoint);
		mapping[x]=mapping[y]=totalV;
		mapping[totalV]=totalV;
		matrix.push_back(matrix[x]+matrix[y]);
		assert(x<totalV&&y<totalV);
		for (std::set<int>::iterator i=connect[x].begin();i!=connect[x].end();i++)
		{
			int ty=*i;
			if (ty==y) continue;
			int X=totalV,Y=ty;
			if (X>Y) std::swap(X,Y);
			pair<real,Vector>info=getNewPoint(point[X],point[Y],matrix[X],matrix[Y]);
			edge.insert(edgeNode(X,Y,info.first,info.second));
//			edge.insert(edgeNode(X,Y,(point[X]-point[Y]).dot(point[X]-point[Y]),(point[X]+point[Y])/2));
		}
		for (std::set<int>::iterator i=connect[y].begin();i!=connect[y].end();i++)
		{
			int tx=*i;
			assert(tx<totalV);
			if (tx==x) continue;
			int X=totalV,Y=tx;
			if (X>Y) std::swap(X,Y);
			pair<real,Vector>info=getNewPoint(point[X],point[Y],matrix[X],matrix[Y]);
			edge.insert(edgeNode(X,Y,info.first,info.second));
//			edge.insert(edgeNode(X,Y,(point[X]-point[Y]).dot(point[X]-point[Y]),(point[X]+point[Y])/2));
		}
		for (std::set<int>::iterator i=connect[x].begin();i!=connect[x].end();i++)
		{
			int ty=*i;
			assert(ty<totalV);
			if (ty==y) continue;
			connect[ty].erase(x);
			connect[ty].insert(totalV);
			connect[totalV].insert(ty);
		}
		for (std::set<int>::iterator i=connect[y].begin();i!=connect[y].end();i++)
		{
			int tx=*i;
			if (tx==x) continue;
			connect[tx].erase(y);
			connect[tx].insert(totalV);
			connect[totalV].insert(tx);
		}
		totalV++;
	}
	std::vector<Vector>newPoint;
	std::vector<Face>newFace;
	std::vector<bool>used(totalV);
	std::vector<int>number(totalV);
	for (int i=totalV-1;i>=0;i--) mapping[i]=mapping[mapping[i]];
	for (int i=0;i<nF;i++)
	{
		int x=face[i].p[0],y=face[i].p[1],z=face[i].p[2];
		x=mapping[x],y=mapping[y],z=mapping[z];
		if (x==y||x==z||y==z) continue;
		used[x]=used[y]=used[z]=true;
	}
	int currentUsed=0;
	for (int i=0;i<totalV;i++)
		if (used[i]) number[i]=currentUsed++,newPoint.push_back(point[i]);
	for (int i=0;i<nF;i++)
	{
		int x=face[i].p[0],y=face[i].p[1],z=face[i].p[2];
		x=mapping[x],y=mapping[y],z=mapping[z];
		if (x==y||x==z||y==z) continue;
		x=number[x],y=number[y],z=number[z];
		newFace.push_back(Face(x,y,z));
	}
	point=newPoint;
	face=newFace;
}

std::pair<std::vector<Vector>, std::vector<Face> > MeshSimplification::getResult()
{
	return make_pair(point,face);
}

