#pragma once
#include "RAABBTreeBuilder.h"
#include "RsSceneBuilder.h"

using namespace rs3;

struct rvertex
{
	MVector3 coord,normal;
	MVector3 color;	// alpha로 사용한다 
	float u,v;
	float u2,v2;	// lightmap 을 위한 uv

	void Save(FILE *file) {
		fwrite(&coord,sizeof(float),3,file);
		fwrite(&normal,sizeof(float),3,file);
		fwrite(&color,sizeof(float),3, file);
		fwrite(&u,sizeof(float),4,file);
	}

	bool operator ==(const rvertex &_v) { 
		return (coord==_v.coord && u==_v.u && v==_v.v && u2==_v.u2 && v2==_v.v2 && color==_v.color);
	}
};

struct rpolygon : public RAABBTreePolygon
{
	rpolygon() : v(NULL), pMaterial(NULL), dwFlags(0) { }
	~rpolygon() { SAFE_DELETE(v); }

	rpolygon(int n) : pMaterial(NULL), dwFlags(0), bColor(false)  { nCount=n; v=new rvertex[n]; }
	rpolygon(rpolygon *pp) { memcpy(this,pp,sizeof(rpolygon));v=new rvertex[nCount];memcpy(v,pp->v,sizeof(rvertex)*nCount); }

	void dump();

	int				nCount;
	rvertex*		v;
	RMaxMaterial*	pMaterial;
	int				nLightMapID;
	DWORD			dwFlags;		// RM_FLAG_COLLISION_MESH : 충돌 유무
	int				vi;				// vertex base index
	bool			bColor;

	int GetVertexCount() { return nCount; }
	const RVector& GetVertexPosition(int i) { return v[i].coord; }
};

typedef vector<rpolygon*>	RPolygonList;
typedef vector<rvertex*>	RVertexList;

class RRsNode : public RAABBTreeNode {
public:
	virtual ~RRsNode();

	int GetIndicesCount();

	bool Save(FILE *file);
};

class RsBuilder : public RAABBTreeBuilder
{
public:
	int			m_nMaterial;
	int			m_nLightMapID;
	RVertexList	m_vertices;

	void BuildVertex(RRsNode *pNode);

public:
	RsBuilder(void);
	virtual ~RsBuilder(void);

	void BuildVertex();
	bool Save(FILE *file);

	RAABBTreeNode* NewNode() { return new RRsNode; }
};
