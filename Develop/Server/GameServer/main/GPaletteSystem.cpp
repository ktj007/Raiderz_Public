#include "StdAfx.h"
#include "GPaletteSystem.h"
#include "GEntityPlayer.h"
#include "GPalette.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GPlayerTalent.h"
#include "GPaletteItem.h"
#include "GItemManager.h"
#include "GTalentInfoMgr.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GServer.h"
#include "GSTLUtil.h"
#include "GCommand.h"
#include "CSItemHelper.h"
#include "GDBTaskDataPalette.h"


GPaletteSystem::GPaletteSystem()
{
}

GPaletteSystem::~GPaletteSystem()
{
}

uint8 GPaletteSystem::NumAndSlotToIndex(PALETTE_NUM nNum, PALETTE_SLOT nSlot)
{
	return (uint8) CSItemHelper::GetPalleteIndex(nNum, nSlot);

}

void GPaletteSystem::IndexToNumAndSlot(uint8 nIndex, PALETTE_NUM& outnNum, PALETTE_SLOT& outnSlot)
{
	outnNum = (PALETTE_NUM) (nIndex / PALETTESLOT_MAX);
	outnSlot = (PALETTE_SLOT) (nIndex % PALETTESLOT_MAX);
}

bool GPaletteSystem::IsValidNumAndSlot(PALETTE_NUM nNum, PALETTE_SLOT nSlot)
{
	if (false == IsValidNum(nNum)) return false;
	if (false == IsValidSlot(nSlot)) return false;

	return true;
}

bool GPaletteSystem::IsValidNum(PALETTE_NUM nNum)
{
	if (PALETTENUM_1 > nNum || PALETTENUM_MAX <= nNum) return false;

	return true;
}

bool GPaletteSystem::IsValidSlot( PALETTE_SLOT nSlot )
{
	if (PALETTESLOT_1 > nSlot || PALETTESLOT_MAX <= nSlot) return false;

	return true;
}

bool GPaletteSystem::Select(GEntityPlayer* pPlayer, PALETTE_NUM nNum)
{	
	if (false == IsValidNum(nNum)) return false;

	// if (false == gsys.pDBManager->PaletteSelect(pPlayer->GetCID(), nNum)) return false;

	pPlayer->GetPalette().SelectPalette(nNum);

	MCommand* pNewCmd = MakeNewCommand(MC_PALETTE_SELECT,
		1,		
		NEW_UCHAR(nNum));

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GPaletteSystem::SetPrimary(GEntityPlayer* pPlayer, PALETTE_NUM nNum)
{
	if (PALETTENUM_MAX != nNum && false == IsValidNum(nNum)) return false;

	//PALETTE_NUM nSecondaryNum = pPlayer->GetPalette().GetSecondary();
	//if (nSecondaryNum == nNum)
	//{
	//	if (false == gsys.pDBManager->PaletteSetPrimaryAndSecondary(pPlayer->GetCID(), nNum, PALETTENUM_MAX)) return false;
	//}
	//else
	//{
	//	if (false == gsys.pDBManager->PaletteSetPrimaryAndSecondary(pPlayer->GetCID(), nNum, nSecondaryNum)) return false;
	//}	
	
	pPlayer->GetPalette().SetPrimary(nNum);

	MCommand* pNewCmd = MakeNewCommand(MC_PALETTE_SET_PRIMARY,
		1,		
		NEW_UCHAR(nNum));
	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GPaletteSystem::SetSecondary(GEntityPlayer* pPlayer, PALETTE_NUM nNum)
{
	if (PALETTENUM_MAX != nNum && false == IsValidNum(nNum)) return false;

	//PALETTE_NUM nPrimaryNum = pPlayer->GetPalette().GetPrimary();
	//if (nPrimaryNum == nNum)
	//{
	//	if (false == gsys.pDBManager->PaletteSetPrimaryAndSecondary(pPlayer->GetCID(), PALETTENUM_MAX, nNum)) return false;
	//}
	//else
	//{
	//	if (false == gsys.pDBManager->PaletteSetPrimaryAndSecondary(pPlayer->GetCID(), nPrimaryNum, nNum)) return false;
	//}	

	pPlayer->GetPalette().SetSecondary(nNum);

	MCommand* pNewCmd = MakeNewCommand(MC_PALETTE_SET_SECONDARY,
		1,		
		NEW_UCHAR(nNum));
	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GPaletteSystem::PutUp(GEntityPlayer* pPlayer, PALETTE_NUM nNum, PALETTE_SLOT nSlot, PALETTE_ITEM_TYPE nType, int nItemIDorTalentID, bool bDBUpdate)
{
	if (NULL == pPlayer) return false;

	if (false == IsValidNumAndSlot(nNum, nSlot)) return false;

	switch (nType)
	{
	case PIT_ITEM :
		{
			GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemIDorTalentID);
			if (NULL == pItemData) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_INVALID_ITEM_ID);
			if (ITEMTYPE_USABLE != pItemData->m_ItemType) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_PALETTE_PUTUP_ONLY_USABLE_ITEM);
		}
		break;
	case PIT_TALENT :
		{
			GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find((int) nItemIDorTalentID);
			if (NULL == pTalentInfo) return false;
			if (false == pPlayer->GetTalent().IsTrainedTalent(pTalentInfo)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_NOT_LEARNED_TALENT);			
			if (true == pTalentInfo->IsPassiveSkill()) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_PALETTE_PUTUP_ONLY_ACTIVE_TALENT);
		}
		break;
	default:
		{
			_ASSERT(0);
			return false;
		}
		break;
	}

	if (true == bDBUpdate)
	{
		GDBT_PALETTE_SET data(pPlayer->GetAID()
			, pPlayer->GetUID()
			, pPlayer->GetCID()
			, GDBT_PALETTE_SLOT(nNum, nSlot, nItemIDorTalentID, nType));

		if (false == gsys.pDBManager->PalettePutUp(data)) return false;
	}	

	pPlayer->GetPalette().Set(nNum, nSlot, nType, nItemIDorTalentID);

	MCommand* pNewCmd = MakeNewCommand(MC_PALETTE_PUTUP,
		4,		
		NEW_UCHAR(nNum),
		NEW_UCHAR(nSlot),
		NEW_UCHAR(nType),
		NEW_INT(nItemIDorTalentID));

	pPlayer->RouteToMe(pNewCmd);
	
	return true;
}

bool GPaletteSystem::PutDown(GEntityPlayer* pPlayer, PALETTE_NUM nNum, PALETTE_SLOT nSlot, bool bDBUpdate)
{
	if (NULL == pPlayer) return false;

	if (false == IsValidNumAndSlot(nNum, nSlot)) return false;
	if (true == pPlayer->GetPalette().IsEmpty(nNum, nSlot)) return false;

	if (true == bDBUpdate)
	{
		GDBT_PALETTE_RESET data(pPlayer->GetAID()
			, pPlayer->GetUID()
			, pPlayer->GetCID()
			, nNum
			, nSlot);

		if (false == gsys.pDBManager->PalettePutDown(data)) return false;
	}	

	if (!pPlayer->GetPalette().Reset(nNum, nSlot))
		return false;

	vector<PALETTE_NUM> vecNum;
	vecNum.push_back(nNum);

	vector<PALETTE_SLOT> vecSlot;
	vecSlot.push_back(nSlot);

	MCommand* pNewCmd = MakeNewCommand(MC_PALETTE_PUTDOWN,
		2,
		NEW_BLOB(vecNum),
		NEW_BLOB(vecSlot));

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GPaletteSystem::PutDown(GEntityPlayer* pPlayer, int nItemIDorTalentID, bool bDBUpdate)
{
	if (NULL == pPlayer) return false;

	vector<pair<PALETTE_NUM, PALETTE_SLOT>> vecNumAndSlot;
	pPlayer->GetPalette().GetNumAndSlotByItemID(nItemIDorTalentID, vecNumAndSlot);
	if (true == vecNumAndSlot.empty()) return true;

	vector<PALETTE_NUM> vecNum;
	vector<PALETTE_SLOT> vecSlot;

	for each (const pair<PALETTE_NUM,PALETTE_SLOT>& pair in vecNumAndSlot)
	{
		PALETTE_NUM nNum = pair.first;
		PALETTE_SLOT nSlot = pair.second;

		if (false == IsValidNumAndSlot(nNum, nSlot)) return false;
		if (true == pPlayer->GetPalette().IsEmpty(nNum, nSlot)) return false;

		if (true == bDBUpdate)
		{
			GDBT_PALETTE_RESET data(pPlayer->GetAID()
				, pPlayer->GetUID()
				, pPlayer->GetCID()
				, nNum
				, nSlot);

			if (false == gsys.pDBManager->PalettePutDown(data)) return false;
		}

		if (!pPlayer->GetPalette().Reset(nNum, nSlot))
			return false;

		vecNum.push_back(nNum);	
		vecSlot.push_back(nSlot);
	}	

	MCommand* pNewCmd = MakeNewCommand(MC_PALETTE_PUTDOWN,
		2,
		NEW_BLOB(vecNum),
		NEW_BLOB(vecSlot));

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GPaletteSystem::Change(GEntityPlayer* pPlayer, PALETTE_NUM nNum1, PALETTE_SLOT nSlot1, PALETTE_NUM nNum2, PALETTE_SLOT nSlot2)
{
	if (NULL == pPlayer) return false;

	if (false == IsValidNumAndSlot(nNum1, nSlot1)) return false;
	if (false == IsValidNumAndSlot(nNum2, nSlot2)) return false;

	if ((nNum1 == nNum2) && (nSlot1 == nSlot2)) return false;
	if (true == pPlayer->GetPalette().IsEmpty(nNum1, nSlot1) &&
		true == pPlayer->GetPalette().IsEmpty(nNum2, nSlot2)) return false;

	PALETTE_ITEM_TYPE nType1 = pPlayer->GetPalette().GetPaletteItem(nNum1, nSlot1)->m_nType;
	int nItemIDorTalentID1 = pPlayer->GetPalette().GetPaletteItem(nNum2, nSlot1)->m_nItemIDorTalentID;

	PALETTE_ITEM_TYPE nType2 = pPlayer->GetPalette().GetPaletteItem(nNum2, nSlot2)->m_nType;
	int nItemIDorTalentID2 = pPlayer->GetPalette().GetPaletteItem(nNum2, nSlot2)->m_nItemIDorTalentID;

	GDBT_PALETTE_CHANGE data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, GDBT_PALETTE_SLOT(nNum1, nSlot1, nItemIDorTalentID1, nType1)
		, GDBT_PALETTE_SLOT(nNum2, nSlot2, nItemIDorTalentID2, nType2));

	if (false == gsys.pDBManager->PaletteChange(data)) return false;

	pPlayer->GetPalette().Change(nNum1, nSlot1, nNum2, nSlot2);

	MCommand* pNewCmd = MakeNewCommand(MC_PALETTE_CHANGE,
		4,
		NEW_UCHAR(nNum1),
		NEW_UCHAR(nSlot1),
		NEW_UCHAR(nNum2),
		NEW_UCHAR(nSlot2));

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GPaletteSystem::PutDownAllTalent(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;

	pPlayer->GetPalette().ResetTalentSlot();
	MCommand* pNewCmd = MakeNewCommand(MC_PALETTE_PUTDOWN_ALLTALENT, 0,	NULL);

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

void GPaletteSystem::MakeTD_PALETTE(GEntityPlayer* pPlayer, TD_PALETTE& outtdPalette)
{
	if (NULL == pPlayer) return;

	GPalette& Palette = pPlayer->GetPalette();

	outtdPalette.nSelectedNum = Palette.GetSelectedPalette();
	outtdPalette.nPrimaryNum = Palette.GetPrimary();
	outtdPalette.nSecondaryNum = Palette.GetSecondary();	

	for (int i = 0; i < PALETTENUM_MAX; ++i)
	{
		for (int j = 0; j < PALETTESLOT_MAX; ++j)
		{
			PALETTE_NUM nNum = (PALETTE_NUM) i;
			PALETTE_SLOT nSlot = (PALETTE_SLOT) j;
			GPaletteItem* pPaletteItem = Palette.GetPaletteItem(nNum, nSlot);
			if (NULL == pPaletteItem) continue;

			uint8 nIndex = gsys.pPaletteSystem->NumAndSlotToIndex(nNum, nSlot);
			outtdPalette.paletteItems[nIndex].nType = pPaletteItem->m_nType;
			outtdPalette.paletteItems[nIndex].nItemIDorTalentID = pPaletteItem->m_nItemIDorTalentID;		
		}
	}	
}