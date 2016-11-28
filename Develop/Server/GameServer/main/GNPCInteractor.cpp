#include "StdAfx.h"
#include "GNPCInteractor.h"
#include "GNPCIETextMaker.h"
#include "GNPCIEApplier.h"
#include "GNPCIEChecker.h"
#include "GEntityNPC.h"
#include "GPlayerInteraction.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "GNPCInteraction.h"
#include "GGlobal.h"
#include "GConditionsSystem.h"
#include "GCommand.h"
#include "GQuestSystem.h"
#include "GInteractionSystem.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "MLocale.h"
#include "GQObjInteractor.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"

GNPCInteractor::GNPCInteractor(void)
{
	m_pNPCIETextmaker = new GNPCIETextMaker;
	m_pNPCIEApplier = new GNPCIEApplier;
	m_pNPCIEChecker = new GNPCIEChecker;
}

GNPCInteractor::~GNPCInteractor(void)
{
	SAFE_DELETE(m_pNPCIETextmaker);
	SAFE_DELETE(m_pNPCIEApplier);
	SAFE_DELETE(m_pNPCIEChecker);
}

bool GNPCInteractor::InteractionByItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, MUID uidUseItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pNPC) return false;

	return Interaction(pPlayer, pNPC, pNPC->GetNPCInfo()->m_InteractionInfo.m_vecNPCIEInfo, ITRT_ITEM, uidUseItem);
}

bool GNPCInteractor::InteractionByClick(GEntityPlayer* pPlayer, GEntityNPC* pNPC, bool bRefreshInteraction)
{
	if (NULL == pPlayer) return false;
	if (NULL == pNPC) return false;

	return Interaction(pPlayer, pNPC, pNPC->GetNPCInfo()->m_InteractionInfo.m_vecNPCIEInfo, ITRT_CLICK, MUID::ZERO, bRefreshInteraction);
}

bool GNPCInteractor::Interaction(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GNPCInteractionElementInfo*>& vecIElementInfo, ITRIGGER_TYPE nITrigType, MUID uidUseItem, bool bRefreshInteraction)
{
	if (NULL == pPlayer) return false;
	if (NULL == pNPC) return false;

	if (false == pPlayer->GetDoing().IsEndableBusy()) return false;
	if (true == pPlayer->GetDoing().IsBusy() && (false == bRefreshInteraction || false == pPlayer->GetDoing().IsNowInteracting()))
	{
		pPlayer->GetDoing().EndDoing();
	}	

	if (false == pNPC->GetNPCInfo()->bInteract) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_INTERACTION_NOT_INTRACTABLE_NPC);
	if (false == pNPC->GetNPCInteraction().IsEnable()) return false;
	if (false == gsys.pInteractionSystem->IsInteractableDistance(pPlayer, pNPC)) return false;	

	vector<GNPCInteractionElementInfo*> vecFilteredIElementInfo = m_pNPCIEChecker->FilterIElementInfo(pPlayer, pNPC, vecIElementInfo, nITrigType, uidUseItem);
	if (true == vecFilteredIElementInfo.empty()) return false;

	pPlayer->GetInteraction().BeginInteraction(nITrigType, ITAT_NPC, pNPC->GetUID(), vecFilteredIElementInfo, uidUseItem);
	pNPC->GetNPCInteraction().Begin(pPlayer);	

	if (true == bRefreshInteraction)
	{
		RouteInteractableIElementRefresh(pPlayer, pNPC->GetUID(), pNPC->GetNPCInfo()->m_strOpeningStatement.c_str(), vecFilteredIElementInfo);
	}
	else
	{
		RouteInteractableIElement(pPlayer, pNPC->GetUID(), pNPC->GetNPCInfo()->m_strOpeningStatement.c_str(), vecFilteredIElementInfo);
	}

	return true;
}

void GNPCInteractor::RouteInteractableIElement(GEntityPlayer* pPlayer, const MUID& uidNPC, const wchar_t* szOpeningText, const vector<GNPCInteractionElementInfo*>& vecIElementInfo)
{
	vector<TD_INTERACTION_ELEMENT> vecTDInteractionElement = MakeTD_INTERACTION_ELEMENT(pPlayer, vecIElementInfo);

	MCommand* pNewCmd = MakeNewCommand(MC_NPCINTERACTION_SELECTABLE_IELEMENT,
		3,
		NEW_UID(uidNPC),
		NEW_WSTR(szOpeningText),
		NEW_BLOB(vecTDInteractionElement));

	pPlayer->RouteToMe(pNewCmd);
}

void GNPCInteractor::RouteInteractableIElementRefresh(GEntityPlayer* pPlayer, const MUID& uidNPC, const wchar_t* szOpeningText, const vector<GNPCInteractionElementInfo*>& vecIElementInfo)
{
	vector<TD_INTERACTION_ELEMENT> vecTDInteractionElement = MakeTD_INTERACTION_ELEMENT(pPlayer, vecIElementInfo);

	MCommand* pNewCmd = MakeNewCommand(MC_NPCINTERACTION_REFRESH_SELECTABLE_IELEMENT,
		3,
		NEW_UID(uidNPC),
		NEW_WSTR(szOpeningText),
		NEW_BLOB(vecTDInteractionElement));

	pPlayer->RouteToMe(pNewCmd);
}

vector<TD_INTERACTION_ELEMENT> GNPCInteractor::MakeTD_INTERACTION_ELEMENT(GEntityPlayer* pPlayer, const vector<GNPCInteractionElementInfo*>& vecIElementInfo)
{
	vector<TD_INTERACTION_ELEMENT> vecTDInteractionElement;
	for each (GNPCInteractionElementInfo* pIElementInfo in vecIElementInfo)
	{
		if (NULL == pIElementInfo) continue;

		wstring strIEText = m_pNPCIETextmaker->Make(pPlayer, pIElementInfo->m_nType, pIElementInfo->GetSinlgeAct());

		TD_INTERACTION_ELEMENT tdInteractionElement;
		tdInteractionElement.nIElementID = pIElementInfo->m_nID;
		tdInteractionElement.nIActType = pIElementInfo->m_nType;
		wcsncpy_s(tdInteractionElement.szIEText, strIEText.c_str(), _TRUNCATE);

		if (pIElementInfo->m_nType == IT_QUEST_END && pIElementInfo->m_vecAct.size() == 1)
		{
			GPlayerQuest* pQuest = pPlayer->GetQuests().Get(pIElementInfo->m_vecAct[0]);
			if (pQuest)
			{
				tdInteractionElement.nQuestState = pQuest->GetState();
			}
		}

		vecTDInteractionElement.push_back(tdInteractionElement);
	}

	return vecTDInteractionElement;
}

bool GNPCInteractor::InteractionElement(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nIElementID)
{
	if (NULL == pPlayer) return false;
	if (NULL == pNPC) return false;

	GNPCInteractionElementInfo* pIElementInfo = pPlayer->GetInteraction().GetSelectableIElementInfo(nIElementID);
	if (NULL == pIElementInfo) return false;
	if (false == gsys.pInteractionSystem->IsInteractableDistance(pPlayer, pNPC)) return false;

	pPlayer->GetInteraction().SelectIElement(*pIElementInfo);
	if (false == m_pNPCIEApplier->Apply(pPlayer, pIElementInfo)) return false;

	return true;
}



bool GNPCInteractor::CheckIProgress(GEntityPlayer* pPlayer, INTERACTION_TYPE nIET)
{
	if (NULL == pPlayer) return false;

	if (true == pPlayer->IsDisableICheck()) return true;

	bool bResult = true;

	GNPCInteractionElementInfo& IElementInfo = pPlayer->GetInteraction().GetSelectedIElementInfo();
	if (IT_NONE == IElementInfo.m_nType)
	{
		bResult = false;
	}

	if (nIET != IElementInfo.m_nType)
	{
		bResult = false;
	}

	if (false == gsys.pConditionsSystem->Check(pPlayer, IElementInfo.m_pConditionsInfo))
	{
		bResult = false;
	}

	if (false == CheckITarget(pPlayer))
	{
		bResult = false;
	}

	if (false == bResult)
	{
		End(pPlayer);
	}

	return bResult;
}

bool GNPCInteractor::CheckITarget(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;

	ITARGET_TYPE nITargetType = pPlayer->GetInteraction().GetITargetType();

	switch (nITargetType)
	{
	case ITAT_NPC:
		{
			GEntityNPC* pNPC = pPlayer->GetInteraction().GetInteractingNPC();
			if (NULL == pNPC) return false;
			if (false == gsys.pInteractionSystem->IsInteractableDistance(pPlayer, pNPC)) return false;
		}
		break;
	case ITAT_ITEM:
		{
			GItem* pItem = pPlayer->GetInteraction().GetInteractingItem();
			if (NULL == pItem) return false;
		}
		break;
	}

	return true;
}

bool GNPCInteractor::End(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;	

	GEntityNPC* pNPC = pPlayer->GetInteraction().GetInteractingNPC();
	if (NULL != pNPC)
	{
		pNPC->GetNPCInteraction().End(pPlayer);
	}

	pPlayer->GetInteraction().EndInteraction();
	gsys.pQuestSystem->GetQObjInteractor().Cancel(pPlayer, pPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct());

	MCommand* pNewCmd = MakeNewCommand(MC_NPCINTERACTION_END, 0, NULL);

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GNPCInteractor::End( GEntityNPC* pNPC )
{
	set<GEntityPlayer*> setInteractingPlayer = pNPC->GetNPCInteraction().GetInteractingPlayer();
	for each (GEntityPlayer* pInteractingPlayer in setInteractingPlayer)
	{		
		End(pInteractingPlayer);
	}

	return true;
}
