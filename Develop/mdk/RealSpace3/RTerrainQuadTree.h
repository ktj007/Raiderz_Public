#pragma once

#include <vector>
#include "RTypes.h"
#include "RViewFrustum.h"
#include "RVType.h"
#include "RSceneNode.h"

namespace rs3 
{
class RTerrainPatch;

// 지형을 위한 QuadTree
class RTerrainQuadTree
{
private:
	enum 
	{
		LT = 0,		// left-top
		RT = 1,		// right-top
		RB = 2,		// left-bottom
		LB = 3,		// right-bottom
	};

	MRectF m_rect;
	int m_iDepth;
	bool m_bIsLeaf;

	RTerrainQuadTree *m_pParent;
	RTerrainQuadTree *m_pChild[4];

	rs3::RBoundingBox *m_pPatchBBox;
	rs3::RBoundingSphere *m_pPatchSphere;
	
	RVector3 m_vMinimum[4];
	RVector3 m_vMaximum[4];

	std::vector<RTerrainPatch*> m_arrPatches;

	bool SplitNode();
	void UpdateNode(RTerrainQuadTree *pNode);
	void RemoveSceneNode(rs3::RSceneNode *pSceneNode);
public:
	RTerrainQuadTree();
	virtual ~RTerrainQuadTree();
	
	int CreateNode(float left, float top, float right,float bottom);

	bool isLeaf()	{ return m_bIsLeaf; }
	int GetDepth()	{ return m_iDepth;  }

	RTerrainQuadTree* GetChild(int nIndex)		{	return m_pChild[nIndex]; }

	rs3::RBoundingBox* const GetPatchAABB()	{	return m_pPatchBBox; }
	rs3::RBoundingSphere* const GetPatchSphere()	{	return m_pPatchSphere; }

	std::vector<RTerrainPatch*>* GetPatches()
	{
		return &m_arrPatches;
	}

	float GetCenterX()	{		return (( m_rect.left + m_rect.right ) / 2.0f );	}
	float GetCenterY()	{		return (( m_rect.top + m_rect.bottom ) / 2.0f );	}

	void *m_pNodeIndex;

	int AddTerrainPatch(rs3::RTerrainPatch *pPatch);
	void Update();

	void DeleteNode();

	const RVector* GetMinimumQuad()		
	{	
		if (m_pPatchBBox==NULL)
			return NULL;

		return m_vMinimum; 
	}
	const RVector* GetMaximumQuad()		
	{	
		if (m_pPatchBBox==NULL)
			return NULL;

		return m_vMaximum; 
	}
};

}
