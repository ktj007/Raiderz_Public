#include "stdafx.h"
#include "MPhysX4R3.h"
#include "MPhysX.h"

#include "RCollisionTree.h"
#include "RCollision.h"
#include "RTerrain.h"
#include "RHeightField.h"

using namespace rs3;
using namespace physx;

void RPxObjStaticSubActorBuilder::SetCollisionTreeRootNode(const rs3::RCollisionTreeNode *pNode, float _fCubeSize)
{
	_ASSERT( _fCubeSize > 0.f );
	_ASSERT( (pNode->GetRightNode() || pNode->GetLeftNode() ) );

	m_pCollisionTree = const_cast<rs3::RCollisionTreeNode *>(pNode);
	m_bUseCubeSector = true;
	SetCubeSectorInfo(m_pCollisionTree->GetBoundBox(), _fCubeSize);

	//mlog("SetCollisionTreeRootNode:%f, %f\n", m_pCollisionTree->GetBoundBox().vmax.x, m_pCollisionTree->GetBoundBox().vmin.x);
}

void RPxObjStaticSubActorBuilder::SetTerrain(const rs3::RTerrain* pTerrain, int _nDivisionCount)
{
	m_pTerrain = const_cast< rs3::RTerrain *>(pTerrain);
	_ASSERT(m_pTerrain != NULL);

	RHeightField *pHeightField = m_pTerrain->GetHeightFieldPtr();
	_ASSERT(pHeightField);

	// calc recommended collision cube size
	m_fRecommendedCollisionCubeSize = pHeightField->GetWidthSize() / _nDivisionCount;

	// sector info
	SetSquareSectorInfo(_nDivisionCount, pHeightField->GetWidthMap(), pHeightField->GetHeightMap(), pHeightField->GetXScale(), pHeightField->GetYScale());
	m_bUseSquareSector = true;

	//mlog("SetTerrain:%d, %d, %d, %f, %f\n", _nDivisionCount, pHeightField->GetWidthMap(), pHeightField->GetHeightMap(), pHeightField->GetXScale(), pHeightField->GetYScale());
}

void RPxObjStaticSubActorBuilder::OnGetCubeSectorMesh(std::vector<NxVec3>& _refVertices, std::vector<NxU32>& _refFaces, const MBox& _refPieceAABB)
{
	GetCollsionMeshInfo(_refPieceAABB, m_pCollisionTree, _refVertices );

	// 속도를 위해 GetCollsionMeshInfo에서는 vertice 정보만 얻어오고 face 인덱스 계산은 여기서 한다.
	if (false == _refVertices.empty())
	{
		_refFaces.resize(_refVertices.size());
	}

	for (size_t i = 0; i< _refVertices.size(); ++i)
	{
		_refFaces[i] = i;
	}
}

void RPxObjStaticSubActorBuilder::OnGetSquareSectorMesh(std::vector<NxVec3>& _refVertices, int x1, int y1, int x2, int y2)
{
	_ASSERT(m_pTerrain != NULL);
	RHeightField *pHeightField = m_pTerrain->GetHeightFieldPtr();
	_ASSERT(pHeightField);

	int nMapTriangleCount = pHeightField->GetWidthMap() * pHeightField->GetHeightMap() * 2;
	if(nMapTriangleCount > 0)
	{
		for(int i = y1; i <= y2; i++)
		{
			for(int j = x1; j <= x2; j++)
			{
				RVector v0;
				pHeightField->GetPositionAtIndexXY(&v0, i, j);
				_refVertices.push_back(NxVec3(v0.x, v0.y, v0.z));
			}
		}
	}
}

void RPxObjStaticSubActorBuilder::GetCollsionMeshInfo( const MBox& _refAABB, const rs3::RCollisionTreeNode *pNode, std::vector<NxVec3>& _refVertices )
{
	_ASSERT( pNode != NULL );
	if(_refAABB.ContainsAABB(pNode->GetBoundBox()))
	{
		if(pNode->IsLeafNode())
		{
			int nPolygonCount = pNode->GetPolygonCount();
			for(int i = 0;i<nPolygonCount;i++) 
			{
				const rs3::RCollisionTriangle& triangles = pNode->GetTriangles(i);
				if ( triangles.dwNodeAttribute & ~rs3::RCollisionTriangle::WATER )
				{
					_refVertices.push_back( NxVec3(triangles.v[0].x, triangles.v[0].y, triangles.v[0].z) );
					_refVertices.push_back( NxVec3(triangles.v[1].x, triangles.v[1].y, triangles.v[1].z) );
					_refVertices.push_back( NxVec3(triangles.v[2].x, triangles.v[2].y, triangles.v[2].z) );
				}
			}
		}
		else 
		{
			GetCollsionMeshInfo(_refAABB, pNode->GetLeftNode(), _refVertices);
			GetCollsionMeshInfo(_refAABB, pNode->GetRightNode(), _refVertices);
		}
	}
}
