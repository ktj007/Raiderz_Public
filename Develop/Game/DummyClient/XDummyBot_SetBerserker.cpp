#include "StdAfx.h"
#include "XDummyBot_SetBerserker.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XDummyLazyCommandPoster.h"
#include "XDummyCollision.h"

XDummyBot_SetBerserker::XDummyBot_SetBerserker(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
{
}

XDummyBot_SetBerserker::~XDummyBot_SetBerserker(void)
{
	
}

void XDummyBot_SetBerserker::OnRun( float fDelta )
{
}

minet::MCommandResult XDummyBot_SetBerserker::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	assert(NULL!=pAgent);
	assert(NULL!=pCommand);
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	assert(NULL!=pClient);

	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		{
			TD_MYINFO vecTDMyInfo;
			vector<int> vecTalent;

			if (pCommand->GetSingleBlob(vecTDMyInfo, 0)==false) return CR_FALSE;
			if (pCommand->GetBlob(vecTalent, 4)==false) return CR_FALSE;

			class RequestTalents : public XTalentInfoMgr::IVisitor
			{
			public:
				RequestTalents(XBirdClient& client,vector<int>& talents) : m_client(client), m_talents(talents) {}
			private:
				void OnVisit(const CSTalentInfo& talentInfo)
				{
					if (TS_BERSERKER != talentInfo.m_nStyle)
						return;

					if (m_talents.end() != find(m_talents.begin(), m_talents.end(), talentInfo.m_nID))
						return;

					wchar_t szTalentID[32];
					_itow(talentInfo.m_nID, szTalentID, 10);

					XBIRDPOSTCMD2(&m_client, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"talent"), MCommandParameterWideString(szTalentID));
				}

			private:
				XBirdClient& m_client;
				vector<int>& m_talents;
			};

			if (3 != vecTDMyInfo.nPlayerGrade)
			{
				XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			}
			
			info.talent->Visit(RequestTalents(*pClient, vecTalent));

			//todo 리소스에서 얻어오는 식으로 변경 해야 함
			for (int talentID = 80010; talentID < 80022; ++talentID)
			{
				if (80017 == talentID)
					continue;

				wchar_t szTalentID[32];
				_itow(talentID, szTalentID, 10);

				if (vecTalent.end() != find(vecTalent.begin(), vecTalent.end(), talentID))
					continue;

				XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"talent"), MCommandParameterWideString(szTalentID));
			}

			pAgent->DelBot(this);
		}
		break;
	}

	return CR_FALSE;
}