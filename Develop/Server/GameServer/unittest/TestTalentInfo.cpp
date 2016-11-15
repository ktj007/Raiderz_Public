#include "stdafx.h"
#include "GTalentInfo.h"
#include "GTalentInfoMgr.h"
#include "CSTalentInfoParser.h"
#include "GUTHelper.h"

SUITE(TalentInfo)
{
	TEST(TestWeaponAllowedParser)
	{
		CSTalentInfoWeaponAllowedParser parser;

		{
			CSTalentWeaponAllowdInfo info;
			parser.Parse(L"  ", info);

			CHECK_EQUAL(true, info.m_bAllAllowed);
		}

		{
			CSTalentWeaponAllowdInfo info;
			parser.Parse(L"1hs", info);

			CHECK_EQUAL(false, info.m_bAllAllowed);
			CHECK_EQUAL(true, info.IsAllowed(TALENT_WEAPON_ALLOWED_1HS));
			CHECK_EQUAL(false, info.IsAllowed(TALENT_WEAPON_ALLOWED_1HB));

			CHECK_EQUAL(true, info.IsAllowed(WEAPON_1H_SLASH, false));
			CHECK_EQUAL(true, info.IsAllowed(WEAPON_1H_SLASH, true));
			CHECK_EQUAL(false, info.IsAllowed(WEAPON_STAFF, false));
		}


		{
			CSTalentWeaponAllowdInfo info;
			parser.Parse(L"1hs,1hb", info);

			CHECK_EQUAL(false, info.m_bAllAllowed);
			CHECK_EQUAL(true, info.IsAllowed(TALENT_WEAPON_ALLOWED_1HS));
			CHECK_EQUAL(true, info.IsAllowed(TALENT_WEAPON_ALLOWED_1HB));

			CHECK_EQUAL(true, info.IsAllowed(WEAPON_1H_SLASH, false));
			CHECK_EQUAL(true, info.IsAllowed(WEAPON_1H_BLUNT, true));
		}

		// 순서가 꺼꾸로, ","도 한두칸 띄어서
		{
			CSTalentWeaponAllowdInfo info;
			parser.Parse(L"1hb ,  1hs", info);

			CHECK_EQUAL(false, info.m_bAllAllowed);
			CHECK_EQUAL(true, info.IsAllowed(TALENT_WEAPON_ALLOWED_1HS));
			CHECK_EQUAL(true, info.IsAllowed(TALENT_WEAPON_ALLOWED_1HB));
		}

		// 모든 무기 사용 가능
		{
			CSTalentWeaponAllowdInfo info;
			parser.Parse(L"all, shd", info);

			CHECK_EQUAL(true, info.m_bAllAllowed);
			CHECK_EQUAL(true, info.IsAllowed(TALENT_WEAPON_ALLOWED_1HS));
			CHECK_EQUAL(true, info.IsAllowed(TALENT_WEAPON_ALLOWED_1HB));
		}


		// 방패가 있어야 사용 가능
		{
			CSTalentWeaponAllowdInfo info;
			parser.Parse(L"1hs, shd", info);

			CHECK_EQUAL(false, info.m_bAllAllowed);
			CHECK_EQUAL(false, info.IsAllowed(WEAPON_1H_SLASH, false));
			CHECK_EQUAL(true, info.IsAllowed(WEAPON_1H_SLASH, true));
			CHECK_EQUAL(false, info.IsAllowed(WEAPON_1H_BLUNT, true));
		}

	}


	TEST(TestTalentInfo_ModeClone)
	{
		GTalentInfo* pTalentInfo = new GTalentInfo();
		pTalentInfo->m_nID = 10;
		pTalentInfo->m_szName = L"aaa";

		GTalentInfo* pModeTalentInfo = new GTalentInfo();
		pTalentInfo->m_nID = 10;
		pTalentInfo->m_nMode = 1;

		pTalentInfo->CopyAll(pModeTalentInfo);

		CHECK(pModeTalentInfo->m_szName == L"aaa");

		SAFE_DELETE(pTalentInfo);
		SAFE_DELETE(pModeTalentInfo);
	}

	struct FTalentInfoCooking
	{
		FTalentInfoCooking() {}
		~FTalentInfoCooking() {}
		GTalentInfo talentInfo;
	};

	TEST_FIXTURE(FTalentInfoCooking, TestTalentInfoCooking_Grapple)
	{
		talentInfo.m_nExtraActive = TEAT_NPC_ONLY_GRAPPLE;
		talentInfo.m_fExtraPhaseTime = 1.0f;
		talentInfo.m_nForceMF = MF_GRAPPLED;

		talentInfo.Cooking();

		CHECK(talentInfo.m_nForceMFWeight >= 1000);
	}

	TEST_FIXTURE(FTalentInfoCooking, TestTalentInfoCooking_Mode)
	{
		talentInfo.m_nID = 1;
		talentInfo.m_szName = L"talent1";

		GTalentInfo* pModeTalentInfo = new GTalentInfo();
		pModeTalentInfo->m_nID = 1;
		pModeTalentInfo->m_nMode = 1;
		pModeTalentInfo->m_szName = L"talent1_mode1";

		talentInfo.InsertMode(pModeTalentInfo);

		talentInfo.Cooking();

		CHECK_EQUAL(true, talentInfo.IsCooked());
		CHECK_EQUAL(true, talentInfo.GetMode(1)->IsCooked());

		// mode 탤런트는 부모 탤런트와 이름이 같다.
		CHECK(talentInfo.m_szName == talentInfo.GetMode(1)->m_szName);
	}


	TEST_FIXTURE(FTalentInfoCooking, TestTalentInfoCooking_InitDistanceToLongestPoint_Guard)
	{
		talentInfo.m_nID = 1;
		talentInfo.m_nSkillType = ST_GUARD;
		talentInfo.Cooking();

		CHECK_EQUAL(0.0f, talentInfo.GetDistanceToLongestPoint());
	}

	TEST_FIXTURE(FTalentInfoCooking, TestTalentInfoCooking_InitDistanceToLongestPoint_HitInfo)
	{
		talentInfo.m_nID = 1;
		talentInfo.m_nSkillType = ST_MELEE;

		GUTHelper helper;
		helper.AddHitInfo(&talentInfo, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 500.0f, 0.0f), 50.0f, 0.0f);

		talentInfo.Cooking();

		float MAGIC_DISTANCE_LOD = 0.8f;
		CHECK_CLOSE(50.0f * MAGIC_DISTANCE_LOD, talentInfo.GetDistanceToLongestPoint(), 0.001f);
	}

	TEST_FIXTURE(FTalentInfoCooking, TestTalentInfoCooking_InitDistanceToLongestPoint_With_TalentStartRange)
	{
		talentInfo.m_nID = 1;
		talentInfo.m_nSkillType = ST_MELEE;

		// 이 값이 있으면 GetDistanceToLongestPoint() 는 이 값으로 대체된다.
		talentInfo.m_fTalentStartRange = 900.0f;		

		GUTHelper helper;
		helper.AddHitInfo(&talentInfo, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 500.0f, 0.0f), 50.0f, 0.0f);

		talentInfo.Cooking();

		CHECK_CLOSE(900.0f, talentInfo.GetDistanceToLongestPoint(), 0.001f);
	}

	TEST_FIXTURE(FTalentInfoCooking, TestTalentInfoCooking_InitExpiredTime)
	{
		talentInfo.m_fDurationTime = 2.0f;
		talentInfo.Cooking();

		// 애니메이션 시간이 더 길면 그 값으로 결정
		CHECK_CLOSE(2.0f, talentInfo.GetExpiredTime(), 0.001f);

		GUTHelper helper;
		helper.AddHitInfo(&talentInfo, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 500.0f, 0.0f), 50.0f, 10.0f);

		
		talentInfo.Cooking();

		// 판정 시간이 더 길면 그 값으로 결정
		CHECK_CLOSE(10.001f, talentInfo.GetExpiredTime(), 0.001f);
	}


	TEST_FIXTURE(FTalentInfoCooking, TestTalentInfoCooking_ModeInitDistanceToLongestPoint_HitInfo)
	{
		talentInfo.m_nID = 1;
		talentInfo.m_szName = L"talent1";

		GTalentInfo* pModeTalentInfo = new GTalentInfo();
		pModeTalentInfo->m_nID = 1;
		pModeTalentInfo->m_nMode = 1;
		pModeTalentInfo->m_szName = L"talent1_mode1";

		GUTHelper helper;
		helper.AddHitInfo(pModeTalentInfo, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 500.0f, 0.0f), 50.0f, 0.0f);

		talentInfo.InsertMode(pModeTalentInfo);

		talentInfo.Cooking();

		float MAGIC_DISTANCE_LOD = 0.8f;
		CHECK_CLOSE(50.0f * MAGIC_DISTANCE_LOD, pModeTalentInfo->GetDistanceToLongestPoint(), 0.001f);
	}

	TEST(TestTalentInfoCooking_HitInfoRef)
	{
		GTalentInfoMgr talentInfoMgr;

		GTalentInfo* pTalentInfo1 = new GTalentInfo();
		pTalentInfo1->m_nID = 1;

		GUTHelper helper;
		helper.AddHitInfo(pTalentInfo1, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 500.0f, 0.0f), 50.0f, 0.0f);

		talentInfoMgr.Insert(pTalentInfo1);

		GTalentInfo* pTalentInfo2 = new GTalentInfo();
		pTalentInfo2->m_nID = 2;

		GTalentInfo* pModeTalentInfo = new GTalentInfo();
		pModeTalentInfo->m_nID = 2;
		pModeTalentInfo->m_nMode = 1;
		pTalentInfo2->InsertMode(pModeTalentInfo);

		// 판정 레퍼런스 설정
		pTalentInfo2->m_HitInfo.m_nReferenceID = 1;
		// 모드 탤런트도 판정 레퍼런스 설정
		pModeTalentInfo->m_HitInfo.m_nReferenceID = 1;

		talentInfoMgr.Insert(pTalentInfo2);

		talentInfoMgr.Cooking();

		// 레퍼런스로 설정한 탤런트에도 Cooking후 판정 정보가 복사되었는지 체크
		CHECK_EQUAL(true, pTalentInfo1->IsCooked());

		vector<GTalentInfo*> vecCheckTalentInfos;
		vecCheckTalentInfos.push_back(pTalentInfo2);
		vecCheckTalentInfos.push_back(pModeTalentInfo);

		for (size_t i = 0; i < vecCheckTalentInfos.size(); i++)
		{
			GTalentInfo* pCheckTalentInfo = vecCheckTalentInfos[i];

			CHECK_EQUAL(1, pCheckTalentInfo->m_HitInfo.m_vSegments.size());

			CHECK_EQUAL(50.0f, pCheckTalentInfo->m_HitInfo.m_vSegments[0].m_vCapsules[0].radius);
			CHECK_EQUAL(vec3(0.0f, 0.0f, 0.0f), pCheckTalentInfo->m_HitInfo.m_vSegments[0].m_vCapsules[0].bottom);
			CHECK_EQUAL(vec3(0.0f, 500.0f, 0.0f), pCheckTalentInfo->m_HitInfo.m_vSegments[0].m_vCapsules[0].top);
		}
	}

	TEST(TestTalentInfo_IsNPCTalent)
	{
		GTalentInfo talentInfo;
		
		CHECK_EQUAL(false, talentInfo.IsNPCTalent());


		talentInfo.m_setNPCID.insert(1);
		CHECK_EQUAL(true, talentInfo.IsNPCTalent());
	}

	TEST(TestTalentInfoHelper_IsInitialLearnedExtraPassiveTalent)
	{
		// 처음부터 배운 ExtraPassive 탤런트
		for (int i = 0; i < TEPT_MAX; i++)
		{
			if (i == TEPT_3COMBO ||
				i == TEPT_SWORD_DRAWING ||
				i == TEPT_UPPERCUT ||
				i == TEPT_COUNTER)
			{
				CHECK_EQUAL(true, CSTalentInfoHelper::IsInitialLearnedExtraPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE(i)));
			}
			else
			{
				CHECK_EQUAL(false, CSTalentInfoHelper::IsInitialLearnedExtraPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE(i)));
			}
		}

	}
}