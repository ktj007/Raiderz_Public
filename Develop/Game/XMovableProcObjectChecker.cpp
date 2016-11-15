#include "stdafx.h"
#include "XMovableProcObjectChecker.h"
#include "XObject.h"
#include "XModuleCollision.h"
#include "XModuleTalent.h"
#include "XModuleMotion.h"
#include "XTalentHelper.h"

bool XMovableProcObjectChecker::CheckObjectCollision(const XMovableFactor& _rMovableFactor, XObject* pObejct)
{
	//----------------------------------------------------------------------//
	// 상대 캐릭터가 특수한 상황중에서는 오브젝트 충돌을 허용한다.
	bool bObjectCol = false;

	// ** 넉백 상태는 오브젝트 충돌을 하지 않는다. (NPC만)
	if(pObejct->GetEntityType() != ETID_PLAYER &&
		_rMovableFactor.KNOCK_BACK.m_bKnockback)
		return false;

	// 1. 탤런트 사용중
	// 2. 이동 모션 팩터 일때...
	// 3. 내 자신일때...
	if(pObejct->GetEntityType() == ETID_PLAYER)				// 플레이어이면서...
	{
		if(pObejct->GetUID() != XGetMyUID())				// 상대 캐릭터면...
		{

			// 모듈탤런트가 있는가...
			bObjectCol = _rMovableFactor.GENERAL_STATE.m_bTalentActive;// 탤런트 사용중인가...

			// 위에서 충돌 체크가 없는가...
			if(!bObjectCol &&								// 이 이후로 추가적인 상황을 넣는다.
				_rMovableFactor.IsMoveMF())					// 이동 모션팩터인가...
			{
				bObjectCol = true;
			}

			// 전투 중인가?
			if(pObejct->AsPlayer()->IsDuel())
				bObjectCol = true;
		}
		else
		{
			bObjectCol = true;
		}
	}

	// 탤런트 사용중이라도 지금 목표지점에 도착하지 않았다면 충돌패스
	if(XGetMyUID() != pObejct->GetUID())
	{
		if(_rMovableFactor.INTERPOLATION.m_bWorking &&
			_rMovableFactor.INTERPOLATION.m_bFinishMove == false)
			return  false;
	}

	// 충돌이 된다면... 충돌에 기본적인 체크
	if(bObjectCol)
	{
		XModuleCollision* pModuleCollision = pObejct->GetModuleCollision();
		if(pModuleCollision && pModuleCollision->IsBaseColliable())
		{
			XModuleTalent* pModuleTalent = pObejct->GetModuleTalent();
			if(pModuleTalent)
				return pModuleTalent->IsTalentColliable();
		}

		return false;
	}

	return bObjectCol;
}

bool XMovableProcObjectChecker::CheckCanRub( XMovableFactor& _rMovableFactor, XObject* pObejct, int nCurrentTalentID )
{
	bool bCheckRub = true;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nCurrentTalentID, pObejct);
	if(pTalentInfo)
	{
		if(pTalentInfo->m_eTalentShoveType == TST_STOP)
			bCheckRub = false;
	}

	if(_rMovableFactor.IsTalentActive() &&
		bCheckRub == false)
	{
		bCheckRub = false;
	}

	// 탤런트 사용중이라도 지금 모션이 달리기면 비빈다.
	// Player에게만 해당된 내용일것이다.
	XModuleMotion* pModuleMotion = pObejct->GetModuleMotion();
	if(pModuleMotion && pModuleMotion->IsCurrentMotion(MOTION_NAME_RUN))
	{
		bCheckRub = true;
	}

	return bCheckRub;
}

bool XMovableProcObjectChecker::CheckCapsuleRadiusOffset( const XMovableFactor& _rMovableFactor, XObject* pObejct )
{
	// 플레이어에만 해당
	if(pObejct->GetEntityType() != ETID_PLAYER)
		return false;

	// 탤런트 사용중이면... OK
	if(_rMovableFactor.IsTalentActive())
		return true;

	return false;
}