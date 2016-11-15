
#include "stdafx.h"
#include <assert.h>
#include <math.h>
#include "RTerrainQuadTree.h"
#include "RTerrainPatch.h"
#include <queue>

using namespace std;

namespace rs3 {

RTerrainQuadTree::RTerrainQuadTree()
{
	m_pParent = NULL;
	m_pChild[0] = NULL;
	m_pChild[1] = NULL;
	m_pChild[2] = NULL;
	m_pChild[3] = NULL;

	m_iDepth = 0;
	m_bIsLeaf = true;

	m_pPatchBBox = NULL;
	m_pPatchSphere = NULL;

	m_rect.left = 0.0f;
	m_rect.right = 0.0f;
	m_rect.top = 0.0f;
	m_rect.bottom = 0.0f;

	m_pNodeIndex = NULL;
}

RTerrainQuadTree::~RTerrainQuadTree()
{
	DeleteNode();
}

bool RTerrainQuadTree::SplitNode()
{
	/*
	*	0---1 : Clock Wise
	*   |   |
	*   3---2
	*/
	if ( !m_bIsLeaf )
		return true;

	int nextDepth = m_iDepth + 1;

	float hcenter = ( m_rect.left + m_rect.right ) / 2.0f;	// X - Center
	float vcenter = ( m_rect.top + m_rect.bottom ) / 2.0f;	// Y - Center

	m_pChild[LT] = new RTerrainQuadTree();
	m_pChild[LT]->m_pParent = this;
	m_pChild[LT]->CreateNode(m_rect.left, m_rect.top, hcenter, vcenter);
	m_pChild[LT]->m_iDepth = nextDepth;
	m_pChild[LT]->m_bIsLeaf = true;

	m_pChild[RT] = new RTerrainQuadTree();
	m_pChild[RT]->m_pParent = this;
	m_pChild[RT]->CreateNode(hcenter, m_rect.top, m_rect.right, vcenter);
	m_pChild[RT]->m_iDepth = nextDepth;
	m_pChild[RT]->m_bIsLeaf = true;

	m_pChild[RB] = new RTerrainQuadTree();
	m_pChild[RB]->m_pParent = this;
	m_pChild[RB]->CreateNode(hcenter, vcenter, m_rect.right, m_rect.bottom);
	m_pChild[RB]->m_iDepth = nextDepth;
	m_pChild[RB]->m_bIsLeaf = true;

	m_pChild[LB] = new RTerrainQuadTree();
	m_pChild[LB]->m_pParent = this;
	m_pChild[LB]->CreateNode(m_rect.left, vcenter, hcenter, m_rect.bottom);
	m_pChild[LB]->m_iDepth = nextDepth;
	m_pChild[LB]->m_bIsLeaf = true;

	m_bIsLeaf = false;

	return true;
}

/**
int RTerrainQuadTree::AddTerrainObject(RSceneNode *pSceneNode)
{
	const RBoundingBox &BBox = pSceneNode->GetAABB();
	if ( m_pObjectBBox )
	{
		m_pObjectBBox->Add(BBox);
	}
	else
	{
		m_pObjectBBox = new RBoundingBox();
		*m_pObjectBBox = BBox;
	}

	if ( m_bIsLeaf )
	{
		m_listObjectIndexs.push_back(pSceneNode);
		return m_iDepth;
	}

	// Patch 목록에 추가
	float cx = (m_rect.right + m_rect.left)/2.0f;
	float cy = (m_rect.bottom + m_rect.top)/2.0f;
	float bx = (BBox.minx + BBox.maxx) /2.0f;
	float by = (BBox.miny + BBox.maxy) /2.0f;

	float dx = bx - cx;
	float dy = by - cy;

	if ( dx < 0.0f )
	{
		if ( dy < 0.0f )
		{
			// (-,-) 
			return m_pChild[LT]->AddTerrainObject(pSceneNode);
		}
		else
		{
			// (-,+)
			return m_pChild[LB]->AddTerrainObject(pSceneNode);
		}
	}
	else
	{
		if ( dy < 0.0f )
		{
			// (+,-)
			return m_pChild[RT]->AddTerrainObject(pSceneNode);
		}
		else
		{
			// (+,+)
			return m_pChild[RB]->AddTerrainObject(pSceneNode);
		}
	}
}
*/

int RTerrainQuadTree::AddTerrainPatch(rs3::RTerrainPatch *pPatch)
{
	// Bounding Box Update
	RBoundingBox * const pBBox = pPatch->GetAABBBoundBox();
	if ( m_pPatchBBox )
	{
		m_pPatchBBox->Add(*pBBox);
		if ( m_pPatchSphere == NULL )
			m_pPatchSphere = new RBoundingSphere();

		m_pPatchSphere->center = m_pPatchBBox->GetCenter();
		m_pPatchSphere->radius = (m_pPatchSphere->center - m_pPatchBBox->vmin).Length();
	}
	else
	{
		m_pPatchBBox = new RBoundingBox;
		*m_pPatchBBox = *pBBox;

		if ( m_pPatchSphere == NULL )
			m_pPatchSphere = new RBoundingSphere();

		m_pPatchSphere->center = m_pPatchBBox->GetCenter();
		m_pPatchSphere->radius = (m_pPatchSphere->center - m_pPatchBBox->vmin).Length();
	} 

	if ( m_pPatchBBox )
	{
		const RVector &kMin = m_pPatchBBox->vmin;
		const RVector &kMax = m_pPatchBBox->vmax;

		m_vMinimum[0] = RVector(kMin.x, kMin.y, kMin.z);
		m_vMinimum[1] = RVector(kMax.x, kMin.y, kMin.z);
		m_vMinimum[2] = RVector(kMax.x, kMax.y, kMin.z);
		m_vMinimum[3] = RVector(kMin.x, kMax.y, kMin.z);

		RVector V = (kMax - kMin)*.5f*1.5f;

		m_vMaximum[0] = RVector(kMin.x - V.x, kMin.y - V.y, kMax.z);
		m_vMaximum[1] = RVector(kMax.x + V.x, kMin.y - V.y, kMax.z);
		m_vMaximum[2] = RVector(kMax.x + V.x, kMax.y + V.y, kMax.z);
		m_vMaximum[3] = RVector(kMin.x - V.x, kMax.y + V.y, kMax.z);
	}

	// Patch 목록에 추가
	float cx = (m_rect.right + m_rect.left)/2.0f;
	float cy = (m_rect.bottom + m_rect.top)/2.0f;
	float bx = (pBBox->minx + pBBox->maxx) /2.0f;
	float by = (pBBox->miny + pBBox->maxy) /2.0f;

	float dx = bx - cx;
	float dy = by - cy;

	// 중심점에 인접한 패치가 현재 노드에 추가된다.
	if ( abs(dx) < 0.1f && abs(dy) < 0.1f )
	{
		// 중복 Index 방지
		bool bFlag = false;
		for ( size_t si = 0; si<m_arrPatches.size(); ++si )
		{
			if (m_arrPatches[si] == pPatch)
			{
				bFlag = true;
				break;
			}
		}

		if ( !bFlag ) 
			m_arrPatches.push_back(pPatch);

		return m_iDepth;
	}
	else
	{
		// 그렇지 않으면, 하위 노드의 근접한 영역에 재추가 시도.
		SplitNode();

		if ( dx < 0.0f )
		{
			if ( dy < 0.0f )
			{
				// (-,-) 
				return m_pChild[LT]->AddTerrainPatch(pPatch);
			}
			else
			{
				// (-,+)
				return m_pChild[LB]->AddTerrainPatch(pPatch);
			}
		}
		else
		{
			if ( dy < 0.0f )
			{
				// (+,-)
				return m_pChild[RT]->AddTerrainPatch(pPatch);
			}
			else
			{
				// (+,+)
				return m_pChild[RB]->AddTerrainPatch(pPatch);
			}
		}
	}
}

void RTerrainQuadTree::UpdateNode(RTerrainQuadTree *pNode)
{
	return;
}

/**
void RTerrainQuadTree::RemoveSceneNode(rs3::RSceneNode *pSceneNode)
{
	RSceneNodeList *pNodeList = GetSceneNodeList();
	for ( RSceneNodeList::iterator it = pNodeList->begin(); it != pNodeList->end(); ++it )
	{
		RSceneNode *ptr = *it;
		if ( ptr == pSceneNode )
		{
			pNodeList->erase(it);

			// RTreeSystem을 좀 수정한 다음, Update와 Render의 순서에 주의하여 결정한다.
			//ptr->RemoveFromParent();
			//delete ptr;
			//ptr = NULL;
			return;
		}
	}

	if ( isLeaf())
		return;

	for ( int i = 0; i<4; ++i )
	{
		RTerrainQuadTree *pChild = GetChild(i);
		if ( pChild )
			pChild->RemoveSceneNode(pSceneNode);
	}
}

int RTerrainQuadTree::RemoveTerrainObject(rs3::RSceneNode *pSceneNode)
{
	RemoveSceneNode(pSceneNode);
	return 1;
}
*/

void RTerrainQuadTree::Update()
{
	UpdateNode(this);
}

int RTerrainQuadTree::CreateNode(float left, float top, float right,float bottom)
{
	m_rect.left = left;
	m_rect.top  = top;
	m_rect.bottom = bottom;
	m_rect.right  = right;

	return m_iDepth;
}

void RTerrainQuadTree::DeleteNode()
{
	SAFE_DELETE(m_pNodeIndex);
	SAFE_DELETE(m_pPatchBBox);
	SAFE_DELETE(m_pPatchSphere);
	//SAFE_DELETE(m_pObjectBBox);

	if ( !m_bIsLeaf )
	{
		SAFE_DELETE( m_pChild[0] );
		SAFE_DELETE( m_pChild[1] );
		SAFE_DELETE( m_pChild[2] );
		SAFE_DELETE( m_pChild[3] );
	}
}

}