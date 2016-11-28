#include "stdafx.h"
#include "XBaseUnitTestHelper.h"
#include "XUnitTestUtil.h"

TD_UPDATE_CACHE_NPC XBaseUnitTestHelper::Make_TD_UPDATE_CACHE_NPC( MUID uidNPC, int nNPCID, vec3 vPos, vec3 vDir, uint32 nStatusFlag/*=0*/ )
{
	UIID nUIID = XUnitTestUtil::NewUIID();

	TD_UPDATE_CACHE_NPC td_update_cache_npc;
	td_update_cache_npc.uid = uidNPC;
	td_update_cache_npc.nUIID = nUIID;
	td_update_cache_npc.nNPCID = nNPCID;
	td_update_cache_npc.vPos = vPos;
	td_update_cache_npc.svDir = vDir;
	td_update_cache_npc.nStatusFlag = nStatusFlag;

	return td_update_cache_npc;
}


TD_UPDATE_CACHE_PLAYER XBaseUnitTestHelper::MakeSamle_TD_UPDATE_CACHE_PLAYER(MUID uidPlayer)
{
	UIID nPlayerUIID = XUnitTestUtil::NewUIID();

	TD_UPDATE_CACHE_PLAYER td_update_cache_player;
	td_update_cache_player.SimpleInfo.uid = uidPlayer;
	td_update_cache_player.SimpleInfo.nUIID = nPlayerUIID;
	td_update_cache_player.SimpleInfo.vPos = vec3(1000.f, 1000.f, 0.0f);
	td_update_cache_player.SimpleInfo.svDir = vec3(0.f, 1.f, 0.f);
	td_update_cache_player.SimpleInfo.nStance = CS_NORMAL;
	td_update_cache_player.SimpleInfo.nStatusFlag = 0;
	wcsncpy_s(td_update_cache_player.ExtraInfo.szName, L"bird", _TRUNCATE);

	// feature
	return td_update_cache_player;
}
