#include "stdafx.h"
#include "GServerModeChanger.h"
#include "FBasePlayer.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"
#include "FBaseNetClient.h"
#include "MockField.h"
#include "GConfig.h"
#include "CCommandTable.h"
#include "CCommandTable_Master.h"


SUITE(ServerModeChanger)
{
	struct Fixture : public FBasePlayer, FBaseMockLink, FBaseNetClient
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField);
			m_pLink = NewLink(m_pPlayer);

			m_pNetAgent->ResetSendCommands();
		}
		~Fixture()
		{
			m_pField->Destroy();
		}

		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		MockLink*			m_pLink;
		GServerModeChanger	m_ServerModeChanger;
	};

	TEST_FIXTURE(Fixture, TestServerModeChanger_ChangeServerMode_StandAlone)
	{
		bool bOriStandAlone = GConfig::m_bStandAlone;
		GConfig::m_bStandAlone = true;
		CHECK_EQUAL(SERVER_MODE_NORMAL, GConfig::m_nServerMode);

		CSServerMode eServerModeToChange = SERVER_MODE_EXPO;

		/// Act
		m_ServerModeChanger.ChangeServerMode(eServerModeToChange);

		/// Assert
		ASSERT_EQUAL(2, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_NARRATION, m_pLink->GetCommandID(0));

		CHECK_EQUAL(MC_GM_CHANGE_SERVER_MODE, m_pLink->GetCommandID(1));
		CHECK_EQUAL(eServerModeToChange, m_pLink->GetParam<int>(1, 0));
		
		/// Clear
		GConfig::m_bStandAlone = bOriStandAlone;
		GConfig::m_nServerMode = SERVER_MODE_NORMAL;
	}

	TEST_FIXTURE(Fixture, TestServerModeChanger_ChangeServerMode)
	{
		bool bOriStandAlone = GConfig::m_bStandAlone;
		GConfig::m_bStandAlone = false;
		CHECK_EQUAL(SERVER_MODE_NORMAL, GConfig::m_nServerMode);

		CSServerMode eServerModeToChange = SERVER_MODE_EXPO;

		/// Act
		m_ServerModeChanger.ChangeServerMode(eServerModeToChange);

		/// Assert
		ASSERT_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_ADMIN_CHANGE_SERVER_MODE_REQ, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(eServerModeToChange, m_pNetAgent->GetSendParam<int>(0, 0));

		/// Clear
		GConfig::m_bStandAlone = bOriStandAlone;
		GConfig::m_nServerMode = SERVER_MODE_NORMAL;
	}

	TEST_FIXTURE(Fixture, TestServerModeChanger_ChangeServerModeImpl)
	{
		CHECK_EQUAL(SERVER_MODE_NORMAL, GConfig::m_nServerMode);

		CSServerMode eServerModeToChange = SERVER_MODE_EXPO;

		/// Act
		m_ServerModeChanger.ChangeServerModeImpl(eServerModeToChange);

		/// Assert
		ASSERT_EQUAL(2, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_NARRATION, m_pLink->GetCommandID(0));

		CHECK_EQUAL(MC_GM_CHANGE_SERVER_MODE, m_pLink->GetCommandID(1));
		CHECK_EQUAL(eServerModeToChange, m_pLink->GetParam<int>(1, 0));

		/// Clear
		GConfig::m_nServerMode = SERVER_MODE_NORMAL;
	}

}