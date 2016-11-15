#include "stdafx.h"
#include "GPlayerPassiveTalents.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GTalentInfoMgr.h"

void GPlayerPassiveTalent::GainEffect( GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);
	if (!pTalentInfo->IsPassiveSkill())		
		return;
	
	int nTalentLine = pTalentInfo->m_nTalentLine;
	GTalentInfo* pPrevPassiveTalent = GetEffect(nTalentLine);
	if (pPrevPassiveTalent)
	{
		if (pPrevPassiveTalent->m_nRank > pTalentInfo->m_nRank)
			return;		// 기존에 익힌 패시브의 랭크가 더 강하다면 무시

		LostEffect(pPrevPassiveTalent);
	}

	m_mapPassiveTalents.insert(MAP_TALENTS::value_type(nTalentLine, pTalentInfo));


	GainEffectImpl(pTalentInfo, pTalentInfo->m_nExtraPassive, pTalentInfo->m_nExtraPassiveParam);
	GainEffectImpl(pTalentInfo, pTalentInfo->m_nExtraPassive2, pTalentInfo->m_nExtraPassiveParam2);
}

void GPlayerPassiveTalent::LostEffect( GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);
	if (!pTalentInfo->IsPassiveSkill())		
		return;

	LostEffectImpl(pTalentInfo, pTalentInfo->m_nExtraPassive, pTalentInfo->m_nExtraPassiveParam);
	LostEffectImpl(pTalentInfo, pTalentInfo->m_nExtraPassive2, pTalentInfo->m_nExtraPassiveParam2);
}

bool GPlayerPassiveTalent::HasEffect( TALENT_EXTRA_PASSIVE_TYPE nPassiveType ) const
{
	if (true == gmgr.pTalentInfoMgr->IsGivedExtraPassive(nPassiveType))
	{
		return true;
	}

	return m_mapPassiveValues.find(nPassiveType)!=m_mapPassiveValues.end();	
}

bool GPlayerPassiveTalent::HasEffect( TALENT_EXTRA_PASSIVE_TYPE nPassiveType, int nExtraPassiveLevel ) const
{
	if (true == gmgr.pTalentInfoMgr->IsGivedExtraPassive(nPassiveType, nExtraPassiveLevel))
	{
		return true;
	}

	int nEffectValue;
	if (!GetEffectValue(nPassiveType, nEffectValue))	
		return false;

	return (nExtraPassiveLevel <= nEffectValue);
}

void GPlayerPassiveTalent::AddEffect( TALENT_EXTRA_PASSIVE_TYPE nEffectType, int nValue )
{
	VALID(!HasEffect(nEffectType));
	m_mapPassiveValues.insert(MAP_EXTRA_PASSIVE_TYPE::value_type(nEffectType, nValue));
}

void GPlayerPassiveTalent::RemoveEffect( TALENT_EXTRA_PASSIVE_TYPE nEffectType )
{
	VALID(HasEffect(nEffectType));
	m_mapPassiveValues.erase(nEffectType);
}

bool GPlayerPassiveTalent::GetEffectValue( TALENT_EXTRA_PASSIVE_TYPE nEffectType, int& nEffectValue ) const
{
	VALID_RET(nEffectType > TEPT_NONE, false);
	VALID_RET(nEffectType < TEPT_MAX, false);
	if (!HasEffect(nEffectType))	return false;

	MAP_EXTRA_PASSIVE_TYPE::const_iterator itor = m_mapPassiveValues.find(nEffectType);
	if (m_mapPassiveValues.end() == itor)
	{
		nEffectValue = 0;
		return true;
	}
	
	nEffectValue = (*itor).second;
	return true;
}

bool GPlayerPassiveTalent::ModifyEffectValue( TALENT_EXTRA_PASSIVE_TYPE nEffectType, int nEffectValue )
{
	VALID_RET(nEffectType > TEPT_NONE, false);
	VALID_RET(nEffectType < TEPT_MAX, false);
	if (!HasEffect(nEffectType))	return false;
	m_mapPassiveValues[nEffectType] = max(nEffectValue, 0);
	return true;
}


void GPlayerPassiveTalent::ApplyEffect( TALENT_CONDITION nCondition, GEntityActor* pThis, GEntityActor* pVictim )
{
	GTalentEffector::Info info;
	info.pUser = pThis;
	info.pTarget = pVictim;
	info.nTalentCondition = nCondition;
	for each (pair<int,GTalentInfo*> each in m_mapPassiveTalents)
	{
		info.pTalentInfo = each.second;
		m_TalentEffector.ApplyTalent(info);
	}
}

int GPlayerPassiveTalent::GetPassiveValue( TALENT_EXTRA_PASSIVE_TYPE nPassiveType ) const
{
	int nEffectValue;
	if (!GetEffectValue(nPassiveType, nEffectValue))	
		return 0;
	return nEffectValue;
}

GTalentInfo* GPlayerPassiveTalent::GetEffect( int nTalentLine )
{
	MAP_TALENTS::iterator iter = m_mapPassiveTalents.find(nTalentLine);
	if (iter == m_mapPassiveTalents.end())	return NULL;
	return iter->second;
}

int GPlayerPassiveTalent::GetRank( TALENT_EXTRA_PASSIVE_TYPE nPassiveType )
{
	return GetPassiveValue(nPassiveType);
}



void GPlayerPassiveTalent::GainEffectImpl( GTalentInfo* pTalentInfo, TALENT_EXTRA_PASSIVE_TYPE nEffectType, int nExtraPassiveParam )
{
	VALID(pTalentInfo);
	VALID(nEffectType < TEPT_MAX);
	VALID(nEffectType >= TEPT_NONE);
	if (nEffectType == TEPT_NONE)
		return;		// 특수효과가 없는 패시브탤런트

	if (!HasEffect(nEffectType))
	{
		AddEffect(nEffectType, 0);
	}

	int nEffectValue;
	if (!GetEffectValue(nEffectType, nEffectValue))	
		return;

	if (!ModifyEffectValue(nEffectType, nEffectValue+nExtraPassiveParam))	
		return;
}

void GPlayerPassiveTalent::LostEffectImpl(GTalentInfo* pTalentInfo, TALENT_EXTRA_PASSIVE_TYPE nEffectType, int nExtraPassiveParam)
{
	VALID(pTalentInfo);
	if (!pTalentInfo->IsPassiveSkill())		
		return;

	VALID(nEffectType < TEPT_MAX);
	VALID(nEffectType >= TEPT_NONE);

	m_mapPassiveTalents.erase(pTalentInfo->m_nTalentLine);

	if (nEffectType == TEPT_NONE)
		return;		// 특수효과가 없는 패시브탤런트

	if (!HasEffect(nEffectType))
		return;

	int nEffectValue;
	if (!GetEffectValue(nEffectType, nEffectValue))	
		return;

	if (!ModifyEffectValue(nEffectType, nEffectValue-nExtraPassiveParam))	
		return;

	if (IsRemovableEffect(nEffectType))
	{
		RemoveEffect(nEffectType);
	}
}

bool GPlayerPassiveTalent::IsRemovableEffect(TALENT_EXTRA_PASSIVE_TYPE nEffectType)
{
	MAP_EXTRA_PASSIVE_TYPE::iterator itor = m_mapPassiveValues.find(nEffectType);
	if (itor == m_mapPassiveValues.end()) return false;
	if (0 < itor->second) return false;

	return true;
}
