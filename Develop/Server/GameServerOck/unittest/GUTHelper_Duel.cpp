#include "StdAfx.h"
#include "GUTHelper_Duel.h"
#include "MockDuel.h"
#include "GEntityPlayer.h"
#include "GNPCInfoMgr.h"
#include "GUTHelper_NPC.h"
#include "SUnitTestUtil.h"
#include "GDuelMgr.h"
#include "GSharedField.h"

MockDuel* GUTHelper_Duel::NewMockDuel(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2)
{
	if (NULL == pPlayer1) return NULL;
	if (NULL == pPlayer2) return NULL;
	if (NULL == pPlayer1->GetField()) return NULL;

	GUTHelper_NPC::NewNPCInfo_DeulFlag();

	MUID uidNew = SUnitTestUtil::NewUID();
	MockDuel* pNewMockDuel = new MockDuel(uidNew, pPlayer1->GetField(), pPlayer1, pPlayer2);

	VALID_RET(pPlayer1->GetField(), pNewMockDuel);

	pPlayer1->GetField()->GetDuelMgr()->InsertDuel(uidNew, pNewMockDuel);

	return pNewMockDuel;
}

MockDuel* GUTHelper_Duel::NewMockDuel_Fighting(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2)
{
	MockDuel* pMockDuel = NewMockDuel(pPlayer1, pPlayer2);

	pMockDuel->SetState(DUEL_FIGHTING);

	return pMockDuel;
}