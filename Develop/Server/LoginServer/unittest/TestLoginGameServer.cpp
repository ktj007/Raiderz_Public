#include "StdAfx.h"
#include "LUnitTestUtil.h"
#include "LTestHelper.h"
#include "MockPlayerObject.h"
#include "LCommandTable.h"
#include "MockLink.h"
#include "LMasterServer.h"
#include "LMasterServerMgr.h"
#include "LGlobal.h"
#include "LDBManager.h"
#include "MockDBManager.h"

SUITE(LoginGameServer)
{
	TEST_FIXTURE(Fixture, CharRequestSelMyChar)
	{
		CHECK_EQUAL(pPlayerObject->GetState(), POS_SELECT_CHAR);

		pLink->OnRecv(MC_COMM_REQUEST_LOGIN, 3, NEW_STR("TEST_ID"), NEW_STR("TEST_PASS"), NEW_INT(SH_COMMAND_VERSION));

		MCommand& SendCommand = pLink->GetCommand(0);

		if (SendCommand.GetID() != MC_COMM_RESPONSE_LOGIN)		{ CHECK(!"wrong command type"); return;}

		int nRet;
		MUID uidPlayer;
		if (SendCommand.GetParameter(&nRet,				0, MPT_INT)==false)	{ CHECK(!"wrong param type"); return; }
		if (SendCommand.GetParameter(&uidPlayer,		1, MPT_UID)==false)	{ CHECK(!"wrong param type"); return; }
		CHECK_EQUAL(nRet, CR_SUCCESS);		// 로그인 성공
		CHECK_EQUAL(uidPlayer, pPlayerObject->GetUID());
	}
}
