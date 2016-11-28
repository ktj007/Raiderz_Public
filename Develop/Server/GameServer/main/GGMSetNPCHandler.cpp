#include "StdAfx.h"
#include "GGMSetNPCHandler.h"
#include "GStrings.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GBirdMonitor.h"

bool GGMSetNPCHandler::Handle( GEntityPlayer* pPlayer, wstring strType, wstring strValue )
{
	VALID_RET(pPlayer, false);

	strType = GStrings::StringToLower(strType);

	int nValue = _wtoi(strValue.c_str());

	if (strType == L"hp")
	{
		HP(pPlayer, nValue);
	}	
	else
	{
		return false;
	}
	
	return true;
}

void GGMSetNPCHandler::HP( GEntityPlayer* pPlayer, int nPercent )
{
	VALID(pPlayer);
	UIID nNPCUIID = BirdM().GetTargetNPC(pPlayer->GetUIID());
	GEntityNPC* pNPC = pPlayer->FindNPC(nNPCUIID);
	if (NULL == pNPC) return;

	int nHP = static_cast<int>(pNPC->GetMaxHP() * GMath::NumberToFloatPercent(nPercent));

	pNPC->SetHP(nHP);
}