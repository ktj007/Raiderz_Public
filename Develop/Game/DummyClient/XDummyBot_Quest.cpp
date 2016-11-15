#include "StdAfx.h"
#include "XDummyBot_Quest.h"
#include "XBirdDummyAgent.h"
#include "XDummyLazyCommandPoster.h"
#include "XDummyMaster.h"
#include "XQuestInfo.h"


XDummyBot_Quest::XDummyBot_Quest(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo), m_nPhase(PHASE_INIT), m_nQuestID(0)
{
	wstring strQuestID = m_pAgentInfo->GetValue(L"quest_id");

	const float TIME_TIMEOUT_SEC = 180.0;
	m_rgltTimeout.SetTickTime(TIME_TIMEOUT_SEC);
}

XDummyBot_Quest::~XDummyBot_Quest(void)
{
}

void XDummyBot_Quest::OnRun( float fDelta )
{
	if (m_rgltTimeout.IsReady(fDelta))
	{
		m_pAgent->SetStatusError();

		char szErrorLog[1024] = {0};
		sprintf_s(szErrorLog, 1024, "(%d)(%S)(%d) XDummyBot_Quest::OnRun(), Timeout!!\n", m_nPhase, m_pAgent->GetLoginID(), m_pAgent->GetID());
		mlog(szErrorLog);
	}

	switch (m_nPhase)
	{
	case PHASE_INIT:
		{
			m_rgltTimeout.Start();

			XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_GOD_REQ);
			XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_CLEAR_INVENTORY_REQ);
			XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_QUEST_RESET_ALL_REQ);		
			
			ChangePhase(PHASE_INIT_DONE);
		}
		break;
	case PHASE_GIVE:
		{
			wstring strQuestID = m_pAgentInfo->GetValue(L"quest_id");
			m_nQuestID = _wtoi(strQuestID.c_str());

			XQuestInfo* pSelectedInfo = info.quest->Get(m_nQuestID);
			if (NULL == pSelectedInfo)
			{
				pSelectedInfo = info.quest->GetRandomQuest();
				if (NULL == pSelectedInfo) return;

				m_nQuestID = pSelectedInfo->nID;
			}
			XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_GM_QUEST_GIVE_REQ, MCommandParameterInt(m_nQuestID));

			ChangePhase(PHASE_GIVE_DONE);
		}
		break;
	case PHASE_COMPLETE:
		{
			XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_GM_QUEST_COMPLETE_REQ, MCommandParameterInt(m_nQuestID));

			ChangePhase(PHASE_COMPLETE_DONE);
		}
		break;
	case PHASE_REWARD:
		{
			XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_GM_QUEST_REWARD_REQ, MCommandParameterInt(m_nQuestID));

			ChangePhase(PHASE_REWARD_DONE);
		}
		break;
	}
}

minet::MCommandResult XDummyBot_Quest::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_GM_QUEST_RESET:
		{
			ChangePhase(PHASE_GIVE);
		}
		break;	
	case MC_QUEST_GIVE:
		{			
			ChangePhase(PHASE_COMPLETE);
		}
		break;
	case MC_QUEST_COMPLETE:
		{			
			ChangePhase(PHASE_REWARD);
		}
		break;
	case MC_QUEST_REWARD:
		{
			ChangePhase(PHASE_INIT);
		}
		break;
	}

	return CR_TRUE;
}

void XDummyBot_Quest::ChangePhase( PHASE nPhase )
{
	m_nPhase = nPhase;
}