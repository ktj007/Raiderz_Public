#include "StdAfx.h"
#include "GNPCIETextMaker.h"
#include "GGlobal.h"
#include "GNPCInteractionInfo.h"
#include "GEntityPlayer.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GDialogInfo.h"
#include "GDialogInfoMgr.h"
#include "GNPCShopInfo.h"
#include "GNPCShopInfoMgr.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GCraftInfo.h"
#include "GCraftInfoMgr.h"
#include "GSoulBindingInfo.h"
#include "GSoulBindingInfoMgr.h"
#include "GConst.h"


wstring GNPCIETextMaker::Make( GEntityPlayer* pPlayer, INTERACTION_TYPE nIEType, int nIEAct )
{
	switch (nIEType)
	{
	case IT_DIALOG :		return Dialog(nIEAct);
	case IT_NPCSHOP :		return NPCShop(nIEAct);
	case IT_QUEST_BEGIN :	return QuestBegin(pPlayer, nIEAct);
	case IT_QUEST_END :		return QuestEnd(pPlayer, nIEAct);
	case IT_CRAFT :			return Craft(nIEAct);
	case IT_INN_SLEEP :		return InnSleep(nIEAct);
	case IT_SOULBINDING :	return SoulBinding(nIEAct);
	case IT_CHALLENGERQUEST:return ChallengeQuest(nIEAct);
	case IT_GUILD_CREATE :	return L"$$INTERACTION_GUILD_CREATE";
	case IT_GUILD_DESTROY:	return L"$$INTERACTION_GUILD_DESTROY";
	default :				return L"";
	}

	return L"";
}

wstring GNPCIETextMaker::Dialog( int nIEAct )
{
	GDialogInfo* pDialogInfo = gmgr.pDialogInfoMgr->Get(nIEAct);
	if (NULL == pDialogInfo) return L"";

	return pDialogInfo->m_strText;
}

wstring GNPCIETextMaker::NPCShop( int nIEAct )
{
	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(nIEAct);
	if (NULL == pNPCShopInfo) return L"";
	GDialogInfo* pDialogInfo = pNPCShopInfo->m_pDialogInfo;
	if (NULL == pNPCShopInfo) return L"";

	return	pDialogInfo->m_strText;
}

wstring GNPCIETextMaker::QuestBegin( GEntityPlayer* pPlayer, int nIEAct )
{
	if (NULL == pPlayer) return L"";

	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nIEAct);
	if (NULL == pQuestInfo) return L"";

	GDialogInfo* pGiveDialogInfo = gmgr.pDialogInfoMgr->Get(pQuestInfo->nGiveDialogID);
	if (NULL == pGiveDialogInfo) return L"";	

	return pGiveDialogInfo->m_strText;
}

wstring GNPCIETextMaker::QuestEnd( GEntityPlayer* pPlayer, int nIEAct )
{
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nIEAct);
	if (NULL == pQuestInfo) return L"";
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(pQuestInfo->nID);
	if (NULL == pPlayerQuest) return L"";

	if (true == pPlayerQuest->IsComplete())
	{		
		GDialogInfo* pRewardDialogInfo = gmgr.pDialogInfoMgr->Get(pQuestInfo->nRewardDialogID);
		if (NULL == pRewardDialogInfo) return L"";

		return pRewardDialogInfo->m_strText;					
	}
	// 퀘스트를 받고 완료하지 못한경우
	else
	{
		GDialogInfo* pIncompleteDialogInfo = gmgr.pDialogInfoMgr->Get(pQuestInfo->nIncompleteDialogID);
		if (NULL == pIncompleteDialogInfo) return L"";

		return pIncompleteDialogInfo->m_strText;
	}
}

wstring GNPCIETextMaker::Craft( int nIEAct )
{
	GCraftInfo* pCraftInfo = gmgr.pCraftInfoMgr->Get(nIEAct);
	if (NULL == pCraftInfo) return L"";
	GDialogInfo* pDialogInfo = gmgr.pDialogInfoMgr->Get(pCraftInfo->m_nDialogID);
	if (NULL == pDialogInfo) return L"";

	return pDialogInfo->m_strText;
}

wstring GNPCIETextMaker::InnSleep( int nIEAct )
{
	GDialogInfo* pDialogInfo = gmgr.pDialogInfoMgr->Get(nIEAct);
	if (NULL == pDialogInfo) return L"";

	return pDialogInfo->m_strText;
}

wstring GNPCIETextMaker::SoulBinding(int nIEAct)
{
	GSoulBindingInfo* pSoulBindingInfo = gmgr.pSoulBindingInfoMgr->Get(nIEAct);
	if (NULL == pSoulBindingInfo) return L"";

	GDialogInfo* pDialogInfo = gmgr.pDialogInfoMgr->Get(pSoulBindingInfo->m_nDialogID);
	if (NULL == pDialogInfo) return L"";

	return pDialogInfo->m_strText;
}

wstring GNPCIETextMaker::ChallengeQuest( int nIEAct )
{
	GDialogInfo* pDialogInfo = gmgr.pDialogInfoMgr->Get(nIEAct);
	if (NULL == pDialogInfo) return L"";

	return pDialogInfo->m_strText;
}
