#include "stdafx.h"
#include "XNaming.h"
#include "XNPCInfo.h"

SUITE(Naming)
{

	TEST(TestNaming_GetPartsMeshName)
	{
		// 플레이어 여자 보통
		wstring strPartsMeshName;

		XNaming::GetPartsMeshName(strPartsMeshName, L"", L"data/model/player/hf/hf.elu", L"potion", true);
		std::transform(strPartsMeshName.begin(), strPartsMeshName.end(), strPartsMeshName.begin(), std::tolower);
		CHECK(strPartsMeshName == wstring(L"data/model/player/hf/hf_potion.elu"));

		// 플레이어 남자 보통
		XNaming::GetPartsMeshName(strPartsMeshName, L"", L"data/model/player/hm/hm.elu", L"soul_primer", true);
		std::transform(strPartsMeshName.begin(), strPartsMeshName.end(), strPartsMeshName.begin(), std::tolower);
		CHECK(strPartsMeshName == wstring(L"data/model/player/hm/hm_soul_primer.elu"));

		// 플레이어 여자가 여성 전용 메쉬
		XNaming::GetPartsMeshName(strPartsMeshName, L"", L"data/model/player/hf/hf.elu", L"potion", false);
		std::transform(strPartsMeshName.begin(), strPartsMeshName.end(), strPartsMeshName.begin(), std::tolower);
		CHECK(strPartsMeshName == wstring(L"data/model/player/hf/potion.elu"));


		// NPC가 여자 플레이어 공용 사용
		XNaming::GetPartsMeshName(strPartsMeshName, L"", L"data/model/NPC/hf_adult/hf_adult.elu", L"soul_primer", true);
		std::transform(strPartsMeshName.begin(), strPartsMeshName.end(), strPartsMeshName.begin(), std::tolower);
		CHECK(strPartsMeshName == wstring(L"data/model/player/hf/hf_soul_primer.elu"));

		// NPC가 남자 플레이어 공용 사용
		XNaming::GetPartsMeshName(strPartsMeshName, L"", L"data/model/NPC/hm_adult/hm_adult.elu", L"soul_primer", true);
		std::transform(strPartsMeshName.begin(), strPartsMeshName.end(), strPartsMeshName.begin(), std::tolower);
		CHECK(strPartsMeshName == wstring(L"data/model/player/hm/hm_soul_primer.elu"));

		// NPC가 NPC 메쉬 사용
		XNaming::GetPartsMeshName(strPartsMeshName, L"data/model/NPC/hf_kid_noble/", L"data/model/NPC/hf_kid_commoner/hf_kid_commoner.elu", L"hf_kid_noble", false);
		std::transform(strPartsMeshName.begin(), strPartsMeshName.end(), strPartsMeshName.begin(), std::tolower);
		CHECK(strPartsMeshName == wstring(L"data/model/npc/hf_kid_noble/hf_kid_noble.elu"));

	}

	TEST(TestNaming_ModeAnimationNameHeader_NpcInfoIsNULL)
	{
		CHECK(wstring(L"") == XNaming::ModeAnimationNameHeader(NPC_MODE_DEFAULT, NULL));
		CHECK(wstring(L"mode1_") == XNaming::ModeAnimationNameHeader(NPC_MODE_1, NULL));
	}

	TEST(TestNaming_ModeAnimationNameHeader_NoPrefix)
	{
		XNPCInfo npcInfo;

		CHECK(wstring(L"") ==XNaming::ModeAnimationNameHeader(NPC_MODE_DEFAULT, &npcInfo));
		CHECK(wstring(L"mode1_") == XNaming::ModeAnimationNameHeader(NPC_MODE_1, &npcInfo));
		CHECK(wstring(L"mode2_") == XNaming::ModeAnimationNameHeader(NPC_MODE_2, &npcInfo));
		CHECK(wstring(L"mode3_") == XNaming::ModeAnimationNameHeader(NPC_MODE_3, &npcInfo));
	}

	TEST(TestNaming_ModeAnimationNameHeader_DefaultModePrefix)
	{
		XNPCInfo npcInfo;
		npcInfo.strAniPrefix = L"prefix_default";

		CHECK(wstring(L"prefix_default") == XNaming::ModeAnimationNameHeader(NPC_MODE_DEFAULT, &npcInfo));
	}

	TEST(TestNaming_PlayerAnimationNameHeader_NormalStance)
	{
		CHECK(wstring(L"none_") == XNaming::PlayerAnimationNameHeader(WEAPON_NONE, CS_NORMAL));
		CHECK(wstring(L"none_") == XNaming::PlayerAnimationNameHeader(WEAPON_1H_SLASH, CS_NORMAL));
	}

	TEST(TestNaming_PlayerAnimationNameHeader_BattleStance)
	{
		CHECK(wstring(L"none_") == XNaming::PlayerAnimationNameHeader(WEAPON_NONE, CS_BATTLE));
		CHECK(wstring(L"1hs_") == XNaming::PlayerAnimationNameHeader(WEAPON_1H_SLASH, CS_BATTLE));
		CHECK(wstring(L"1hb_") == XNaming::PlayerAnimationNameHeader(WEAPON_1H_BLUNT, CS_BATTLE));
		CHECK(wstring(L"1hp_") == XNaming::PlayerAnimationNameHeader(WEAPON_1H_PIERCE, CS_BATTLE));
		CHECK(wstring(L"2hd_") == XNaming::PlayerAnimationNameHeader(WEAPON_TWO_HANDED, CS_BATTLE));
		CHECK(wstring(L"sta_") == XNaming::PlayerAnimationNameHeader(WEAPON_STAFF, CS_BATTLE));
		CHECK(wstring(L"arc_") == XNaming::PlayerAnimationNameHeader(WEAPON_ARCHERY, CS_BATTLE));
		CHECK(wstring(L"2hb_") == XNaming::PlayerAnimationNameHeader(WEAPON_2H_BLUNT, CS_BATTLE));
	}

}