#include "StdAfx.h"
#include "XDummyBot_GetMaxQuest.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XDummyLazyCommandPoster.h"
#include "XDummyCollision.h"
#include "XQuestInfo.h"

namespace
{
	typedef vector<TD_PLAYERQUEST> QUESTS;
}

XDummyBot_GetMaxQuest::XDummyBot_GetMaxQuest(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
{
}

XDummyBot_GetMaxQuest::~XDummyBot_GetMaxQuest(void)
{
	
}

void XDummyBot_GetMaxQuest::OnRun( float fDelta )
{
}

minet::MCommandResult XDummyBot_GetMaxQuest::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	assert(NULL!=pAgent);
	assert(NULL!=pCommand);
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	assert(NULL!=pClient);

	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		OnCommand_MC_CHAR_MYINFO(*pAgent, *pClient, *pCommand);
		pAgent->DelBot(this);
		break;
	}

	return CR_FALSE;
}

void XDummyBot_GetMaxQuest::OnCommand_MC_CHAR_MYINFO(XBirdDummyAgent& agent,XBirdClient& client,MCommand& cmd)
{
	QUESTS quests;

	if (false == cmd.GetBlob(quests, 5))
	{
		mlog("%s > XDummyBot_GetMaxQuest::OnCommand_MC_CHAR_MYINFO() - 퀘스트 추출 실패!", agent.GetName());
		return;
	}

	const int needCnt = MAX_QUEST_INSTACNE_COUNT - quests.size();
	if (0 >= needCnt)
	{
		return;
	}

	class AddQUest : public XQuestInfoMgr::IVisitor
	{
	public:
		AddQUest(XBirdClient& client,QUESTS& quests,const int& needCnt)
			: m_client(client), m_quests(quests), m_needCnt(needCnt)
		{
			assert(0 < m_needCnt);
		}

	private:
		void OnVisit(const CSQuestInfo& questInfo,bool& continueVisit)
		{
			for each (const TD_PLAYERQUEST& quest in m_quests)
			{
				if (quest.nQuestID == questInfo.nID)
				{
					return;
				}
			}

			XBIRDPOSTCMD1(&m_client, MC_GM_QUEST_GIVE_REQ, MCommandParameterInt(questInfo.nID));

			--m_needCnt;
			if (0 >= m_needCnt)
			{
				continueVisit = false;
			}
		}

		XBirdClient&	m_client;
		QUESTS&			m_quests;
		int				m_needCnt;
	};

	XBIRDPOSTCMD2(&client, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
	info.quest->Visit(AddQUest(client, quests, needCnt));
}
