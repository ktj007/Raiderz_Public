#include "stdafx.h"
#include "GWorld.h"
#include "CCommandTable.h"
#include "GUTHelper.h"
#include "MockLink.h"

SUITE(WorldTest)
{
	TEST(NotSendMyInfoForMoveServer)
	{
		GWorld world;
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer();
		MockLink* pLink = GUTHelper::NewLink(pPlayer);				

		MUID uidPlayer = pPlayer->GetUID();


		world.SendMyInfo(uidPlayer);
		CHECK_EQUAL(MC_CHAR_MYINFO, pLink->GetCommandID(0));

		
		pLink->ResetCommands();
		pPlayer->OnMovedFromOtherGameServer();

		world.SendMyInfo(uidPlayer);
		CHECK_EQUAL(0, pLink->GetCommandCount());
	}
}
