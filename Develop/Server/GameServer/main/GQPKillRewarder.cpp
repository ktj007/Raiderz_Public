#include "StdAfx.h"
#include "GQPKillRewarder.h"
#include "GEntityNPC.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GQPKillRewardOwner.h"
#include "GNPCRewarder.h"
#include "GNPCRewardOwner.h"
#include "GModuleBuff.h"
#include "GEntityPlayer.h"
#include "GPlayerPVPArea.h"
#include "GLootModifyHandler.h"
#include "GGlobal.h"
#include "GLootSystem.h"
#include "CSDef_QuestPVP.h"

GQPKillRewarder::GQPKillRewarder(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
}

GQPKillRewarder::~GQPKillRewarder(void)
{
}

GEntityNPC* GQPKillRewarder::Reward()
{
	GField* pField = m_pOwner->GetField();
	if (NULL == pField) return NULL;
	const GFieldInfo* pFieldInfo = pField->GetInfo();
	if (NULL == pFieldInfo) return NULL;

	if (false == m_pOwner->GetPlayerPVPArea().IsLocateAndBelongTeam()) return NULL;
	if (true == m_pOwner->GetQPKillRewardOwner().IsEmpty()) return NULL;
	if (true == m_pOwner->GetModuleBuff()->IsNowDisableQuestPVPDrop()) return NULL;

	int nNPCID = 0;
	switch(m_pOwner->GetPlayerPVPArea().GetTeam())
	{
	case QT_ONE: nNPCID = pFieldInfo->m_nQPVPNPCID1; break;
	case QT_TWO: nNPCID = pFieldInfo->m_nQPVPNPCID2; break;
	default : return NULL;
	}

	SpawnDetailInfo infoDetail;
	infoDetail.lootable.bEnable = true;
	infoDetail.lootable.uidCorpseForPlayer = m_pOwner->GetUID();
	GEntityNPC* pNPC = pField->SpawnNPC(nNPCID, NULL, m_pOwner->GetPos(), vec3(0.0f,1.0f,0.0f), 0, &infoDetail);
	VALID_RET(pNPC, NULL);

	gsys.pLootSystem->GetModifyHandler().RouteInsertLootableNPC(pNPC);

	m_pOwner->GetQPKillRewardOwner().Clear();

	return pNPC;
}