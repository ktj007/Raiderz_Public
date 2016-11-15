#include "stdafx.h"
#include "GTalent.h"
#include "GUTHelper.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GTestWrapper.h"
#include "GModuleAI.h"
#include "GModuleCombat.h"
#include "GTalent.h"
#include "GTalentInfoMgr.h"
#include "GFieldMgr.h"
#include "GEntityNPC.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"

SUITE(Talent_Moving)
{
	struct FTalentMoving : public FBaseMockLink
	{
		FTalentMoving() 
			: INIT_POS(vec3(1000.0f, 1000.0f, 0.0f))
		{
			m_pField = GUTHelper_Field::DefaultMockField();

			m_NPCInfo.nID = 1;
			m_NPCInfo.m_pMeshInfo = new GActorMeshInfo();
			m_NPCInfo.m_pMeshInfo->Init(NULL, 100.0f);


			vec3 pos = INIT_POS;
			vec3 dir = vec3(0,1,0);

			m_pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, pos, dir);
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField, pos);
			m_pPlayer->SetDir(vec3(0.0f, 1.0f, 0.0f));

			m_pMovingTalentInfo = m_Helper.NewTalentInfo();
			m_pMovingTalentInfo->m_nSkillType = ST_MELEE;
			m_pMovingTalentInfo->m_nTiming = TC_HIT_TALENT;
			m_pMovingTalentInfo->m_bHitCapsulePosSync = true;

			// 판정 타입은 AREA
			m_pMovingTalentInfo->m_EffectInfo.m_fRadius = 1.0f;
			m_pMovingTalentInfo->m_fDurationTime = 2.0f;		// aitime 2초
			m_pMovingTalentInfo->m_nForceMF = MF_BEATEN;
			m_pMovingTalentInfo->m_nForceMFWeight = 2000;

			// 0.1초마다 1000씩 앞으로 이동한다.
			for (int i = 0; i < (int)m_pMovingTalentInfo->m_fDurationTime * 10; i++)
			{
				CSTalentInfo::_TALENT_MOVE_INFO	new_move_info(vec3(0.0f, -1000.0f * i, 0.0f), 0.0f);
				m_pMovingTalentInfo->m_MovingInfoList.push_back(new_move_info);
			}
	
			m_Helper.SetTalentDamage(m_pMovingTalentInfo, 1);		// 데미지 1로 설정
			m_Helper.AddHitInfo(m_pMovingTalentInfo, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1000.0f), 100.0f, 0.5f);		// 0.5초에 판정
			m_Helper.AddHitInfo(m_pMovingTalentInfo, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1000.0f), 100.0f, 0.6f);		// 0.6초에 판정
			m_Helper.AddHitInfo(m_pMovingTalentInfo, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1000.0f), 100.0f, 0.7f);		// 0.7초에 판정
			m_pMovingTalentInfo->Cooking();

			m_pNPC->SetThinkable(false);
		}
		~FTalentMoving()
		{
			m_pField->Destroy();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityNPC*		m_pNPC;
		GEntityPlayer*	m_pPlayer;

		GNPCInfo		m_NPCInfo;
		GTalentInfo*	m_pMovingTalentInfo;

		const vec3	INIT_POS;

		GTestMgrWrapper<GTalentInfoMgr>	m_TalentInfoMgrWrapper;
	};

	TEST_FIXTURE(FTalentMoving, TestMovingTalent_Hit)
	{
		m_pPlayer->doUseTalent(m_pMovingTalentInfo);
		m_pField->Update(0.1f);

		// 판정안됨
		CHECK_EQUAL(MF_NONE, m_pNPC->GetCurrMF());


		// 판정을 0.5초에서 하고, 실제 판정은 0.2초 미리 0.3초에서 하지만
		// 그래도 위치는 0.5초일 때 위치에서 판정한다.
		m_pNPC->SetPos(vec3(1000.0f, INIT_POS.y + 5000.0f, 0.0f));

		m_pField->Update(0.25f);	// 0.35초 지남

		CHECK_EQUAL(MF_BEATEN, m_pNPC->GetCurrMF());

	}

	TEST_FIXTURE(FTalentMoving, TestMovingTalent_ExpectedPos_IfNotMovingTalent)
	{
		m_pMovingTalentInfo->m_MovingInfoList.clear();
		m_pMovingTalentInfo->Cooking();

		m_pPlayer->doUseTalent(m_pMovingTalentInfo);
		m_pField->Update(0.1f);

		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		CHECK(pTalent != NULL);

		GTalentMove::MOVEINFO info = pTalent->GetExpectedMoveInfo(0.5f);

		CHECK_CLOSE(INIT_POS.y, info.vPos.y, 0.001f);
	}
	TEST_FIXTURE(FTalentMoving, TestMovingTalent_ExpectedPos_IfMovingTalent)
	{
		m_pPlayer->doUseTalent(m_pMovingTalentInfo);
		m_pField->Update(0.1f);

		GTalent* pTalent = m_pPlayer->GetModuleCombat()->GetTalent();
		CHECK(pTalent != NULL);

		GTalentMove::MOVEINFO info = pTalent->GetExpectedMoveInfo(0.5f);

		CHECK_CLOSE(INIT_POS.y + 5000.0f, info.vPos.y, 0.001f);
	}

}