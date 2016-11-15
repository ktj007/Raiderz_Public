#include "StdAfx.h"
#include "GGmSetMeHandler.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GWorld.h"
#include "GCalculator.h"
#include "GExpCalculator.h"
#include "GDBManager.h"
#include "GPlayerTalent.h"
#include "GMoneySystem.h"
#include "GFatigueSystem.h"
#include "GInnSystem.h"
#include "GStrings.h"
#include "GTrainingSystem.h"
#include "GSTLUtil.h"
#include "GTalentInfoMgr.h"
#include "CSCharHelper.h"
#include "GPlayerLevelUpdater.h"
#include "GCommand.h"
#include "GItemEquiper.h"
#include "GItemAdder.h"
#include "GItemHolder.h"
#include "SLevelTable.h"
#include "GItemRemover.h"
#include "GConst.h"
#include "GPresetInfoMgr.h"
#include "GPresetInfo.h"


bool GGmSetMeHandler::Handle(GEntityPlayer* pPlayer, wstring strType, wstring strValue)
{
	if (NULL == pPlayer) return false;

	strType = GStrings::StringToLower(strType);

	if (IsRunForTest())
	{
		if (strType != L"grade")
		{
			// 개발자가 아닌 사람은 grade만 가능 - 나중에 없어질 예정
			if (CSCharHelper::IsDeveloperGrade(pPlayer->GetPlayerGrade()) == false) return false;
		}
	}

	if (strType == L"preset")
	{
		Preset(pPlayer, strValue);
	}
	else if (strType == L"talent")
	{
		Talent(pPlayer, strValue);
	}
	else
	{
		int nValue = _wtoi(strValue.c_str());

		if (strType == L"level")
		{
			Level(pPlayer, nValue);
		}
		else if (strType == L"tp")
		{
			TP(pPlayer, nValue);
		}
		else if (strType == L"money")
		{
			Money(pPlayer, nValue);
		}
		else if (strType == L"fatigue")
		{		
			Fatigue(pPlayer, nValue);
		}
		else if (strType == L"innroom")
		{		
			InnRoom(pPlayer, nValue);
		}
		else if (strType == L"hp")
		{
			HP(pPlayer, nValue);
		}
		else if (strType == L"grade")
		{
			Grade(pPlayer, nValue);
		}
		else if (strType == L"exp")
		{
			Exp(pPlayer, nValue);
		}
		else
		{
			return false;
		}
	}	

	MUID uidPlayer = pPlayer->GetUID();
	gsys.pWorld->SendMyInfo(uidPlayer);

	return true;
}

void GGmSetMeHandler::Level(GEntityPlayer* pPlayer, int nLevel)
{
	if (NULL == pPlayer) return;

	nLevel = max(1, min(nLevel, MAX_LEVEL));

	GPlayerLevelUpdater levelUpdater(pPlayer);
	levelUpdater.SetLevelForGM(nLevel, 0);
}

void GGmSetMeHandler::TP(GEntityPlayer* pPlayer, int nTP)
{
	if (NULL == pPlayer) return;

	nTP = max(0, min(nTP, 100));

	if (nTP == pPlayer->GetTalent().GetTP()) return;

	pPlayer->GetTalent().SetTP(nTP);

	gsys.pDBManager->GM_CharUpdateTP(
		pPlayer->GetAID(),
		pPlayer->GetPlayerInfo()->nCID, 
		pPlayer->GetTalent().GetTP(),
		pPlayer->GetTalent().GetTotalTP(),
		pPlayer->GetPlayerInfo()->nPlayTimeSec);	
}

void GGmSetMeHandler::Money(GEntityPlayer* pPlayer, int nMoney)
{
	if (NULL == pPlayer) return;

	nMoney = max(0, min(nMoney, MAX_MONEY));

	if (nMoney > pPlayer->GetMoney())
	{
		gsys.pMoneySystem->Increase(pPlayer, nMoney - pPlayer->GetMoney());
	}
	else if (nMoney < pPlayer->GetMoney())
	{
		gsys.pMoneySystem->Decrease(pPlayer, pPlayer->GetMoney() - nMoney);
	}
	else
	{
		gsys.pMoneySystem->RouteUpdateMoney(pPlayer);
	}
}

void GGmSetMeHandler::Fatigue(GEntityPlayer* pPlayer, int nFatigue)
{
	if (NULL == pPlayer) return;

	gsys.pFatigueSystem->SetFatigueByCommand(pPlayer, nFatigue);
}

void GGmSetMeHandler::InnRoom(GEntityPlayer* pPlayer, int nInnRoomID)
{
	if (NULL == pPlayer) return;

	gsys.pInnsystem->SetInnRoom(pPlayer, nInnRoomID);
}

void GGmSetMeHandler::HP(GEntityPlayer* pPlayer, int nHP)
{
	if (NULL == pPlayer) return;

	pPlayer->SetHP(nHP);
}

void GGmSetMeHandler::Grade( GEntityPlayer* pPlayer, int nPlayerGrade )
{
	if (!IsRunForTest())
	{
		return; // 릴리즈모드에서는 사용하지 않음
	}
	
	GPlayerGrade playerGrade = static_cast<GPlayerGrade>(nPlayerGrade);
	pPlayer->SetPlayerGrade(playerGrade);

	gsys.pDBManager->CharUpdatePlayerGrade(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->nCID, static_cast<uint8>(nPlayerGrade));
}

void GGmSetMeHandler::Exp( GEntityPlayer* pPlayer, int nExp )
{
	if (NULL == pPlayer) return;

	int nMaxExp = GExpCalculator::GetLevelUpRequiredXP(pPlayer->GetLevel());

	nExp = min(nExp, nMaxExp);

	GPlayerLevelUpdater levelUpdater(pPlayer);
	levelUpdater.SetLevelForGM(pPlayer->GetLevel(), nExp);
}

void GGmSetMeHandler::Preset(GEntityPlayer* pPlayer, wstring strValue)
{
	if (NULL == pPlayer) return;
	GPresetInfo* pInfo = gmgr.pPresetInfoMgr->Get(GStrings::StringToLower(strValue));
	if (NULL == pInfo) return;

	Level(pPlayer, pInfo->nLevel);

	// 탤런트 학습
	for each (int nTalentID in pInfo->setTalentID)
	{
		gsys.pTrainingSystem->ForceTrain(pPlayer, nTalentID);
	}

	TP(pPlayer, pInfo->nTP);

	// 소유한 아이템 삭제
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	vector<GItem*> vecAllItem;
	pItemHolder->CollectAllItem(SLOTTYPE_EQUIP, vecAllItem);
	pItemHolder->CollectAllItem(SLOTTYPE_INVENTORY, vecAllItem);
	pItemHolder->CollectAllItem(SLOTTYPE_STORAGE, vecAllItem);
	for each (GItem* pItem in vecAllItem)
	{
		if (NULL == pItem) continue;
		gsys.pItemSystem->GetRemover().Remove(pPlayer, pItem->m_nSlotType,  pItem->m_nSlotID, pItem->GetAmount());
	}

	// 아이템 지급 및 장착
	for each (int nItemID in pInfo->setItemID)
	{
		if (false == gsys.pItemSystem->GetAdder().Add(pPlayer, nItemID)) continue;

		GItem* pAddedItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nItemID);
		if (NULL == pAddedItem) continue;

		if (ITEMTYPE_ARMOR != pAddedItem->m_pItemData->m_ItemType &&
			ITEMTYPE_WEAPON != pAddedItem->m_pItemData->m_ItemType) continue;

		gsys.pItemSystem->GetEquiper().Equip(pPlayer, pAddedItem, pAddedItem->m_pItemData->m_nItemSlot);
	}
}

void GGmSetMeHandler::Talent( GEntityPlayer* pPlayer, wstring strTalent )
{
	vector<int> vecTalentID;
	if (strTalent == L"all")
	{
		gmgr.pTalentInfoMgr->GetStyleTalentID(TS_DEFENDER, vecTalentID);
		gmgr.pTalentInfoMgr->GetStyleTalentID(TS_BERSERKER, vecTalentID);
		gmgr.pTalentInfoMgr->GetStyleTalentID(TS_CLERIC, vecTalentID);
	}
	else if (strTalent == L"defender")
	{
		gmgr.pTalentInfoMgr->GetStyleTalentID(TS_DEFENDER, vecTalentID);
	}
	else if (strTalent == L"berserker")
	{
		gmgr.pTalentInfoMgr->GetStyleTalentID(TS_BERSERKER, vecTalentID);
	}
	else if (strTalent == L"cleric")
	{
		gmgr.pTalentInfoMgr->GetStyleTalentID(TS_CLERIC, vecTalentID);
	}
	else if (strTalent == L"license")
	{
		gmgr.pTalentInfoMgr->GetStyleTalentID(TS_LICENSE, vecTalentID);
	}
	else if (strTalent == L"speed")
	{
		// 개발자의 행진곡
		vecTalentID.push_back(2556);
	}
	else
	{
		vecTalentID.push_back(_wtoi(strTalent.c_str()));
	}


	for each (int nTalentID in vecTalentID)
	{
		if (pPlayer->GetTalent().IsTrainedTalent(nTalentID))
			continue;

		gsys.pTrainingSystem->ForceTrain(pPlayer, nTalentID);
	}
}
