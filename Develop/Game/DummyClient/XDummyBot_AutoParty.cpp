#include "StdAfx.h"
#include "XDummyBot_AutoParty.h"
#include "XBirdDummyAgent.h"
#include "XDummyLazyCommandPoster.h"

static vec3 vOutPoint(30000,30000,100);
static vec3 vInPoint(8957,5077,100);
static int nAutopartyQuestID = 999022;

XDummyBot_AutoParty::XDummyBot_AutoParty( XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo )
: XDummyBot(pAgent, pAgentInfo)
, m_nBehavior(BEHAVIOR_PREPARE)
{
	m_rgrTick.SetTickTime(0.2f);
	m_rgrTick.Start();
}

XDummyBot_AutoParty::~XDummyBot_AutoParty(void)
{
}

void XDummyBot_AutoParty::OnRun(float fDelta)
{
	__super::OnRun(fDelta);

	if (!m_rgrTick.IsReady(fDelta))
		return;

	if (m_nBehavior != BEHAVIOR_PREPARE)
	{
		m_nBehavior = (BEHAVIOR_TYPE)MMath::RandomNumber(BEHAVIOR_START, BEHAVIOR_MAX-1);
	}
	
	switch(m_nBehavior)
	{
	case BEHAVIOR_PREPARE:
		m_nBehavior = BEHAVIOR_MOVE_GAIN_QUEST;
		XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
		XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_QUEST_RESET_ALL_REQ);
		XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_CLEAR_INVENTORY_REQ);
		break;
	case BEHAVIOR_MOVE_IN:
		MovePosReq(vInPoint);
		XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_AUTOPARTY_ENQUEUE_REQ, MCommandParameterInt(nAutopartyQuestID));
		break;
	case BEHAVIOR_MOVE_OUT:
		MovePosReq(vOutPoint);
		XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_AUTOPARTY_DEQUEUE_REQ, MCommandParameterInt(nAutopartyQuestID));
		break;
	case BEHAVIOR_MOVE_GAIN_QUEST:
		XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_GM_QUEST_GIVE_REQ, MCommandParameterInt(nAutopartyQuestID));
		break;
	case BEHAVIOR_MOVE_DROP_QUEST:
		XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_QUEST_RESET_ALL_REQ);
		XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_CLEAR_INVENTORY_REQ);
		break;
	case BEHAVIOR_PARTY_LEAVE:
		XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_PARTY_LEAVE_REQ);
		break;
	}
}

void XDummyBot_AutoParty::MovePosReq( vec3 vPos )
{
	TD_PC_MOVE mi;
	mi.vTarPos = vPos;
	vec3 vTransDir = vec3(0,1,0);
	vTransDir.z = 0.0f;
	vTransDir.Normalize();
	mi.svDir.SetX(vTransDir.x);
	mi.svDir.SetY(vTransDir.y);
	mi.nFlags = MOVEMENTFLAG_FORWARD;
	XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_ACTION_MOVE_REQ, MCommandParameterSingleBlob(&mi,sizeof(TD_PC_MOVE)));
}
