#include "stdafx.h"
#include "TestGame.h"
#include "FBaseNetClient.h"
#include "CCommandResultTable.h"

SUITE(GO_INTO_MAP)
{
	struct FGoIntoMap : public FBaseNetClient
	{
		FGoIntoMap()
		{

		}
		virtual ~FGoIntoMap()
		{

		}
	};
	TEST_FIXTURE(FGoIntoMap, TestNetSendCommand_WorldLoadingComplete)
	{
		TestGame		tg;

		// TODO: 로딩 커맨드 보내는 정책이 바뀜
	//	CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 1);
	}

	TEST_FIXTURE(FGoIntoMap, TestNetHandler_SelMyChar)
	{
		m_pNetAgent->IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		TestGame		tg;

		m_pNetAgent->OnRecv(MC_COMM_RESPONSE_LOGIN_GAME_SERVER, 3, NEW_INT(CR_SUCCESS), NEW_UID(m_pNetClient->GetUID()), NEW_USHORT(0));

		int nFieldID = m_pNetAgent->GetSendParam<int>(0, 0);
		vec3 pos = m_pNetAgent->GetSendParam<vec3>(0, 1);

		CHECK_EQUAL(m_pNetAgent->GetSendCommandID(0), MC_CHAR_GAMESTART);
		CHECK_EQUAL(nFieldID, -1);
		CHECK_CLOSE(pos.x, 0.0f, 0.001f);
		CHECK_CLOSE(pos.y, 0.0f, 0.001f);
		CHECK_CLOSE(pos.z, 0.0f, 0.001f);

		//net_agent.OutputLog("Sample");
	}

}