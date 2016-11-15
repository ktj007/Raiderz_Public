#include "stdafx.h"
#include "XTalentHelper.h"
#include "XPlayer.h"
#include "XTalentInfoMgr.h"
#include "XGlobal.h"
#include "XStrings.h"
#include "XGameState.h"

XTalentInfo* XTalentHelper::FindTalentInfo( int nTalentID, XObject* pObject )
{
	if (pObject && pObject->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = pObject->AsPlayer();
		WEAPON_TYPE nMode = pPlayer->GetCurrWeaponType();
		return info.talent->Get(nTalentID, nMode);
	}
	else
	{
		return info.talent->Get(nTalentID);
	}

	return NULL;
}

wstring XTalentHelper::MakeFullPlayerTalentAniName( const wchar_t* szAniName, CHAR_STANCE nStance, WEAPON_TYPE nCurrWeaponType )
{
	bool bWeaponAni = false;

	if (nStance == CS_BATTLE) bWeaponAni = true;

	//if( m_pTalentInfo->m_nSkillType == ST_MAGIC)
	//	bWeaponAni = false;

	if (bWeaponAni)
	{
		if ( ( nCurrWeaponType != WEAPON_MAX ) && ( nCurrWeaponType != WEAPON_NONE ) )
		{
			return wstring(XStrings::WeaponType(nCurrWeaponType)) + wstring(L"_") + wstring(szAniName);
		}
	}
	else
	{
		WEAPON_TYPE weapon_type = WEAPON_NONE;//player->GetCurrWeaponType();
		return wstring(XStrings::WeaponType(WEAPON_TYPE(weapon_type))) + wstring(L"_") + wstring(szAniName);
	}

	return wstring(szAniName);
}

MCapsule XTalentHelper::GetTalentHitCapule( int nTalentID, int nSegIndex, int nCapsuleIndex )
{
	MCapsule hitCapsule;

	XTalentInfo* pTalentInfo = FindTalentInfo(nTalentID);
	if(pTalentInfo == NULL ||
		nSegIndex < 0 || nCapsuleIndex < 0)
		return hitCapsule;

	if((int)pTalentInfo->m_HitInfo.m_vSegments.size() > nSegIndex)
	{
		if((int)pTalentInfo->m_HitInfo.m_vSegments[nSegIndex].m_vCapsules.size() > nCapsuleIndex)
		{
			hitCapsule = pTalentInfo->m_HitInfo.m_vSegments[nSegIndex].m_vCapsules[nCapsuleIndex];
		}
	}

	return hitCapsule;
}

MCapsule XTalentHelper::GetTalentHitCapule( XTalentInfo* pTalentInfo, int nSegIndex, int nCapsuleIndex, MMatrix& matWorld )
{
	MCapsule hitCapsule;

	if(pTalentInfo == NULL ||
		nSegIndex < 0 || nCapsuleIndex < 0)
		return hitCapsule;

	if((int)pTalentInfo->m_HitInfo.m_vSegments.size() > nSegIndex)
	{
		if((int)pTalentInfo->m_HitInfo.m_vSegments[nSegIndex].m_vCapsules.size() > nCapsuleIndex)
		{
			hitCapsule = pTalentInfo->m_HitInfo.m_vSegments[nSegIndex].m_vCapsules[nCapsuleIndex];
		}
	}

	vec3 b, t;
	matWorld.TransformVect(hitCapsule.bottom, b);
	matWorld.TransformVect(hitCapsule.top, t);

	return MCapsule(b, t, hitCapsule.radius);
}

bool XTalentHelper::IsTalentCancelableMotionFactor( MF_STATE nMotionFactor )
{
	if (nMotionFactor == MF_NONE ||
		nMotionFactor == MF_FAKE_BEATEN ||
		nMotionFactor == MF_FAKE_KNOCKBACK)
	{
		return false;
	}
	return true;
}

bool XTalentHelper::CheckPortalTalentUsable( XTalentInfo* pTalentInfo )
{
	if (NULL == pTalentInfo) return false;
	if(!gg.currentgamestate->GetWorld()) return false;	
	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if (NULL == pFieldInfo) return false;

	if (false == pFieldInfo->m_bPotralTalentUsable)
	{
		if (true == pTalentInfo->IsPortalTalent())
		{
			return false;
		}			
	}

	return true;
}