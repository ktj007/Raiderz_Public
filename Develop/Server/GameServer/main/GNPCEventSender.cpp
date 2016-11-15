#include "StdAfx.h"
#include "GNPCEventSender.h"
#include "GMessageID.h"
#include "GModuleAI.h"
#include "GFactionSystem.h"
#include "GEntityNPC.h"

#include "GField.h"
#include "GGlobal.h"
#include "GRelationChecker.h"

GNPCEventSender::GNPCEventSender(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
}

GNPCEventSender::~GNPCEventSender()
{
}

void GNPCEventSender::SendEvent(GMessage& refMessage, int nRange)
{
	vector<GEntityNPC*> vecTargetList;

	switch(refMessage.m_nID)
	{
	case GMSG_AI_EVT_YELL:
		MakeTargetNPCList_FriendlyFaction(vecTargetList, nRange);
		break;
	}

	for (int i = 0; i < (int)vecTargetList.size(); ++i)
	{
		vecTargetList[i]->GetModuleAI()->Message(refMessage);
	}
}

void GNPCEventSender::MakeTargetNPCList_FriendlyFaction(vector<GEntityNPC*>& outVecTargetList, int nRange)
{
	GField* pField = m_pOwner->GetField();
	if (NULL == pField) return;

	GFieldGrid::EntitySelector* grid_selector = 
		pField->GetGrid().GetEntitySelector();

	GVectorMUID vecEntities;
	grid_selector->GetNPCs(vecEntities.Vector(), m_pOwner->GetPos());

	for each (const MUID& each in vecEntities.Vector())
	{
		GEntityNPC* pTarget = pField->FindNPC(each);
		if (NULL == pTarget) continue;

		if (m_pOwner == pTarget) continue;
		if (false == CheckFaction(m_pOwner, pTarget)) continue;
		if (false == CheckRange(m_pOwner, pTarget, nRange)) continue;

		outVecTargetList.push_back(pTarget);
	}
}

bool GNPCEventSender::CheckFaction(GEntityNPC* pSender, GEntityNPC* pTarget)
{
	GNPCInfo* pSenderInfo = pSender->GetNPCInfo();
	if (NULL == pSenderInfo) return false;

	GRelationChecker relationChecker;

	if (false == relationChecker.IsAlly(m_pOwner, pTarget))
		return false;

	return true;
}

bool GNPCEventSender::CheckRange(GEntityNPC* pSender, GEntityNPC* pTarget, int nRange)
{
	// Entity가 시야에 안보이면 실패
	vec3 vSenderPos = pSender->GetPos();
	vec3 vTargetPos = pTarget->GetPos();
	float fDistance = vSenderPos.DistanceTo(vTargetPos);

	if ((float)nRange < fDistance)
		return false;

	return true;
}
