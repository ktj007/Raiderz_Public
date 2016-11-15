#include "StdAfx.h"
#include "M_ToolDecal.h"

namespace rs3 {

RToolDecal::RToolDecal(void) :
m_pDecal_Polygon(NULL)
{
	m_vecDecal_Projection.clear();
}

RToolDecal::~RToolDecal(void)
{
	Destroy();
}

void RToolDecal::Destroy()
{
	for (std::vector<RDecalProjectionSceneNode*>::iterator itr = m_vecDecal_Projection.begin(); itr != m_vecDecal_Projection.end(); ++itr)
	{
		// 삭제는 요렇게
		(*itr)->RemoveFromParent();
		delete *itr;
	}
	m_vecDecal_Projection.clear();

	if(m_pDecal_Polygon)
		m_pDecal_Polygon->RemoveFromParent();

	SAFE_DELETE(m_pDecal_Polygon);
}

// http://dev/redmine/projects/engine/wiki/Decalmap
// DecalProjection(DECAL_TYPE_PROJECTION)	: 마법진 같이 매 프레임 업데이트가 필요한 데칼
// DecalPolygon(DECAL_TYPE_POLYGON)			: 총알, 혈흔같이 한번 찍히면 그리기만 할 데칼
void RToolDecal::Create(RToolSceneManager* pToolSceneManager, CRollupEtcDecal* pRollupDecal, RVector MousePosVec, CRollupEtcCollision* pCollision)
{
	if( pCollision->GetCollisionTree().GetCollisionTree() == NULL )
		pCollision->GetCollisionTree().Load( pCollision->GetCollisionFileName().c_str() );

	if( pRollupDecal->m_eDecalType == DECAL_TYPE_PROJECTION )
	{	// Projection 데칼은 데칼 객체마다 각각 생성해서 사용합니다. 하나의 데칼이 하나의 씬노드.
		RDecalProjectionSceneNode* pDecal = static_cast< RDecalProjectionSceneNode* >( pToolSceneManager->CreateSceneNode(RSID_DECAL_PROJECTION) );
		pDecal->Create( pCollision->GetCollisionTree().GetCollisionTree(), pRollupDecal->m_strTextureFileName.GetString(), MousePosVec, pRollupDecal->m_fDecalSize_X, pRollupDecal->m_fDecalSize_Y, pRollupDecal->m_fDecalSize_Z, pRollupDecal->m_fDecalRotate_Z );

		m_vecDecal_Projection.push_back(pDecal);
		pToolSceneManager->AddSceneNode(pDecal);
	}
	else if( pRollupDecal->m_eDecalType == DECAL_TYPE_POLYGON )
	{	// PolygonDecal 은 프로그램 내에서 하나의 씬노드를 가지며, 씬노드 내부에서 텍스쳐별로 알아서 데칼을 관리해준다.
		if( !m_pDecal_Polygon )	
		{
			RDecalPolygonSceneNode* pDecal = static_cast< RDecalPolygonSceneNode* >( pToolSceneManager->CreateSceneNode(RSID_DECAL_POLYGON) );
			m_pDecal_Polygon = (pDecal);
			pToolSceneManager->AddSceneNode(pDecal);

			pDecal->Create( pCollision->GetCollisionTree().GetCollisionTree() );	// 처음 한번만 설정해 주자
			m_pDecal_Polygon->Add(pRollupDecal->m_strTextureFileName.GetString(), MousePosVec, pRollupDecal->m_fDecalSize_X, pRollupDecal->m_fDecalSize_Y, pRollupDecal->m_fDecalSize_Z, pRollupDecal->m_fDecalRotate_Z);
		}
		else
		{
			m_pDecal_Polygon->Add(pRollupDecal->m_strTextureFileName.GetString(), MousePosVec, pRollupDecal->m_fDecalSize_X, pRollupDecal->m_fDecalSize_Y, pRollupDecal->m_fDecalSize_Z, pRollupDecal->m_fDecalRotate_Z);
		}
	}
}

}
