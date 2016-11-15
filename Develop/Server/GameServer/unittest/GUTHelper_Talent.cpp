#include "StdAfx.h"
#include "GUTHelper_Talent.h"
#include "GTalentInfo.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "SUnitTestUtil.h"
#include "GBuffInfo.h"
#include "GTrainingSystem.h"
#include "GPlayerTalent.h"
#include "GTalentInfoMgr.h"
#include "GTestForward.h"

void GUTHelper_Talent::SetTalentDamage( GTalentInfo* pTalentInfo, int nDamage )
{
	pTalentInfo->m_nMinDamage = pTalentInfo->m_nMaxDamage = nDamage;
}

void GUTHelper_Talent::AddSimpleTalentInfoHitInfo( GTalentInfo* pTalentInfo, float fCheckTime/*=0.0f*/ )
{
	// 캡슐은 적당히 큰 값
	MCapsule hit_cap = MCapsule(vec3(0.0f, 0.0f, -10000.0f), vec3(0.0f, 0.0f, 10000.0f), 5000.0f);
	CSHitSegment hit_seg;
	hit_seg.m_vCapsules.push_back(hit_cap);
	hit_seg.m_vDoHitCheck.push_back(true);
	hit_seg.m_fCheckTime = fCheckTime;

	int id = (int)pTalentInfo->m_HitInfo.m_vSegments.size() + 1;
	pTalentInfo->m_HitInfo.m_nID = id;
	pTalentInfo->m_HitInfo.m_vSegments.push_back(hit_seg);
}

GTalentInfo* GUTHelper_Talent::NewTalentInfo(int nID, int nTalentLine, int8 nRank, int nCooltimeLine)
{
	GTalentInfo* pNewTalentInfo = new GTalentInfo();
	pNewTalentInfo->m_nID = SUnitTestUtil::NewID(nID);
	pNewTalentInfo->m_nTalentLine = nTalentLine;
	pNewTalentInfo->m_nCooltimeLine = nCooltimeLine;
	pNewTalentInfo->m_nCategory = TC_COMMON;
	pNewTalentInfo->m_nSkillType = ST_MELEE;
	pNewTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
	pNewTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
	pNewTalentInfo->m_bNeedTraining = false;

	if (gmgr.pTalentInfoMgr->Find(pNewTalentInfo->m_nID))
	{
		// 이미 등록된 탤런트 아이디라면 제거
		gmgr.pTalentInfoMgr->Erase(pNewTalentInfo->m_nID);
	}
	
	gmgr.pTalentInfoMgr->Insert(pNewTalentInfo);
	

	return pNewTalentInfo;
}

GTalentInfo* GUTHelper_Talent::NewMeleeTalentInfo(int nID)
{
	GTalentInfo* pNewTalentInfo = NewTalentInfo(nID);
	SetMeleeTalentInfo(pNewTalentInfo, pNewTalentInfo->m_nID);
	return pNewTalentInfo;
}


GTalentInfo* GUTHelper_Talent::NewPassiveTalentInfo( int nID )
{
	GTalentInfo* pNewTalentInfo = NewTalentInfo(nID);
	SetPassiveTalentInfo(pNewTalentInfo, pNewTalentInfo->m_nID);

	return pNewTalentInfo;
}

GTalentInfo* GUTHelper_Talent::NewExtraPassiveTalentInfo( TALENT_EXTRA_PASSIVE_TYPE nExtraPassiveType, int nID, int nTalentLine )
{
	GTalentInfo* pNewTalentInfo = NewTalentInfo(SUnitTestUtil::NewID(nID), SUnitTestUtil::NewID(nTalentLine));
	SetExtraPassiveTalentInfo(pNewTalentInfo, pNewTalentInfo->m_nID, nExtraPassiveType);

	return pNewTalentInfo;
}

GTalentInfo* GUTHelper_Talent::NewArcheryTalentInfo( int nID )
{
	GTalentInfo* pNewTalentInfo = NewTalentInfo(nID);
	SetArcheryTalentInfo(pNewTalentInfo, pNewTalentInfo->m_nID);
	return pNewTalentInfo;
}

GTalentInfo* GUTHelper_Talent::NewMagicTalentInfo( int nID )
{
	GTalentInfo* pNewTalentInfo = NewTalentInfo(nID);
	pNewTalentInfo->m_nCategory = TC_MAGIC;
	pNewTalentInfo->m_nTalentType = TT_SKILL;
	pNewTalentInfo->m_nSkillType = ST_MAGIC;
	pNewTalentInfo->m_fCastingTime = 0.0f;
	pNewTalentInfo->m_fHitDelay = 0.0f;
	pNewTalentInfo->m_nDodgeDifficulty = 255;
	pNewTalentInfo->m_Resist.m_nDifficulty = 255;
	pNewTalentInfo->m_bCriticalEnable = false;

	return pNewTalentInfo;
}

GTalentInfo* GUTHelper_Talent::NewItemTalentInfo(int nID)
{
	GTalentInfo* pTalentInfo = NewTalentInfo(nID);
	if (NULL == pTalentInfo) return NULL;

	pTalentInfo->m_nSkillType = ST_ITEM;
	pTalentInfo->m_nRank = 1;
	pTalentInfo->m_nTalentType = TT_SKILL;
	pTalentInfo->m_nCategory = TC_COMMON;
	pTalentInfo->m_nTiming = TC_HIT_TALENT;
	pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
	pTalentInfo->m_nEnabledStanceFlag = 0xFFFFFFFF;

	return pTalentInfo;
}

GTalentInfo* GUTHelper_Talent::NewGuardTalentInfo(int nID)
{
	GTalentInfo* pNewTalentInfo = NewTalentInfo(nID);
	pNewTalentInfo->m_nSkillType = ST_GUARD;
	pNewTalentInfo->m_nTiming = TC_GUARD_ME;
	pNewTalentInfo->m_nGuardDirection = GUARD_DIRECTION_ALL;
	pNewTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
	pNewTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;

	return pNewTalentInfo;
}

GTalentInfo* GUTHelper_Talent::MakeDamageTalent( int nDamage, int nTalentID )
{
	GTalentInfo* pPlayerTalentInfo = NULL;

	pPlayerTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	
	if (pPlayerTalentInfo == NULL)
	{
		pPlayerTalentInfo = NewTalentInfo(nTalentID);
		pPlayerTalentInfo->m_bNeedTraining = false;
	}

	SetMeleeTalentInfo(pPlayerTalentInfo, nTalentID);
	AddSimpleTalentInfoHitInfo(pPlayerTalentInfo);
	pPlayerTalentInfo->Cooking();
	pPlayerTalentInfo->RemoveExpiredTime();
	SetTalentDamage(pPlayerTalentInfo, nDamage);
	return pPlayerTalentInfo;
}

void GUTHelper_Talent::SetMeleeTalentInfo( GTalentInfo* pTalentInfo, int nID )
{
	pTalentInfo->m_nID = SUnitTestUtil::NewID(nID);
	pTalentInfo->m_nTiming = TC_HIT_TALENT;
	pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
	pTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
	pTalentInfo->m_nCategory = TC_MELEE;
	pTalentInfo->m_nSkillType = ST_MELEE;	
	pTalentInfo->m_nDodgeDifficulty = 255;
	pTalentInfo->m_Resist.m_nDifficulty = 255;
	pTalentInfo->m_bCriticalEnable = false;
}

void GUTHelper_Talent::SetExtraActiveTalentInfo( GTalentInfo* pTalentInfo, int nID, TALENT_EXTRA_ACTIVE_TYPE nExtraActiveType )
{
	pTalentInfo->m_nID = SUnitTestUtil::NewID(nID);
	pTalentInfo->m_nTalentType = TT_SKILL;
	pTalentInfo->m_nSkillType = ST_EXTRA_ACTIVE;
	pTalentInfo->m_nExtraActive = nExtraActiveType;
}


void GUTHelper_Talent::SetPassiveTalentInfo( GTalentInfo* pTalentInfo, int nID )
{
	pTalentInfo->m_nID = SUnitTestUtil::NewID(nID);
	pTalentInfo->m_nSkillType = ST_PASSIVE;
	pTalentInfo->m_bNeedTraining = true;
}

void GUTHelper_Talent::SetArcheryTalentInfo( GTalentInfo* pTalentInfo, int nID )
{
	pTalentInfo->m_nID = SUnitTestUtil::NewID(nID);
	pTalentInfo->m_nTiming = TC_HIT_TALENT;
	pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
	pTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
	pTalentInfo->m_nCategory = TC_RANGE;
	pTalentInfo->m_nSkillType = ST_ARCHERY;	
	pTalentInfo->m_fCastingTime = 0.0f;
	pTalentInfo->m_nDodgeDifficulty = 255;
	pTalentInfo->m_Resist.m_nDifficulty = 255;
	pTalentInfo->m_bCriticalEnable = false;
}

void GUTHelper_Talent::SetMagicTalentInfo( GTalentInfo* pTalentInfo, int nID )
{
	pTalentInfo->m_nID = SUnitTestUtil::NewID(nID);
	pTalentInfo->m_nCategory = TC_MAGIC;
	pTalentInfo->m_nTalentType = TT_SKILL;
	pTalentInfo->m_nSkillType = ST_MAGIC;
	pTalentInfo->m_fCastingTime = 0.0f;
	pTalentInfo->m_fHitDelay = 0.0f;
	pTalentInfo->m_nDodgeDifficulty = 255;
	pTalentInfo->m_Resist.m_nDifficulty = 255;
	pTalentInfo->m_bCriticalEnable = false;
}

void GUTHelper_Talent::SetMagicMissileTalentInfo( GTalentInfo* pTalentInfo, int nID )
{
	SetMagicTalentInfo(pTalentInfo, nID);

	pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;	
}

void GUTHelper_Talent::SetMagicTargetTalentInfo( GTalentInfo* pTalentInfo, int nID )
{
	SetMagicTalentInfo(pTalentInfo, nID);

	pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;	
}

void GUTHelper_Talent::SetMagicAreaTalentInfo( GTalentInfo* pTalentInfo, int nID )
{
	SetMagicTalentInfo(pTalentInfo, nID);

	pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
	pTalentInfo->m_EffectInfo.m_fRadius = 1.0f;
}

void GUTHelper_Talent::SetExtraPassiveTalentInfo( GTalentInfo* pTalentInfo, int nID, TALENT_EXTRA_PASSIVE_TYPE nExtraPassiveType )
{
	pTalentInfo->m_nID = SUnitTestUtil::NewID(nID);
	pTalentInfo->m_nTalentType = TT_SKILL;
	pTalentInfo->m_nSkillType = ST_EXTRA_PASSIVE;
	pTalentInfo->m_nExtraPassive = nExtraPassiveType;
	pTalentInfo->m_nExtraPassiveParam = 1;
	pTalentInfo->m_bNeedTraining = true;
}

void GUTHelper_Talent::AddTalentEventInfo( GTalentInfo* pTalentInfo, TALENT_EVENT nEvent, float fTime )
{
	CSTalentEventInfo eventinfo;
	eventinfo.m_nEvent = nEvent;
	eventinfo.m_fTime1 = fTime;
	pTalentInfo->m_Events.m_vecEvents.push_back(eventinfo);
}

void GUTHelper_Talent::AddTalentActEventInfo( GTalentInfo* pTalentInfo, TALENT_EVENT nEvent, float fTime )
{
	CSTalentEventInfo eventinfo;
	eventinfo.m_nEvent = nEvent;
	eventinfo.m_fTime1 = fTime;
	pTalentInfo->m_Events.m_vecActEvents.push_back(eventinfo);
}

void GUTHelper_Talent::CancelTalent( GEntityActor* pUser )
{
	VALID(pUser);
	VALID(pUser->GetModuleCombat());
	pUser->GetModuleCombat()->CancelTalent(false);
	pUser->GetModuleCombat()->Update(0.0f);
}

void GUTHelper_Talent::UseTalent(GEntityActor* pUser, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO target_info, bool bCancelCurTalent)
{
	VALID(pUser);
	VALID(pUser->GetField());
	VALID(pUser->GetModuleCombat());

	if (bCancelCurTalent)
	{
		CancelTalent(pUser);
	}

	pUser->GetModuleCombat()->UseTalent(pTalentInfo, target_info, true);
	pUser->GetField()->Update(0.0f);
}

void GUTHelper_Talent::UseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo, MUID uidTarget, bool bCancelCurTalent )
{
	TALENT_TARGET_INFO target_info;
	target_info.uidTarget = uidTarget;
	UseTalent(pUser, pTalentInfo, target_info, bCancelCurTalent);
}

void GUTHelper_Talent::Guard( GEntityActor* pUser )
{
	int nGuardTalentID = SUnitTestUtil::NewID();
	GTalentInfo* pNewTalentInfo = NewTalentInfo(nGuardTalentID);
	pNewTalentInfo->m_nSkillType = ST_GUARD;
	pNewTalentInfo->m_nGuardDirection = GUARD_DIRECTION_ALL;
	pUser->doGuardReleased();
	pUser->doGuard(nGuardTalentID);
}

void GUTHelper_Talent::Train( GEntityPlayer* pPlayer, GTalentInfo* pTalentInfo )
{
	pTalentInfo->m_bNeedTraining = true;

	VALID(pPlayer);
	VALID(pTalentInfo);
	pPlayer->GetTalent().SetTP(1000); // TP 설정
	gsys.pTrainingSystem->Train(pPlayer, pTalentInfo->m_nID);
}

void GUTHelper_Talent::UntrainAll( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	GItem* pUntrainItem = test.item.GiveNewItem(pPlayer);
	pUntrainItem->m_pItemData->m_ItemType = ITEMTYPE_USABLE;
	pUntrainItem->m_pItemData->m_nUsableType = USABLE_TALENT_UNTRAIN;

	gsys.pTrainingSystem->UntrainAll(pPlayer, pUntrainItem);
}

GTalentInfo* GUTHelper_Talent::NewTrainingNeedTalentInfo( int nID/*=INVALID_ID*/, int nTalentLine/*=0*/, int8 nRank/*=0*/ )
{
	GTalentInfo* pTalentInfo = NewTalentInfo(nID, nTalentLine, nRank);

	if (NULL == pTalentInfo) return NULL;

	pTalentInfo->m_bNeedTraining = true;

	return pTalentInfo;
}

void GUTHelper_Talent::TrainEquipUsableAllTalent(GEntityPlayer* pPlayer)
{
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_1H_SLASH);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_1H_BLUNT);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_1H_PIERCE);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_DUAL_PIERCE);

	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_TWO_HANDED);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_2H_BLUNT);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_ARCHERY);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_GUN);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_STAFF);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_WEAPON_BOOK);

	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_ARMOR_LIGHT);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_ARMOR_MEDUIM);
	LearnExtraPassiveTalent(pPlayer, TEPT_BASE_ARMOR_HEAVY);
}

void GUTHelper_Talent::LearnExtraPassiveTalent(GEntityPlayer* pPlayer, TALENT_EXTRA_PASSIVE_TYPE nExtraPassive)
{
	if (false == pPlayer->GetPlayerPassiveTalent().HasEffect(nExtraPassive))
	{		
		GTalentInfo* pTalentInfo = NewExtraPassiveTalentInfo(nExtraPassive);
		Train(pPlayer, pTalentInfo);
	}
}