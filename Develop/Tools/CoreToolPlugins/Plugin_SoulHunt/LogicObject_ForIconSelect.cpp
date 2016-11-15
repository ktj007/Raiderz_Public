#include "stdafx.h"
#include "LogicObject_ForIconSelect.h"
#include "M_EntityObject.h"
#include "M_ToolTerrain.h"
#include "C_ControlMediator.h"
#include "../../ToolsCommon/toolPick.h"

// 사이즈
#define ICON_DEFAULT_SIZE  300
#define ICON_SIZE_RATIO  40.0f

CLogicObject_ForIconSelect::CLogicObject_ForIconSelect()
{
	m_fSize = ICON_DEFAULT_SIZE;
	m_fOffset = 0.0f;

	m_bUse = false;
	m_bReady = false;
}

CLogicObject_ForIconSelect::~CLogicObject_ForIconSelect()
{

}

void CLogicObject_ForIconSelect::Update( CEntityObject* pObject, CControlMediator* pControlMediator_ )
{
	UpdateSizeForIconSelection(pObject, pControlMediator_);

	// 만약입니다.
	// Offset이 준비가 안되었으면...
	if(m_bReady == false)
		UpdateSmartOffsetHeightForIconSelection(pObject, pControlMediator_);
}

void CLogicObject_ForIconSelect::UpdateSizeForIconSelection( CEntityObject* pObject, CControlMediator* pControlMediator_ )
{
	if(m_bUse == false)
		return;

	rs3::RVector3 vPos;
	if ( pObject->GetPosition(vPos) == false || pControlMediator_->GetToolCamera() == NULL)
	{
		m_fSize = ICON_DEFAULT_SIZE;
		return;
	}

	float fLen = (vPos - pControlMediator_->GetToolCamera()->GetPosition()).Length();
	m_fSize = fLen / ICON_SIZE_RATIO;

	if(m_fSize < 1.0f)
		m_fSize = 1.0f;
}

bool CLogicObject_ForIconSelect::PickForSelection( CEntityObject* pObject, CControlMediator* pControlMediator_, float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to )
{
	vec3 vDir = (to - origin);
	vDir.Normalize();

	rs3::RVector3 vPos;
	if ( pObject->GetPosition(vPos) == false || m_bUse == false)
		return false;

	vPos.z += m_fSize + m_fOffset;
	float fDist = 0.0f;

	MMatrix matView = pControlMediator_->GetCurrentDevice()->GetTransform(rs3::RST_VIEW);
	if(CToolPick::BillboardPicking(vPos, m_fSize, m_fSize, origin, vDir, matView, fDist))
	{
		if(fDist < _rPrevPickedSqLength)
		{
			_rPrevPickedSqLength = fDist;
			return true;
		}
	}

	return false;
}

bool CLogicObject_ForIconSelect::PickForSelection( CEntityObject* pObject, CControlMediator* pControlMediator_, rs3::RViewFrustum& _refFrustum )
{
	rs3::RVector3 vPos;
	if ( pObject->GetPosition(vPos) == false || m_bUse == false)
		return false;

	vPos.z += m_fSize + m_fOffset;
	float fDist = 0.0f;

	MMatrix matView = pControlMediator_->GetCurrentDevice()->GetTransform(rs3::RST_VIEW);
	if(CToolPick::BillboardIntersection(vPos, m_fSize, m_fSize, _refFrustum, matView))
	{
		return true;
	}

	return false;
}

void CLogicObject_ForIconSelect::UpdateSmartOffsetHeightForIconSelection( CEntityObject* pObject, CControlMediator* pControlMediator_ )
{
	m_fOffset = 0.0f;

	rs3::RVector3 vPos;
	if ( m_bUse == false || pObject->GetPosition(vPos) == false || pControlMediator_->GetToolCamera() == NULL)
		return;

	vec3 vTerrainContactPos(vPos);
	vec3 vOrg = pControlMediator_->GetToolCamera()->GetPosition();
	float fTerrainColLen = m_fOffset;
	bool bRet(false);

	// 1. 아이콘이 땅 밑에 있냐?
	float fIconLen = 0.0f;
	vec3 vResultPos;
	vec3 vStart = vPos + vec3(0, 0, vOrg.z);
	vec3 vTo = vPos;
	if(PickZone(pControlMediator_, vStart, vTo, fIconLen, vResultPos) == false)
	{
		// 충돌 안되고 있어요
		return;
	}

	// 2. 지상으로 옮긴다.
	if(vResultPos.z > vPos.z)
	{
		m_fOffset = vResultPos.z - vPos.z;
	}
}

bool CLogicObject_ForIconSelect::PickZone( CControlMediator* pControlMediator_, vec3 vOrg, vec3 vTo, float& fOutDist, vec3& vOutColPos )
{
	rs3::RToolTerrain* pTerrain = (rs3::RToolTerrain*) pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();
	if( pTerrain == NULL ||
		pTerrain->GetUsable() == false ||
		pTerrain->GetHeightFieldPtr() == NULL ||
		pTerrain->IsLoadingCompleted() == false)
		return false;

	// 준비됐다.
	m_bReady = true;

	bool bRet(false);

	// 1. 지형 충돌
	vec3 vTerrainContactPos;
	if(pTerrain->GetPickLine(vTerrainContactPos, NULL, vOrg, vTo))
	{
		fOutDist = (vTerrainContactPos - vOrg).Length();
		vOutColPos = vTerrainContactPos;
		bRet = true;
	}

	//// 2. 오브젝트 충돌
	//rs3::RPICKINFO_MAP mapPickInfo;
	//mapPickInfo.clear();
	//if( pControlMediator_->GetToolSceneManager()->Pick(vOrg, vTo, mapPickInfo, false) )
	//{
	//	vec3 vResultPos = mapPickInfo.begin()->second.outPosition;
	//	float fDist = (vResultPos - vOrg).Length();

	//	if(fDist < fOutDist)
	//	{
	//		fOutDist = fDist;
	//		vOutColPos = vResultPos;
	//	}

	//	return true;
	//}
	//else if(bRet)
	//{
	//	return true;
	//}

	return bRet;
}

