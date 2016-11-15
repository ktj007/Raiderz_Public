#include "stdafx.h"
#include "GTalentFactory.h"
#include "GArcheryTalent.h"
#include "GMagicTalent.h"
#include "GMeleeTalent.h"
#include "GItemTalent.h"
#include "GExtraActiveTalent.h"

GTalent* GTalentFactory::NewTalent( GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress ) const
{
	switch (pTalentInfo->m_nSkillType)
	{
	case ST_MELEE: 			return new GMeleeTalent(pOwner, pTalentInfo, Target, bGainStress);	
	case ST_ARCHERY:		return new GArcheryTalent(pOwner, pTalentInfo, Target, bGainStress);
	case ST_MAGIC:			return new GMagicTalent(pOwner, pTalentInfo, Target, bGainStress);
	case ST_EXTRA_ACTIVE:	return NewExtraActiveTalent(pOwner, pTalentInfo, Target, bGainStress);
	case ST_ITEM:			return new GItemTalent(pOwner, pTalentInfo, Target, bGainStress);
	}

//	_ASSERT(0);
	return new GMeleeTalent(pOwner, pTalentInfo, Target, bGainStress);
}

GTalent* GTalentFactory::NewExtraActiveTalent( GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress/*=true*/ ) const
{
	if (pTalentInfo->m_nSkillType == ST_EXTRA_ACTIVE)
	{
		switch (pTalentInfo->m_nExtraActive)
		{
		case TEAT_NPC_ONLY_GRAPPLE: return new GExtraActiveTalent_Grapple(pOwner, pTalentInfo, Target, bGainStress);
		case TEAT_NPC_ONLY_EAT:		return new GExtraActiveTalent_Swallowed(pOwner, pTalentInfo, Target, bGainStress);
		}
	}

	return new GExtraActiveTalent(pOwner, pTalentInfo, Target, bGainStress);
}