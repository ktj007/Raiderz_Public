#include "stdafx.h"
#include "XTargetSticky.h"
#include "XNonPlayer.h"

XTargetSticky::XTargetSticky(XObject* pObject)
: XTargetLine(pObject)
{

}

XTargetSticky::~XTargetSticky()
{

}

void XTargetSticky::UpdateTarget()
{
	// 카메라 정보 가져오기
	vec3 vPos;
	vec3 vDir;
	GetCameraInfo(vPos, vDir);

	// 타겟 유효한가?
	CheckExistTargetObj();

	// 타겟 업데이트
	UpdateStickyTarget(vPos, vDir);
}

void XTargetSticky::UpdateStickyTarget( vec3& pos, vec3& dir )
{
	///************************************************************************/
	///* // [10/10/2007 isnara] 일단 타겟은 오직 하나 잡는걸로 되어 있다.     */ 
	///************************************************************************/
	float fNearDis	= 99999.0f;
	stTargetID targetObject;

	for(XObjectMap::iterator itor = gg.omgr->BeginItor(); itor != gg.omgr->EndItor(); ++itor)
	{
		XObject* pObj = (*itor).second;

		//------------------------------------------------------------------------
		// 사용 가능 타겟 검사
		if(CheckTargetAllowed(pObj) == false)	continue;

		//------------------------------------------------------------------------
		// 상대에 대한 체크
		// 타겟 조건이 안돼는가?
		if(CheckTargetObject(pObj) == false)	continue;

		//------------------------------------------------------------------------
		// 기타 체크
		// 부분 판정이 있는가?
		XModuleCollision* pModuleCollision = pObj->GetModuleCollision();
		if(pModuleCollision == NULL)	continue;

		//------------------------------------------------------------------------
		// 타겟 검사
		CheckLineTarget(pObj, pos, dir, targetObject, fNearDis);
	}

	// 최종 타겟
	if(targetObject.uidTarget != MUID::ZERO)
	{
		if (global.ui)
			global.ui->TargetUILock();

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
	}
	else
	{
		// 없다면...
		DelTargetAll();

		if (global.ui)	global.ui->TargetUIAim();
	}
}

bool XTargetSticky::CheckLineTarget( XObject* pObject, vec3& pos, vec3& dir, stTargetID& outTargetObject, float& fOutDist )
{
	bool bColl = false;

	//------------------------------------------------------------------------
	// 충돌 판정
	// 앞쪽에 있는 사람만 체크
	vec3 vObjectDir = (pObject->GetPosition() - m_pOwner->GetPosition()).Normalize();
	if(vObjectDir.DotProduct(m_pOwner->GetDirection()) > 0)
	{
		if(CheckLineTargetObj(pObject, pos, dir, outTargetObject, fOutDist))
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

bool XTargetSticky::CheckTargetObject( XObject* pObject )
{
	if( pObject )
	{
		return CSTalentInfoHelper::CheckEffectiveRange(pObject->GetPosition(), m_pOwner->GetPosition(), m_fTargetRange);		// 타겟 사거리에 들어오나
	}

	return false;
}

bool XTargetSticky::CheckLineTargetObj( XObject* pObject, vec3& pos, vec3& dir, stTargetID& outTargetObject, float& fOutDist )
{
	if(pObject->GetModuleCollision())
	{
		vec3 vCenterPos;
		float fCurDist			= fOutDist;

		if(pObject->GetModuleCollision()->IsHaveHitEffectCount())
		{
			// 히트 이펙트 캡슐로 검사
			// 1. Ray 검사
			if(pObject->GetModuleCollision()->PickRayToHitEffectObject(pos, dir, fCurDist, vCenterPos, fOutDist, outTargetObject.nTargetGroupID, outTargetObject.nTargetCapsuleID)) 
			{
				outTargetObject.uidTarget			= pObject->GetUID();
				outTargetObject.vTargetPos			= vCenterPos;

				return true;
			}

			// 2. 1번이 안돼면 Sticky 방식으로 검사
			if(pObject->GetModuleCollision()->LineRayToHitEffectObject(pos, dir, fCurDist, vCenterPos, fOutDist, outTargetObject.nTargetGroupID, outTargetObject.nTargetCapsuleID))
			{
				outTargetObject.uidTarget			= pObject->GetUID();
				outTargetObject.vTargetPos			= vCenterPos;

				return true;
			}
		}
		else
		{
			// 충돌캡슐 Sticky 방식으로 검사
			MCapsule outCapsule;
			if(pObject->GetModuleCollision()->PickRayToColCapsule(pos, dir, fCurDist, outCapsule, vCenterPos, fOutDist))
			{
				outTargetObject.uidTarget			= pObject->GetUID();
				outTargetObject.vTargetPos			= vCenterPos;

				return true;
			}
		}
	}

	return false;
}

