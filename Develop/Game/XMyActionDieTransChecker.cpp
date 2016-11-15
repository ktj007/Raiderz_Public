#include "stdafx.h"
#include "XMyActionDieTransChecker.h"
#include "XModuleMotion.h"
#include "XModuleMyControl.h"
#include "XMyPlayer.h"

//////////////////////////////////////////////////////////////////////////
/// XMyActionDieTransChecker

bool XMyActionDieTransChecker::CheckReservedHitByTalent( bool bReservedHitByTalent )
{
	if (bReservedHitByTalent) return false;

	return true;
}

bool XMyActionDieTransChecker::CheckMotion( bool bIsHoldingSequence, wstring& strCurrMotion )
{
	if (bIsHoldingSequence ||
		strCurrMotion == MOTION_NAME_DIE)
	{
		return true;
	}

	return false;
}

bool XMyActionDieTransChecker::CheckForceToDead( float fDeltaTime )
{
	m_fDeadElapsedTime += fDeltaTime;

	// Á×¾îÀÖ´Â »óÅÂ·Î 5ÃÊ ÀÌ»óÀÌ¸é ¹º°¡ ¾ÃÈù°ÍÀÌ¹Ç·Î Á×¾ú´Ù°í Ã³¸®
	const float TOLER_DEAD_ELAPSED_TIME = 5.0f;
	if (m_fDeadElapsedTime >= TOLER_DEAD_ELAPSED_TIME)
	{
		return true;
	}

	return false;
}

bool XMyActionDieTransChecker::CheckDead( bool bDead )
{
	if (bDead)
	{
		return true;
	}

	m_fDeadElapsedTime = 0.0f;

	return false;
}

bool XMyActionDieTransChecker::_IsReady( float fDeltaTime, bool bIsDead, bool bReservedHitByTalent )
{
	m_fDeadElapsedTime += fDeltaTime;

	if (CheckDead(bIsDead) == false) return false;

	if (CheckForceToDead(fDeltaTime) == true)
	{
		return true;
	}

	if (CheckReservedHitByTalent(bReservedHitByTalent) == false) return false;

	return true;

}

bool XMyActionDieTransChecker::_IsReadyForHitState( float fDeltaTime, bool bCheckMotion, bool bIsDead, bool bReservedHitByTalent, bool bIsHoldingSequence, wstring& strCurrMotion )
{
	m_fDeadElapsedTime += fDeltaTime;

	if (CheckDead(bIsDead) == false) return false;
	if (CheckReservedHitByTalent(bReservedHitByTalent) == false) return false;

	if (bCheckMotion)
	{
		if (CheckMotion(bIsHoldingSequence, strCurrMotion) == false) return false;
	}

	return true;

}

bool XMyActionDieTransChecker::IsReady( float fDeltaTime, XMyPlayer* pOwner )
{
	XModuleMyControl* pModuleMyControl = pOwner->GetModuleMyControl();
	if (pModuleMyControl == NULL) return false;

	bool bReservedHitByTalent = pModuleMyControl->IsReservedHitByTalent();
	bool bIsDead = pOwner->IsDead();

	return _IsReady(fDeltaTime, bIsDead, bReservedHitByTalent);
}

bool XMyActionDieTransChecker::IsReadyForHitState( float fDeltaTime, XMyPlayer* pOwner, bool bCheckMotion )
{	
	XModuleMyControl* pModuleMyControl = pOwner->GetModuleMyControl();
	XModuleMotion* pModuleMotion = pOwner->GetModuleMotion();
	if (pModuleMyControl == NULL || pModuleMotion == NULL) return false;

	bool bReservedHitByTalent = pModuleMyControl->IsReservedHitByTalent();
	bool bIsDead = pOwner->IsDead();

	return _IsReadyForHitState(fDeltaTime, bCheckMotion, bIsDead, bReservedHitByTalent, pModuleMotion->IsHoldingSequence(), pModuleMotion->GetCurrMotion());

}
