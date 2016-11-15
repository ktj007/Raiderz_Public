#include "stdafx.h"
#include "RSceneNodePolygonCollector_Collision.h"
#include "RCollisionTreeBuilder.h"

namespace rs3
{

RSceneNodePolygonCollector_Collision::RSceneNodePolygonCollector_Collision( RCollisionTreeBuilder* pTreeBuilder ) : m_pTreeBuilder(pTreeBuilder)
{

}

RSceneNodePolygonCollector_Collision::~RSceneNodePolygonCollector_Collision()
{

}

void RSceneNodePolygonCollector_Collision::HandleError( const std::string& strError )
{
	m_pTreeBuilder->m_vecFailedList.push_back(strError);
}

DWORD RSceneNodePolygonCollector_Collision::GetZoneMaterialID( const std::string &strMaterialName )
{
	return m_pTreeBuilder->GetZoneMaterialID(strMaterialName);
}

void RSceneNodePolygonCollector_Collision::OnAddPolygon( const RVector& p1, const RVector& p2, const RVector& p3, WORD wNodeFlag, WORD wUserFlag, DWORD dwMaterial )
{
	m_pTreeBuilder->AddPolygon(p1, p2, p3, wNodeFlag, wUserFlag, dwMaterial);
}

}
