#include "StdAfx.h"
#include "XTalentAvailable.h"
#include "XMyPlayer.h"
#include "XTalentInfoMgr.h"
#include "XModuleBuff.h"
#include "XGameState.h"
#include "XTalentHelper.h"


XTalentAvailable::XTalentAvailable(void)
: m_nResult(RESULT_TRUE)
{
}

XTalentAvailable::~XTalentAvailable(void)
{
}

bool XTalentAvailable::IsAvailable(XTalentInfo* pTalentInfo)
{
	if (pTalentInfo == NULL) 
	{
		m_nResult = RESULT_FALSE;
		return false;
	}

	if(CheckTrainedTalent(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE_UNTRAINED;
		return false;
	}

	if (CheckBuff(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE_BUFF;
		return false;
	}

	if (CheckCoolTime(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE_COOLTIME;
		return false;
	}

	if(CheckStance(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE_STANCE;
		return false;
	}

	if (CheckSTA(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE_STA;
		return false;
	}

	if (CheckEN(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE_EN;
		return false;
	}

	if (CheckWeapon(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE_WEAPON;
		return false;
	}

	if (CheckAlive(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE;
		return false;
	}

	if (CheckPortalTalentUsable(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE_PORTALTALENT;
		return false;
	}

	if (CheckGuard(pTalentInfo) == false)
	{
		m_nResult = RESULT_FALSE;
		return false;
	}

	m_nResult = RESULT_TRUE;
	return true;
}

bool XTalentAvailable::IsAvailable( int nTalentID )
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (NULL == pTalentInfo) return false;

	return IsAvailable(pTalentInfo);
}

bool XTalentAvailable::CheckBuff( XTalentInfo* pTalentInfo )
{
	// 특정 버프를 요구하는 탈랜트일 경우 해당 버프가 걸려있는지 확인한다
	if( pTalentInfo->m_nRequireFocus != TFT_NONE &&
		gvar.MyInfo.BuffExist(BUFF_FOCUS_ID[pTalentInfo->m_nRequireFocus]) == false)
	{
		return false;
	}


	// 버프 정보
	if ( gvar.Game.pMyPlayer != NULL)
	{
		XModuleBuff* pModuleBuff = gvar.Game.pMyPlayer->GetModuleBuff();
		if ( pModuleBuff != NULL)
		{
			XBuffAttribute _buff = pModuleBuff->GetBuffAttribute();
			if ( _buff.IsUsableTalent( pTalentInfo) == false)	return false;
		}
	}

	return true;
}

bool XTalentAvailable::CheckStance( XTalentInfo* pTalentInfo )
{
	if ( gvar.Game.pMyPlayer == NULL)	return false;

	if (CheckBitSet(pTalentInfo->m_nEnabledStanceFlag, CS_BATTLE) && 
		!CheckBitSet(pTalentInfo->m_nEnabledStanceFlag, CS_NORMAL))
	{
		if(gvar.Game.pMyPlayer->GetStance() == CS_NORMAL)
		{
			return false;
		}
	}
	else if (CheckBitSet(pTalentInfo->m_nEnabledStanceFlag, CS_NORMAL) &&
		!CheckBitSet(pTalentInfo->m_nEnabledStanceFlag, CS_BATTLE))
	{
		if(gvar.Game.pMyPlayer->GetStance() == CS_BATTLE)
		{
			return false;
		}
	}

	return true;
}

bool XTalentAvailable::CheckSTA( XTalentInfo* pTalentInfo )
{
	if(pTalentInfo->m_nSTACost > gvar.MyInfo.GetSTA())
		return false;

	return true;
}

bool XTalentAvailable::CheckEN( XTalentInfo* pTalentInfo )
{
	if (pTalentInfo->m_nENCost > gvar.MyInfo.GetEN())
		return false;

	return true;
}

bool XTalentAvailable::CheckWeapon( XTalentInfo* pTalentInfo )
{
	if (gvar.Game.pMyPlayer == NULL) return false;

	WEAPON_TYPE nWeaponType = gvar.Game.pMyPlayer->GetCurrWeaponType();
	bool bEquipShield = gvar.Game.pMyPlayer->IsEquipShield();
	if (pTalentInfo->m_WeaponAllowedInfo.IsAllowed(nWeaponType, bEquipShield) == false)
	{
		return false;
	}

	return true;
}

bool XTalentAvailable::CheckCoolTime( XTalentInfo* pTalentInfo )
{
	if(pTalentInfo->m_fCoolTime != 0.f)
	{
		return gvar.MyInfo.Talent.CoolTime.IsCoolDown(pTalentInfo->m_nID) ? false : true;
	}

	return true;
}

bool XTalentAvailable::CheckAlive( XTalentInfo* pTalentInfo )
{
	if (gvar.Game.pMyPlayer == NULL) return false;

	if (gvar.Game.pMyPlayer->IsDead() == false &&
		pTalentInfo->m_bUsableAtDead == true)
		return false;

	return true;
}

bool XTalentAvailable::CheckTrainedTalent( XTalentInfo* pTalentInfo )
{
	if (gvar.Game.pMyPlayer == NULL) return false;

	return gvar.MyInfo.Talent.IsLearnedTalent(pTalentInfo);
}

bool XTalentAvailable::CheckPortalTalentUsable(XTalentInfo* pTalentInfo)
{
	return XTalentHelper::CheckPortalTalentUsable(pTalentInfo);
}

bool XTalentAvailable::CheckGuard( XTalentInfo* pTalentInfo )
{
	if (gvar.Game.pMyPlayer->GetModuleMyControl()->GetCurrentActionStateID() == ACTION_STATE_GUARD)
	{
		if (!pTalentInfo->m_bAvailableOnGuard) return false;
	}

	return true;
}
