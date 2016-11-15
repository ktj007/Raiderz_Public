#include "StdAfx.h"
#include "GUTHelper_Interaction.h"
#include "SUnitTestUtil.h"
#include "GPlayerInteraction.h"
#include "GNPCInteraction.h"
#include "GEntityPlayer.h"
#include "GInteractionSystem.h"

GNPCInteractionElementInfo* GUTHelper_Interaction::GiveNewIElementInfoForClick(GNPCInfo* pNPCInfo, INTERACTION_TYPE nIET, int nAct)
{
	if (NULL == pNPCInfo) return NULL;

	GNPCInteractionElementInfo* pNewInfo = new GNPCInteractionElementInfo();
	pNewInfo->m_nID = SUnitTestUtil::NewID();
	pNewInfo->m_nType = nIET;
	if (0 != nAct)
	{
		pNewInfo->m_vecAct.push_back(nAct);
	}	

	pNPCInfo->m_InteractionInfo.m_vecNPCIEInfo.push_back(pNewInfo);

	return pNewInfo;
}

GNPCInteractionElementInfo* GUTHelper_Interaction::GiveNewIElementInfoForItem(GNPCInfo* pNPCInfo, int nItemID, INTERACTION_TYPE nIET , int nAct)
{
	if (NULL == pNPCInfo) return NULL;

	GNPCInteractionElementInfo* pNewInfo = new GNPCInteractionElementInfo();
	pNewInfo->m_nID = SUnitTestUtil::NewID();
	pNewInfo->m_nType = nIET;
	pNewInfo->m_nUseItemID = nItemID;
	if (0 != nAct)
	{
		pNewInfo->m_vecAct.push_back(nAct);
	}	

	pNPCInfo->m_InteractionInfo.m_vecNPCIEInfo.push_back(pNewInfo);

	return pNewInfo;
}

void GUTHelper_Interaction::MakeInteractionState( GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo )
{
	vector<GNPCInteractionElementInfo*> vecIElementInfo;
	vecIElementInfo.push_back(pIElementInfo);

	pPlayer->GetInteraction().BeginInteraction(ITRT_CLICK, ITAT_NPC, pNPC->GetUID(), vecIElementInfo);
	pNPC->GetNPCInteraction().Begin(pPlayer);
	pPlayer->GetInteraction().SelectIElement(*pIElementInfo);
}

void GUTHelper_Interaction::SetUninteractableDistacne(GEntityActor* pActor1, GEntityActor* pActor2)
{
	VALID(pActor1);
	VALID(pActor1);

	float fInvalidDistance = gsys.pInteractionSystem->GetInteractableDistance(pActor1, pActor2) + 1;
	vec3 vInvalidPos = vec3(pActor1->GetPos().x+fInvalidDistance, pActor1->GetPos().y, pActor1->GetPos().z);
	pActor2->SetPos(vInvalidPos);
}