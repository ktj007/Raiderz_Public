#include "stdafx.h"
#include "XTargetLine.h"
#include "XNonPlayer.h"

XTargetLine::XTargetLine(XObject* pObject)
: XTarget(pObject)
, m_pTarget(NULL)
{

}

XTargetLine::~XTargetLine()
{
	SAFE_DELETE(m_pTarget);
}

void XTargetLine::UpdateTarget()
{

	// 카메라 정보 가져오기
	vec3 vPos;
	vec3 vDir;
	GetCameraInfo(vPos, vDir);

	// 타겟 유효한가?
	CheckExistTargetObj();

	// 타겟 업데이트
	UpdateTarget(vPos, vDir);
}

void XTargetLine::CheckExistTargetObj()
{
	if( m_pTarget != NULL)
	{
		XObject* pTargetObject = gg.omgr->FindObject(m_pTarget->uidTarget);
		if(pTargetObject == NULL)
		{
			DelTargetAll();
		}
	}
}

// 존재하지 않으면 삭제
void XTargetLine::DelTargetAll()
{
	PFC_THISFUNC;

	if (m_pTarget == NULL) return;

	m_TargetRenderer.DelTargetEffect(m_pTarget->uidEffect);

	SAFE_DELETE(m_pTarget);
}

void XTargetLine::UpdateTarget( vec3& pos, vec3& dir )
{
	PFC_THISFUNC;

	///************************************************************************/
	///* // [10/10/2007 isnara] 일단 타겟은 오직 하나 잡는걸로 되어 있다.     */ 
	///************************************************************************/
	float fNearDis	= 99999.0f;
	stTargetID targetObject;

	for(XObjectMap::iterator itor = gg.omgr->BeginItor(); itor != gg.omgr->EndItor(); ++itor)
	{
		XObject* pObj = (*itor).second;
		if (m_pOwner == pObj) continue;

		//------------------------------------------------------------------------
		// 사용 가능 타겟 검사
		if(CheckTargetAllowed(pObj) == false)	continue;

		//------------------------------------------------------------------------
		// 상대에 대한 체크
		// 타겟 조건이 안돼는가?
		if(CheckTargetObject(pObj, pos) == false)	continue;

		//------------------------------------------------------------------------
		// 타겟 검사
		//stTargetID targetObject;
		CheckLineTarget(pObj, pos, dir, targetObject, fNearDis);
	}

	// 최종 타겟
	if(targetObject.uidTarget != MUID::ZERO)
	{
		// 오직 하나라는 조건에 검사를 한다.
		// 이전과 같은가?
		if(CheckOverlapDummy(targetObject))
		{
			// 같다
			return;
		}

		// 오직 하나라 무조건 그냥 다 지우고 다시 검색을 한다.
		DelTargetAll();

		//mlog("타겟 정보 %d %d\n", targetObject.nTargetGroupID, targetObject.nTargetDummyID);

		// 타겟 셋팅
		m_pTarget = new stTargetID();
		m_pTarget->uidTarget		= targetObject.uidTarget;
		m_pTarget->nTargetGroupID	= targetObject.nTargetGroupID;
		m_pTarget->nTargetCapsuleID	= targetObject.nTargetCapsuleID;
		m_pTarget->time				= timeGetTime();
	}
	else
	{
		// 없다면...
		DelTargetAll();
	}
}

bool XTargetLine::CheckLineTarget( XObject* pObject, vec3& pos, vec3& dir, stTargetID& outTargetObject, float& fOutDist )
{
	PFC_THISFUNC;

	bool bColl = false;

	//------------------------------------------------------------------------
	// 충돌 판정


	// 앞쪽에 있는 사람만 체크
	vec3 vObjectDir = (pObject->GetPosition() - pos).Normalize();
	if(vObjectDir.DotProduct(dir) > 0)
	{
		if(CheckLineTargetObj(pObject, pos, dir,outTargetObject, fOutDist))
		{
			bColl = true;
		}
	}

	//------------------------------------------------------------------------
	// 충돌이면 지형 충돌 검사
	if(bColl)
	{
		vec3 vTargetPos = outTargetObject.vTargetPos;

		float fDistTerrain = 0.0f;
		vec3 vTerrainPos;
		if(CheckColTerrain(pos, vTargetPos, fDistTerrain, &vTerrainPos))
		{
			if(fOutDist > fDistTerrain)
			{
				//#ifdef _DEBUG
				//		global.smgr->GetPrimaryCamera()->GetPassRenderer< RDebugPassRenderer >()->AddDebugLine(pos, vTerrainPos, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
				//#endif
				//mlog("지형 충돌 %f(%f)\n", fDistTerrain, fNearDis);
				outTargetObject.Init();
				return false;
			}
		}

		return true;
	}

	return false;
}

bool XTargetLine::CheckTargetObject( XObject* pObject, vec3& pos)
{
	PFC_THISFUNC;

	if( m_pTarget &&
		m_pTarget->uidTarget != pObject->GetUID())
	{
		if(CheckTargetHitCapsule(pObject) == false)
			return false;

		return CSTalentInfoHelper::CheckEffectiveRange(pObject->GetPosition(), pos, m_fTargetRange);		// 타겟 사거리에 들어오나
	}

	return true;
}

bool XTargetLine::CheckLineTargetObj( XObject* pObject, vec3& pos, vec3& dir, stTargetID& outTargetObject, float& fOutDist )
{
	PFC_THISFUNC;

	if(pObject->GetModuleCollision())
	{
		vec3 vCenterPos;
		float fCurDist			= fOutDist;

		// 1. Pick으로 검사
		if(pObject->GetModuleCollision()->PickRayToHitEffectObject(pos, dir, fCurDist, vCenterPos, fOutDist, outTargetObject.nTargetGroupID, outTargetObject.nTargetCapsuleID))
		{
			outTargetObject.uidTarget			= pObject->GetUID();
			outTargetObject.vTargetPos			= pos + (dir * fOutDist);

			return true;
		}
	}

	return false;
}

bool XTargetLine::GetTargetIDByUID( MUID uid, int& nOutGroupID, int& nOutDummyID )
{
	if( m_pTarget &&
		m_pTarget->uidTarget == uid)
	{
		nOutGroupID = m_pTarget->nTargetGroupID;
		nOutDummyID = m_pTarget->nTargetCapsuleID;

		return true;
	}

	return false;
}

bool XTargetLine::CheckOverlapDummy( stTargetID& targetObject )
{
	// 현재 저장되어 있는 타겟과 검사를 한다.
	// 타겟 정보가 있어야 한다. 그래야 비교를 하닌깐...
	if( m_pTarget &&
		m_pTarget->uidTarget != MUID::ZERO)
	{
		if(targetObject.uidTarget == m_pTarget->uidTarget &&
			targetObject.nTargetGroupID == m_pTarget->nTargetGroupID &&
			targetObject.nTargetCapsuleID == m_pTarget->nTargetCapsuleID)					
		{
			// 같다면...
			return true;
		}
	}

	return false;
}
