#ifndef _RAABBTREEBUILDER_H
#define _RAABBTREEBUILDER_H

#include "RLib.h"
#include <vector>

using namespace std;

namespace rs3 {

class RAABBTreePolygon
{
public:
	MVector3 m_center;	// center

	virtual ~RAABBTreePolygon() {}
	virtual int GetVertexCount() = 0;
	virtual const MVector3& GetVertexPosition(int i) = 0;
};

const int DEFAULT_AABB_POLYGON_COUNT_BOUND = 20;

class RAABBTreeBuilder;

class RAABBTreeNode
{
	friend RAABBTreeBuilder;

public:
	MBox	m_bb;		// bounding box
	RAABBTreeNode* m_pChildren[2];
	vector<RAABBTreePolygon*> m_polygons;

	virtual ~RAABBTreeNode() {}
	int GetNodeCount();
	int GetPolygonCount();
	int GetVertexCount();
	bool IsLeafNode() { return (m_pChildren[0]==NULL); }
};

class RAABBTreeBuilder
{
public:
	RAABBTreeNode*	m_pRootNode;
	unsigned int	m_nPolygonPerNodes;
	bool			m_bBuilt;

	void Build(RAABBTreeNode* pNode);
public:
	RAABBTreeBuilder();
	virtual ~RAABBTreeBuilder();

	void BeginBuild();
	void AddPolygon(RAABBTreePolygon* pPolygon);
	void EndBuild(int nPolygonPerNodes = DEFAULT_AABB_POLYGON_COUNT_BOUND);

	virtual RAABBTreeNode* NewNode() = 0;
};

}

#endif