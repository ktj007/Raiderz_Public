#include "StdAfx.h"
#include "GPlayerNPCIconSender.h"
#include "GTDMaker.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "CSConditionInfo.h"
#include "GNPCIconSelector.h"

GPlayerNPCIconSender::GPlayerNPCIconSender(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
	m_pIconSelector = new GNPCIconSelector;
}

GPlayerNPCIconSender::~GPlayerNPCIconSender(void)
{
	SAFE_DELETE(m_pIconSelector);
	m_mapSendedNPCIcon.clear();
}

void GPlayerNPCIconSender::RemoveNPCIconData(const vector<MUID>& vecNPCUID)
{
	for each (const MUID& nNPCUID in vecNPCUID)
	{
		m_mapSendedNPCIcon.erase(nNPCUID);
	}
}

void GPlayerNPCIconSender::ClearNPCIconData()
{
	m_mapSendedNPCIcon.clear();
}

void GPlayerNPCIconSender::SendByFindEnemy(GEntityNPC* pNPC)
{
	if (NULL == pNPC) return;

	Send(vector<NPC_ICON>(1, NPC_ICON(pNPC, NIT_COMBAT)), vector<NPC_ICON>());
}

void GPlayerNPCIconSender::SendByPlayerConditionChange(CONDITION_TYPE nConditionType)
{
	GVectorMUID vecNPCUID;
	m_pOwner->GetNeighborSectorNPC(vecNPCUID.Vector());

	GVectorMUID vecEssentialNPCUID;
	CollectEssentialNPCUID(vecEssentialNPCUID.Vector());

	vector<NPC_ICON> vecNPCIcon;
	vector<NPC_ICON> vecEssentialNPCIcon;
	Select(vecNPCUID.Vector(), vecNPCIcon, nConditionType, vecEssentialNPCUID.Vector(), vecEssentialNPCIcon);	
	Send(vecNPCIcon, vecEssentialNPCIcon);
}

void GPlayerNPCIconSender::SendByPlayerIn(const vector<MUID>& vecSetcorNPCUID, bool bFieldChange)
{
	GVectorMUID vecNPCUID;
	vecNPCUID.Vector().insert(vecNPCUID.Vector().end(), vecSetcorNPCUID.begin(), vecSetcorNPCUID.end());

	GVectorMUID vecEssentialNPCUID;
	if (bFieldChange)
	{
		CollectEssentialNPCUID(vecEssentialNPCUID.Vector());
	}	

	vector<NPC_ICON> vecNPCIcon;
	vector<NPC_ICON> vecEssentialNPCIcon;
	Select(vecNPCUID.Vector(), vecNPCIcon, CT_MAX, vecEssentialNPCUID.Vector(), vecEssentialNPCIcon);	
	Send(vecNPCIcon, vecEssentialNPCIcon);
}

void GPlayerNPCIconSender::SendByNPC(GEntityNPC* pNPC)
{
	if (NULL == pNPC) return;

	vector<NPC_ICON> vecNPCIcon;
	Select(vector<MUID>(1, pNPC->GetUID()), vecNPCIcon, CT_MAX);
	Send(vecNPCIcon);
}

void GPlayerNPCIconSender::SendByDisableNPCInteraction(GEntityNPC* pNPC)
{
	if (NULL == pNPC) return;

	Send(vector<NPC_ICON>(1, NPC_ICON(pNPC, NIT_NONE)), vector<NPC_ICON>());
}

void GPlayerNPCIconSender::CollectEssentialNPCUID(vector<MUID>& vecNPCUID)
{
	GField* pField = m_pOwner->GetField();
	if (NULL == pField) return;
	const GFieldInfo* pFieldInfo = pField->GetInfo();
	if (NULL == pFieldInfo) return;

	const set<int>& setQuestNPC = pFieldInfo->CollectQuestNPC();
	for each (int nQuestNPCID in setQuestNPC)
	{
		vector<GEntityNPC*> vecNPC = pField->FindNPCByID(nQuestNPCID);

		for each (GEntityNPC* pNPC in vecNPC)
		{
			vecNPCUID.push_back(pNPC->GetUID());
		}		
	}
}

void GPlayerNPCIconSender::Select(const vector<MUID>& vecNPCUID, vector<NPC_ICON>& vecNPCIcon, CONDITION_TYPE nConditionType, const vector<MUID>& vecEssentialNPCUID, vector<NPC_ICON>& vecEssentialNPCIcon)
{
	vecNPCIcon = m_pIconSelector->Select(m_pOwner, vecNPCUID, nConditionType);
	FilterEssentialNPCIcon(m_pIconSelector->Select(m_pOwner, vecEssentialNPCUID, nConditionType), vecEssentialNPCIcon);	
}

void GPlayerNPCIconSender::Send(const vector<NPC_ICON>& vecNPCIcon, const vector<NPC_ICON>& vecEssentialNPCIcon)
{
	vector<NPC_ICON> vecFilteredNPCIcon;
	vector<NPC_ICON> vecFilteredEssentialNPCIcon;
	FilterSendedIcon(vecNPCIcon, vecFilteredNPCIcon);
	FilterSendedIcon(vecEssentialNPCIcon, vecFilteredEssentialNPCIcon);

	GTDMaker tdMaker;
	vector<TD_NPC_ICON> vecTDNPCIcon = tdMaker.MakeTD_NPCIcon(vecFilteredNPCIcon);
	vector<TD_ESSENTIAL_NPC_ICON> vecTDEssentialNPCIcon = tdMaker.MakeTD_EssentialNPCIcon(vecFilteredEssentialNPCIcon);
	if (vecTDNPCIcon.empty() && vecTDEssentialNPCIcon.empty()) return;

	MCommand* pNewCmd = MakeNewCommand(MC_NPCINTERACTION_ICON,
		2,
		NEW_BLOB(vecTDNPCIcon),
		NEW_BLOB(vecTDEssentialNPCIcon));

	m_pOwner->RouteToMe(pNewCmd);
}

void GPlayerNPCIconSender::FilterSendedIcon(const vector<NPC_ICON>& vecNPCIcon, vector<NPC_ICON>& vecFilteredNPCIcon)
{
	for each (const NPC_ICON& npcIcon in vecNPCIcon)
	{
		map<MUID, NPC_ICON_TYPE>::iterator itor = m_mapSendedNPCIcon.find(npcIcon.m_pNPC->GetUID());
		if (m_mapSendedNPCIcon.end() == itor)
		{
			m_mapSendedNPCIcon.insert(map<MUID, NPC_ICON_TYPE>::value_type(npcIcon.m_pNPC->GetUID(), npcIcon.m_nIcon));
			vecFilteredNPCIcon.push_back(npcIcon);
		}
		else
		{
			if (npcIcon.m_nIcon != itor->second)
			{
				itor->second = npcIcon.m_nIcon;
				vecFilteredNPCIcon.push_back(npcIcon);
			}
		}
	}
}

void GPlayerNPCIconSender::FilterEssentialNPCIcon( const vector<NPC_ICON>& vecNPCIcon, vector<NPC_ICON>& vecFilteredNPCIcon )
{
	for each (const NPC_ICON& npcIcon in vecNPCIcon)
	{
		if (NIT_QUEST_BEGIN == npcIcon.m_nIcon ||
			NIT_QUEST_END == npcIcon.m_nIcon)
		{
			vecFilteredNPCIcon.push_back(npcIcon);
		}
	}
}