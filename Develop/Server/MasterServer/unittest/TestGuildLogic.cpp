#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "SCommandTable.h"
#include "ZGuildLogic.h"
#include "CCommandTable_Master.h"
#include "AStlUtil.h"

SUITE(GuildLogic)
{
	struct Fixture : public FBaseGameServer, FBaseMockLink, FBasePlayer
	{
		Fixture()
		{
			m_pReceiveGameServer = AddGameServerObject();
			m_pReceiveGameServerLink = NewLink(m_pReceiveGameServer->GetUID());
			m_pReceiveGameServerLink->AddIgnoreID(MSC_PROXY_PLAYER_UPDATE);
			
			m_pReceiver = AddNewPlayerInWorld(m_pReceiveGameServer->GetID());
		}

		ZGameServerObject*	m_pReceiveGameServer;
		MockLink*			m_pReceiveGameServerLink;
		ZPlayer*			m_pReceiver;
	};

	TEST_FIXTURE(Fixture, MemberOnlineInfo)
	{
		vector<int> vecMemberCID;
		vecMemberCID.push_back(m_pReceiver->GetCID());
		int nOffLineCID = ZTestHelper::NewCID();

		gsys.pGuildLogic->MemberOnlineInfo(m_pReceiveGameServer->GetUID(), m_pReceiver->GetUID(), vecMemberCID);

		vector<TD_GUILD_ONLINE_MEMBER_INFO> vecOnlineMemberInfo;
		ASSERT_EQUAL(MMC_GUILD_MEMBER_ONLINEINFO, m_pReceiveGameServerLink->GetCommandID(0));
		CHECK_EQUAL(m_pReceiver->GetUID(), m_pReceiveGameServerLink->GetParam<MUID>(0, 0));		
		CHECK_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetBlob(vecOnlineMemberInfo, 1));

		TD_GUILD_ONLINE_MEMBER_INFO onlineMemberInfo = vecOnlineMemberInfo.front();
		CHECK_EQUAL(onlineMemberInfo.nCID, m_pReceiver->GetCID());
		CHECK_EQUAL(onlineMemberInfo.nFieldID, m_pReceiver->GetFieldID());
		CHECK_EQUAL(onlineMemberInfo.nChannelID, m_pReceiver->GetChannelID());
	}
}