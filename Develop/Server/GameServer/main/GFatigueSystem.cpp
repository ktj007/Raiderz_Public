#include "StdAfx.h"
#include "GFatigueSystem.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "CCommandTable.h"
#include "GInnRoomInfo.h"
#include "GCommand.h"
#include "CSDef_Fatigue.h"


GFatigueSystem::GFatigueSystem()
{
}

GFatigueSystem::~GFatigueSystem()
{
}

void GFatigueSystem::SetFatigue(GEntityPlayer* pPlayer, int nQuantity)
{
	FATIGUE_TYPE nBeforeFatigueType = GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity);

	pPlayer->GetPlayerInfo()->nFatigueQuantity = nQuantity;

	FATIGUE_TYPE nAfterFatigueType = GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity);

	if (nBeforeFatigueType != nAfterFatigueType)
	{
		MCommand* pNewCmd = MakeNewCommand(MC_FATIGUE_UPDATE,
			1,
			NEW_INT(nAfterFatigueType));

		pPlayer->RouteToMe(pNewCmd);
	}
}

void GFatigueSystem::IncFatigue(GEntityPlayer* pPlayer, short nIncQuantity)
{
	if (NULL == pPlayer) return;

	if (BEST_FATIGUE - nIncQuantity < pPlayer->GetPlayerInfo()->nFatigueQuantity)
	{
		SetFatigue(pPlayer, BEST_FATIGUE);
	}
	else
	{
		SetFatigue(pPlayer, pPlayer->GetPlayerInfo()->nFatigueQuantity + nIncQuantity);
	}
}

void GFatigueSystem::DecFatigue(GEntityPlayer* pPlayer, short nDecQuantity)
{
	if (NULL == pPlayer) return;

	if (WORST_FATIGUE + nDecQuantity > pPlayer->GetPlayerInfo()->nFatigueQuantity)
	{
		SetFatigue(pPlayer, WORST_FATIGUE);
	}
	else
	{
		SetFatigue(pPlayer, pPlayer->GetPlayerInfo()->nFatigueQuantity - nDecQuantity);
	}	
}


FATIGUE_TYPE GFatigueSystem::GetFatigueType(short nFatigueQuantity)
{
	if (WORST_FATIGUE >= nFatigueQuantity)
	{
		return FT_TOO_BAD;
	}
	else if (BAD_FATIGUE >= nFatigueQuantity)
	{
		return FT_BAD;
	}
	else if (NORMAL_FATIGUE >= nFatigueQuantity)
	{
		return FT_NORMAL;
	}
	else if (GOOD_FATIGUE >= nFatigueQuantity)
	{
		return FT_GOOD;
	}
	else
	{
		return FT_VERY_GOOD;
	}
}

void GFatigueSystem::SetFatigueByCommand(GEntityPlayer* pPlayer, int nFatiguePercent)
{
	if (NULL == pPlayer) return;

	nFatiguePercent = max(0, min(nFatiguePercent, 100));

	SetFatigue(pPlayer, BEST_FATIGUE * nFatiguePercent / 100);
}

void GFatigueSystem::IncFatigueByBlankTime(GEntityPlayer* pPlayer, int nBlankTime)
{
	if (NULL == pPlayer) return;

	short nIncQuantity = (SHRT_MAX < nBlankTime * INC_FATIGUE_PER_HOUR_BY_BLANKTIME) ? SHRT_MAX : nBlankTime * INC_FATIGUE_PER_HOUR_BY_BLANKTIME;

	IncFatigue(pPlayer, nIncQuantity);
}

void GFatigueSystem::SetFatigueBySleep(GEntityPlayer* pPlayer, INN_ROOM_TYPE nInnRoomGrade)
{
	if (NULL == pPlayer) return;

	switch(nInnRoomGrade)
	{
	case IRT_NORMAL:
		{
			SetFatigue(pPlayer, SET_FATIGUE_ROOM_NORMAL);
		}
		break;
	case IRT_BASIC:
		{
			SetFatigue(pPlayer, SET_FATIGUE_ROOM_SPECIAL);
		}
		break;
	default:
		{
			_ASSERT(0);
		}
	}
}

void GFatigueSystem::DecFatigueByTime(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return;

	GField* pField = pPlayer->GetField();
	if (NULL == pField) return;

	const GFieldInfo* pFieldInfo = pField->GetInfo();
	if (NULL == pFieldInfo) return;
	
	int nDecQuantity = (FIELDTYPE_HUNTING_GROUND == pFieldInfo->m_nType) ? DEC_FATIGUE_PER_MINUTE_ON_HUNTING_GROUND : DEC_FATIGUE_PER_MINUTE_ON_NOT_HUNTING_GROUND;
	int nModAppliedDecQuantity = nDecQuantity * pField->GetFatigueMod();

	DecFatigue(pPlayer, nModAppliedDecQuantity);

	pPlayer->GetDecFatigueRegulator().Reset(false);
}