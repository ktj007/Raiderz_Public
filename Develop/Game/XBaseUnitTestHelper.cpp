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
	td_update_cache_player.uid = uidPlayer;
	td_update_cache_player.nUIID = nPlayerUIID;
	td_update_cache_player.vPos = vec3(1000.f, 1000.f, 0.0f);
	td_update_cache_player.svDir = vec3(0.f, 1.f, 0.f);
	td_update_cache_player.nStance = CS_NORMAL;
	td_update_cache_player.nStatusFlag = 0;
	wcsncpy_s(td_update_cache_player.szName, L"bird", _TRUNCATE);

	// feature
	td_update_cache_player.Feature.nSex = SEX_FEMALE;
	td_update_cache_player.Feature.nHair = 0;
	td_update_cache_player.Feature.nFace = 0;
	td_update_cache_player.Feature.nHairColor = 0;
	td_update_cache_player.Feature.nSkinColor = 0;
	td_update_cache_player.Feature.nWeaponSet = 0;
	for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		td_update_cache_player.Feature.nItemID[i] = 0;
	for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		td_update_cache_player.Feature.nItemID_DyedColor[i] = 0;

	return td_update_cache_player;
}
