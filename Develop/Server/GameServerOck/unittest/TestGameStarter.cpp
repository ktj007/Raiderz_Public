#include "stdafx.h"
#include "GGameStarter.h"
#include "TestLogin.h"
#include "GServer.h"
#include "GUTHelper_CmdHandler.h"

SUITE(GameStarter)
{
	class FGameStarter : public Login::FLogin
	{
	public:
		FGameStarter() 
		{
			m_uidPlayer = SUnitTestUtil::NewUID();
			m_pPlayerObject = gmgr.pPlayerObjectManager->NewPlayerObject(m_uidPlayer);
			gmgr.pPlayerObjectManager->AddPlayer(m_pPlayerObject);

			m_pPlayerLink = NewLink(m_uidPlayer);
		}
		virtual ~FGameStarter() 
		{

		}

		MUID			m_uidPlayer;
		GPlayerObject*	m_pPlayerObject;
		MockLink*		m_pPlayerLink;

		MockLink*		m_pMasterServerLink;
	};

	TEST_FIXTURE(FGameStarter, TestGameStarter_Start_RouteGateAsk)
	{
		/// Fixture 설치
		GEntityPlayer* pEntityPlayer = m_pPlayerObject->GetEntity();
		int nReqFieldID = m_nSharedFieldID;
		vec3 vReqPos = vec3::ZERO;

		/// SUT 실행
		GGameStarter gameStarter;
		gameStarter.Start(m_uidPlayer, nReqFieldID, vReqPos);

		/// 검증
		CHECK_EQUAL(GGameStarter::RESULT_SUCCESS, gameStarter.GetResult());
		CHECK_EQUAL(MMC_GATE_ASK_SHARED_FIELD, m_pNetAgent->GetSendCommandID(0));
	}

	TEST_FIXTURE(FGameStarter, TestGameStarter_Start_Recv_GateRetry)
	{
		/// Fixture 설치
		GEntityPlayer* pEntityPlayer = m_pPlayerObject->GetEntity();
		int nReqFieldID = m_nSharedFieldID;
		vec3 vReqPos = vec3::ZERO;

		GGameStarter gameStarter;
		gameStarter.Start(m_uidPlayer, nReqFieldID, vReqPos);

		int nChannelID = 1;
		TD_GATE_INFO tdGateInfo;
		tdGateInfo.nFieldID = nReqFieldID;
		tdGateInfo.nChannelID = nChannelID;
		tdGateInfo.bDynamicField = false;
		tdGateInfo.vPosition = vReqPos;
		tdGateInfo.vDirection = vec3(0, 0, 1);

		/// SUT 실행
		GUTHelper_CmdHandler::OnRecv_MMC_GATE_RES(m_pNetAgent, m_uidPlayer, RGT_EXECUTE, &tdGateInfo);
		pEntityPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

		/// 검증
		GField* pField = pEntityPlayer->GetField();
		ASSERT_CHECK(pField != NULL);
		CHECK_EQUAL(nReqFieldID, pField->GetID());
	}

	TEST_FIXTURE(FGameStarter, TestGameStarter_Start_Negative)
	{

	}
}