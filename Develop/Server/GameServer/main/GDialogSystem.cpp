#include "StdAfx.h"
#include "GDialogSystem.h"
#include "GDialogInfo.h"
#include "GEntityPlayer.h"
#include "CCommandTable.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"
#include "GTradeSystem.h"
#include "GNPCShopSystem.h"
#include "GTrainingSystem.h"
#include "GLootSystem.h"
#include "GQuestSystem.h"
#include "GMessageID.h"
#include "GDialogInfoMgr.h"
#include "GInteractionSystem.h"
#include "GPlayerInteraction.h"
#include "GCraftSystem.h"
#include "GInnSystem.h"
#include "GGuildSystem.h"
#include "GGlobal.h"
#include "GQuestInfoMgr.h"
#include "GSTLUtil.h"
#include "GCommand.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GNPCInteractor.h"
#include "GNPCShopShower.h"
#include "GPlayerSystem.h"
#include "GItemAddRemoveInfo.h"
#include "GItemAddAndRemoverForInfo.h"
#include "MLocale.h"
#include "GQuestUIShower.h"

GDialogSystem::GDialogSystem(void)
{
}

GDialogSystem::~GDialogSystem(void)
{
}

bool GDialogSystem::Start(GEntityPlayer* pPlayer, int nDialogID)
{
	if (NULL == pPlayer) return false;

	GDialogInfo* pDialogInfo = NULL;
	if (false == Start_Check(pPlayer, nDialogID, pDialogInfo)) return false;

	pPlayer->GetInteraction().BeginDialog(nDialogID, pDialogInfo->m_pDialogSayInfo);

	if (true == pPlayer->GetInteraction().IsDialogExit())
	{
		return Start_Exit(pDialogInfo, pPlayer);
	}
	else
	{
		Start_Route(pDialogInfo, pPlayer);
	}	

	return true;
}

bool GDialogSystem::Start_Check( GEntityPlayer* pPlayer, int nDialogID, GDialogInfo*& outpDialogInfo )
{
	outpDialogInfo = gmgr.pDialogInfoMgr->Get(nDialogID);		
	if (NULL == outpDialogInfo) return false;	

	return true;
}

bool GDialogSystem::Start_Exit(GDialogInfo* pDialogInfo, GEntityPlayer* pPlayer)
{
	if (0 != pDialogInfo->m_nExit)
	{
		if (false == ApplyInteractionElement(pPlayer, pPlayer->GetInteraction().GetDialogID(), pDialogInfo->m_nExit)) return false;			
	}
	else
	{
		// exit이 0일때는 아무 처리도 하지 않고 인터랙션 종료
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
	}

	return true;
}

void GDialogSystem::Start_Route( GDialogInfo* pDialogInfo, GEntityPlayer* pPlayer )
{
	vector<TD_DIALOG_SELECT_NODE> vecTDDialgSelectNode;
	int nSelectID = 0;
	for each (GDialogSelectInfo* pDialogSelectInfo in pDialogInfo->m_pDialogSayInfo->m_vecDialogSelectInfo)
	{
		vecTDDialgSelectNode.push_back(TD_DIALOG_SELECT_NODE(nSelectID, pDialogSelectInfo->m_nTextID));
		nSelectID++;
	}

	MCommand* pNewCmd = MakeNewCommand(MC_DIALOG_START, 
		2,
		NEW_INT(pDialogInfo->m_pDialogSayInfo->m_nTextID),
		NEW_BLOB(vecTDDialgSelectNode));

	pPlayer->RouteToMe(pNewCmd);
}

bool GDialogSystem::Select(GEntityPlayer* pPlayer, int nSelectID)
{
	if (NULL == pPlayer) return false;

	GDialogSelectInfo* pSelectInfo = NULL;
	if (false == Select_Check(pPlayer, nSelectID, pSelectInfo)) return false;	

	pPlayer->GetInteraction().ContinueDialog(pSelectInfo->m_pDialogSayInfo);
	if (true == pPlayer->GetInteraction().IsDialogExit())
	{
		return Select_Exit(pSelectInfo, pPlayer);	}
	else
	{		
		Select_Route(pPlayer);
	}	

	return true;
}

bool GDialogSystem::Select_Check( GEntityPlayer* pPlayer, int nSelectID, GDialogSelectInfo*& outpSelectInfo )
{
	GDialogSayInfo* pSayInfo = pPlayer->GetInteraction().GetDialogSayInfo();
	if (NULL == pSayInfo) return false;
	if (0 > nSelectID || static_cast<int>(pSayInfo->m_vecDialogSelectInfo.size()) <= nSelectID) return false;
	outpSelectInfo = pSayInfo->m_vecDialogSelectInfo[nSelectID];
	if (NULL == outpSelectInfo) return false;

	return true;
}

bool GDialogSystem::Select_Exit( GDialogSelectInfo* pSelectInfo, GEntityPlayer* pPlayer )
{
	if (0 != pSelectInfo->m_nExit)
	{
		if (false == ApplyItemAddRemove(pPlayer, pSelectInfo->m_pItemAddRemoveInfo)) return false;
		if (false == ApplyInteractionElement(pPlayer, pPlayer->GetInteraction().GetDialogID(), pSelectInfo->m_nExit)) return false;
	}
	else
	{
		// exit이 0일때는 아무 처리도 하지 않고 인터랙션 종료
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
	}

	return true;
}

void GDialogSystem::Select_Route(GEntityPlayer* pPlayer)
{
	vector<TD_DIALOG_SELECT_NODE> vecTDDialgSelectNode;
	int nSelectID = 0;
	for each (GDialogSelectInfo* pDialogSelectInfo in pPlayer->GetInteraction().GetDialogSayInfo()->m_vecDialogSelectInfo)
	{
		vecTDDialgSelectNode.push_back(TD_DIALOG_SELECT_NODE(nSelectID, pDialogSelectInfo->m_nTextID));
		nSelectID++;
	}

	MCommand* pNewCmd = MakeNewCommand(MC_DIALOG_SELECT,
		2,
		NEW_INT(pPlayer->GetInteraction().GetDialogSayInfo()->m_nTextID),
		NEW_BLOB(vecTDDialgSelectNode));

	pPlayer->RouteToMe(pNewCmd);
}

bool GDialogSystem::ApplyItemAddRemove(GEntityPlayer* pPlayer, GItemAddRemoveInfo* pItemAddRemoveInfo)
{
	if (NULL == pItemAddRemoveInfo) return true;

	return gsys.pItemSystem->GetAddAndRemoverForInfo().AddAndRemove(pPlayer, pItemAddRemoveInfo);	
}

bool GDialogSystem::ApplyInteractionElement(GEntityPlayer* pPlayer, int nDialogID, int nExit)
{
	GNPCInteractionElementInfo& IElementInfo = pPlayer->GetInteraction().GetSelectedIElementInfo();

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IElementInfo.m_nType))
	{
		return false;
	}

	GEntityNPC* pNPC = pPlayer->GetInteraction().GetInteractingNPC();
	if (NULL != pNPC)
	{
		GetScriptNPC()->OnDialogExit(pNPC->GetDelegator(), pPlayer->GetDelegator(), nDialogID, nExit);
	}	

	switch (IElementInfo.m_nType)
	{	
	case IT_NPCSHOP :		return gsys.pNPCShopSystem->GetShower().Show(pPlayer, IElementInfo.GetSinlgeAct());
	case IT_QUEST_BEGIN :	return gsys.pQuestSystem->GetUIShower().ShowAcceptUI(pPlayer, IElementInfo.GetSinlgeAct());
	case IT_QUEST_END :		return gsys.pQuestSystem->GetUIShower().ShowRewardUI(pPlayer, IElementInfo.GetSinlgeAct());
	case IT_CRAFT :			return gsys.pCraftSystem->Show(pPlayer, IElementInfo.GetSinlgeAct());
	case IT_INN_SLEEP :		return gsys.pInnsystem->BeginSleep(pPlayer);
	case IT_SOULBINDING :	
		{
			bool bResult = gsys.pPlayerSystem->SaveSoulBinding(pPlayer, IElementInfo.GetSinlgeAct());
			gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);

			return bResult;
		}			
	default :
		{
			gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);	
		}break;
	}	

	return true;
}

