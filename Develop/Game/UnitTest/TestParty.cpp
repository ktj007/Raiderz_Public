#include "stdafx.h"
#include "FTestGame.h"
#include "XMyPlayer.h"
#include "CCommandResultTable.h"

SUITE(Party)
{
	class FDuelGame : public FTestGame
	{
	public:
		FDuelGame()
		{

		}

		virtual ~FDuelGame()
		{

		}
	};

	TEST_FIXTURE(FDuelGame, TestParty)
	{
		XMyPlayer* pMyPlayer = GetMyPlayer();
		pMyPlayer->SetPosition(vec3::ZERO);

		XNetPlayer* pNetPlayer = NewNetPlayer(vec3::ZERO);

		CHECK(gvar.MyInfo.Party.IsJoined() == false);
		
		//====================================================================

		TD_PARTY_SETTING tdPartySetting;
		tdPartySetting.m_uidLeader = pMyPlayer->GetUID();

		TD_PARTY PartyInfo(global.system->GenerateLocalUID(), tdPartySetting);
		vector<TD_PARTY_MEMBER> vecMemberInfo;

		TD_PARTY_MEMBER tdMember1;
		tdMember1.m_uidPlayer = pMyPlayer->GetUID();
		vecMemberInfo.push_back(tdMember1);

		TD_PARTY_MEMBER tdMember2;
		tdMember2.m_uidPlayer = pNetPlayer->GetUID();
		vecMemberInfo.push_back(tdMember2);

		vector<vector<int> >	vecMemberBuffID;
		vector<int> vecBuffID;		

		m_pNetAgent->OnRecv(MC_PARTY_REFRESH_INFO_ALL, 7, NEW_BLOB(&PartyInfo, sizeof(TD_PARTY), 1), NEW_BLOB(vecMemberInfo),
			NEW_BLOB(vecBuffID), NEW_BLOB(vecBuffID), NEW_BLOB(vecBuffID), NEW_BLOB(vecBuffID), NEW_BLOB(vecBuffID));

		//====================================================================

		CHECK(gvar.MyInfo.Party.IsJoined() == true);
		CHECK(gvar.MyInfo.Party.IsLeader() == true);

		m_pNetAgent->OnRecv(MC_PARTY_CHANGE_LEADER, 1, NEW_UID(pNetPlayer->GetUID()));

		CHECK(gvar.MyInfo.Party.IsLeader() == false);

	}
}