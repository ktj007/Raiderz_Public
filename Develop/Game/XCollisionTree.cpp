#include "stdafx.h"
#include "XCollisionTree.h"
#include "RCollisionTree.h"
#include "RCollision.h"


XCollisionTree::XCollisionTree() : m_pTerrain(NULL)
{
}

void XCollisionTree::GetCollidablePolygons(RCollision &collision, DWORD dwAttribute)
{
	RCollisionTree::GetCollidablePolygons(collision, dwAttribute);
	if(m_pTerrain && (dwAttribute & RCollisionTriangle::TERRAIN))
	{
		RBox boxMovement;
		collision.GetMovementBoundingBox(boxMovement);
		vector<RVt_pos> vecVertex;
		m_pTerrain->GetVertexFromBound(vecVertex,boxMovement);
		for(size_t i=0;i<vecVertex.size()/3;i++)
		{
			collision.CheckPolygon(vecVertex[i*3].vPos,vecVertex[i*3+1].vPos,vecVertex[i*3+2].vPos, RCollisionTriangle::TERRAIN);
		}
	}
}

bool XCollisionTree::Pick(RCollisionPickInfo& pickInfo, DWORD dwAttributeFlag, bool bNoCull)
{
	// object, tree, water 등 picking
	bool bReturn = RCollisionTree::Pick(pickInfo, dwAttributeFlag, bNoCull);

	// 지형이 있으면, 피킹해서 둘 중 가까운 결과를 검사
	if(m_pTerrain && (dwAttributeFlag & RCollisionTriangle::TERRAIN))
	{
		RVector vOut;
		RVector vNormal;
		if ( m_pTerrain->GetPickLine(vOut,&vNormal, pickInfo.m_inPickOrigin, pickInfo.m_inPickTo))
		{
			bReturn = true;
			float lengthSq = (vOut - pickInfo.m_inPickOrigin).LengthSq();
			if ( lengthSq<pickInfo.GetPickDistance() && pickInfo.m_outDistanceSq>lengthSq) 
			{
				pickInfo.m_outPosition = vOut;
				pickInfo.m_outNormal = vNormal;
				pickInfo.m_outDistanceSq = lengthSq;
				pickInfo.m_outNodeAttr = RCollisionTriangle::TERRAIN;

				DWORD dwMaterialID = m_pTerrain->GetHeightFieldPtr()->GetMaterial(vOut.x, vOut.y);
				if ( dwMaterialID >= 0 )
				{
					pickInfo.m_outMaterial = dwMaterialID;
					const RTerrain::LayerInfo *pInfo = m_pTerrain->GetLayerInfo(dwMaterialID);
					if ( pInfo )
						pickInfo.m_outMaterialName = pInfo->tileMapName.c_str();
					else
						pickInfo.m_outMaterialName = NULL;
				}
			}
		}
	}
	return bReturn;
}

int XCollisionTree::GetTerrainPolygonCount()
{
	if (m_pTerrain == NULL)
		return 0;

	RHeightField *pHeightField = m_pTerrain->GetHeightFieldPtr();
	_ASSERT(pHeightField);

	int count = pHeightField->GetWidthMap() * pHeightField->GetHeightMap() * 2;
	return count;
}

void XCollisionTree::GetTerrainTriangle(int index, RVector &v0, RVector &v1, RVector &v2)
{
	_ASSERT(m_pTerrain);

	RHeightField *pHeightField = m_pTerrain->GetHeightFieldPtr();
	_ASSERT(pHeightField);

	int n = index;
	int BottomOrUp = n%2;	n/=2;
	int x = n % pHeightField->GetHeightMap();	n/= pHeightField->GetHeightMap();
	int y = n;

	if ( BottomOrUp == 0 )
	{
		pHeightField->GetPositionAtIndexXY(&v0, x, y);
		pHeightField->GetPositionAtIndexXY(&v1, x+1,y);
		pHeightField->GetPositionAtIndexXY(&v2, x,y+1);
	}
	else
	{
		pHeightField->GetPositionAtIndexXY(&v0, x+1,y);
		pHeightField->GetPositionAtIndexXY(&v1, x+1,y+1);
		pHeightField->GetPositionAtIndexXY(&v2, x,y+1);
	}

	return;
}
