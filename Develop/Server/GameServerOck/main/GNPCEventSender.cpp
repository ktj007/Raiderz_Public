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
	VALID(NULL!=m_pOwner);
	GField* pField = m_pOwner->GetField();
	if (NULL == pField) return;

	GFieldGrid::EntitySelector* grid_selector = 
		pField->GetGrid().GetEntitySelector();

	class CollectTargetList : public GFieldGrid::Sector::IVisitor
	{
	public:
		CollectTargetList(GNPCEventSender& sender,GField& field, GEntityNPC& owner, const int& range, vector<GEntityNPC*>& npcs)
			: m_sender(sender)
			, m_field(field)
			, m_owner(owner)
			, m_range(range)
			, m_npcs(npcs) {}
	private:
		void OnVisit(const MUID& uid)
		{
			GEntityNPC* const pTarget = m_field.FindNPC(uid);
			if (NULL == pTarget) return;

			if (&m_owner == pTarget) return;
			if (false == m_sender.CheckFaction(&m_owner, pTarget)) return;
			if (false == m_sender.CheckRange(&m_owner, pTarget, m_range)) return;

			m_npcs.push_back(pTarget);
		}
	private:
		GNPCEventSender&		m_sender;
		GField&					m_field;
		GEntityNPC&				m_owner;
		const int&				m_range;
		vector<GEntityNPC*>&	m_npcs;
	};
	grid_selector->VisitByPos(CollectTargetList(*this, *pField, *m_pOwner, nRange, outVecTargetList), ETID_NPC, m_pOwner->GetPos(), 1);
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
