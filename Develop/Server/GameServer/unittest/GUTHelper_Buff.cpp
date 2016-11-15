#include "StdAfx.h"
#include "GUTHelper_Buff.h"
#include "GUTHelper_Talent.h"
#include "GBuffInfo.h"
#include "SUnitTestUtil.h"
#include "GGlobal.h"
#include "GEntityActor.h"
#include "GModuleBuff.h"

void GUTHelper_Buff::SetBuffTalentInfo( GTalentInfo* pTalentInfo, int nID, CSBuffEnchantInfo* pBuffEnchantInfo )
{
	VALID(pBuffEnchantInfo);	// 버프정보가 반드시 필요하다.

	GUTHelper_Talent helper_talent;
	helper_talent.SetMagicTalentInfo(pTalentInfo, nID);

	pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
	pBuffEnchantInfo->nEnchantEvent = TC_USE_TALENT;
	pTalentInfo->m_Buff1 = *pBuffEnchantInfo;
}

void GUTHelper_Buff::SetBuffInfo( GBuffInfo* pInfo, int nID )
{
	pInfo->m_nID = SUnitTestUtil::NewID(nID);
	pInfo->m_nType = BUFT_BUFF;
	pInfo->m_nStackSlot = pInfo->m_nID;
	pInfo->m_nStackPower = 1;
	pInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
	pInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
}

GBuffInfo* GUTHelper_Buff::NewBuffInfo( int nID )
{
	if (nID != INVALID_BUFF_ID &&
		gmgr.pBuffInfoMgr->Exist(nID))
	{
		return gmgr.pBuffInfoMgr->Get(nID);	// 이미 등록된 버프라면 해당 버프를 반환
	}
	GBuffInfo* pNewBuffInfo = new GBuffInfo();
	SetBuffInfo(pNewBuffInfo, nID);
	gmgr.pBuffInfoMgr->Insert(pNewBuffInfo);
	return pNewBuffInfo;
}

GTalentInfo* GUTHelper_Buff::NewBuffTalentInfo( int nID, CSBuffEnchantInfo* pBuffEnchantInfo )
{
	GUTHelper_Talent helper_talent;
	GTalentInfo* pNewTalentInfo = helper_talent.NewTalentInfo(nID);
	SetBuffTalentInfo(pNewTalentInfo, pNewTalentInfo->m_nID, pBuffEnchantInfo);
	return pNewTalentInfo;
}


GTalentInfo* GUTHelper_Buff::NewBuffTalent(GBuffInfo*& pBuff, 
										float fDuration/*=BUFF_DURATION_INFINITY */,
										int nTalentID/*=INVALID_TALENT_ID*/, 
										int nBuffID/*=INVALID_BUFF_ID */ )
{
	// 버프 만들기
	if (!pBuff)
	{
		pBuff = NewBuffInfo(nBuffID);
	}
	VALID_RET(pBuff, NULL);
	
	// 버프 탤런트 만들기
	if (fDuration != -1.0f)
	{
		pBuff->m_fDuration = fDuration;
	}
	
	CSBuffEnchantInfo infoEnchant; 
	infoEnchant.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
	infoEnchant.EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
	infoEnchant.nID = pBuff->m_nID;
	return NewBuffTalentInfo(nTalentID, &infoEnchant);
}

GTalentInfo* GUTHelper_Buff::NewBuffTalent(GBuffInfo*& pBuff, CSBuffEnchantInfo* pBuffEnchantInfo)
{
	VALID_RET(pBuffEnchantInfo, NULL);

	// 버프 만들기
	if (!pBuff)
	{
		pBuff = NewBuffInfo(pBuffEnchantInfo->nID);
	}
	VALID_RET(pBuff, NULL);

	// 버프 탤런트 만들기
	return NewBuffTalentInfo(INVALID_TALENT_ID, pBuffEnchantInfo);
}

void GUTHelper_Buff::GainBuffSelf( GEntityActor* pUser, GBuffInfo* pBuff, float fDuration)
{
	VALID(pUser);

	GTalentInfo* pBuffTalent= NewBuffTalent(pBuff, fDuration);

	// 버프 탤런트 시전
	GUTHelper_Talent helper_talent;

	TALENT_TARGET_INFO target;
	target.uidTarget = pUser->GetUID();
	helper_talent.UseTalent(pUser, pBuffTalent, target);
}

void GUTHelper_Buff::GainBuffSelf( GEntityActor* pUser, GBuffInfo* pBuff, CSBuffEnchantInfo* pBuffEnchantInfo)
{
	VALID(pUser);

	GTalentInfo* pBuffTalent= NewBuffTalent(pBuff, pBuffEnchantInfo);

	// 버프 탤런트 시전
	GUTHelper_Talent helper_talent;

	TALENT_TARGET_INFO target;
	target.uidTarget = pUser->GetUID();
	helper_talent.UseTalent(pUser, pBuffTalent, target);
}

void GUTHelper_Buff::LostBuff( GEntityActor* pUser, int nBuffID)
{
	VALID(pUser);
	pUser->LostBuff(nBuffID);
	pUser->GetModuleBuff()->Update(0.0f);
}

void GUTHelper_Buff::LostBuffStack( GEntityActor* pUser, int nBuffIDSlot )
{
	VALID(pUser);
	pUser->LostBuffStack(nBuffIDSlot);
	pUser->GetModuleBuff()->Update(0.0f);
}
