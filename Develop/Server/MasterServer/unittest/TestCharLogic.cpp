#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "ZCharLogic.h"
#include "CCommandTable_Master.h"

SUITE(CharLogic)
{
	struct Fixture : public FBaseServerField, public FBasePlayer, public FBaseMockLink
	{
		Fixture()
		{
			m_pReciveGameServer = AddGameServerObject();
			m_pReciveGameServerLink = NewLink(m_pReciveGameServer->GetUID());

			m_pReciver = AddNewPlayerInWorld(m_pReciveGameServer->GetID());

			m_pReciveGameServerLink->ResetCommands();
		}

		~Fixture()
		{

		}

		ZGameServerObject* m_pReciveGameServer;
		MockLink*	m_pReciveGameServerLink;

		ZPlayer* m_pReciver;
	};

	TEST_FIXTURE(Fixture, NPCDieReward)
	{
		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pReciver->GetCID());

		int nAllBeneficiaryCount = static_cast<int>(vecBeneficiaryCID.size()) + 1;
		int nNPCID = ZTestHelper::NewID();

		gsys.pCharLogic->NPCDieReward(vecBeneficiaryCID, nAllBeneficiaryCount, nNPCID);

		ASSERT_EQUAL(MMC_CHAR_NPCDIE_REWARD, m_pReciveGameServerLink->GetCommandID(0));

		vector<int> vecBeneficiaryCID_CMD;
		ASSERT_EQUAL(true, m_pReciveGameServerLink->GetCommand(0).GetBlob(vecBeneficiaryCID_CMD, 0));
		ASSERT_EQUAL(vecBeneficiaryCID.front(), vecBeneficiaryCID_CMD.front());
		CHECK_EQUAL(nAllBeneficiaryCount, m_pReciveGameServerLink->GetParam<int>(0, 1));
		CHECK_EQUAL(nNPCID, m_pReciveGameServerLink->GetParam<int>(0, 2));
	}
}