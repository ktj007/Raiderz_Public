#include "stdafx.h"
#include "XMagicArea.h"
#include "XGame.h"
#include "XUnitTestHelper.h"
#include "XUnitTestUtil.h"
#include "TestGame.h"
#include "XSystem.h"
#include "XTalentInfoMgr.h"
#include "FTestGame.h"
#include "XMyPlayer.h"


SUITE(TestMagicArea)
{
	class FTestMagicArea : public FTestGame
	{
	public:
		FTestMagicArea()
		{
			// 탤런트 정보 추가
			m_Helper.CreateTalentInfo(25400, L"<TALENT id=\"25400\" name=\"치유 마법진 (test)\" TalentLine=\"55217\" Rank=\"1\" TalentType=\"skill\" Category=\"magic\" SkillType=\"magicarea\"  \
											 Style=\"sorcerer\" HPCost=\"0\" APCost=\"0\" ENCost=\"0\" STACost=\"0\" HPTickCost=\"0\" ENTickCost=\"0\" STATickCost=\"0\" CoolTime=\"5\" \
											 DurationTime=\"10\" Stance=\"all\" CastingAni=\"spell_area_cast\" CastingLoopAni=\"PS_pb_idle\" CastingTime=\"1.6\" UseAni=\"PS_pb_fire\" \
											 AttackDirection=\"front\" CriticalEnable=\"True\" Guardable=\"True\" Cancelable=\"False\" Buff1=\"348\" BuffEffecteePoint1=\"self\" \
											 BuffEffecteeRelation1=\"allied\" BuffPeriod1=\"2\" UseBloodEffect=\"True\" EffecteePoint=\"areapoint\" EffecteeRadius=\"10\" EffecteeRelation=\"allied\" \
											 Validation=\"False\" />");
		}

		virtual ~FTestMagicArea()
		{

		}
	};

	TEST_FIXTURE(FTestMagicArea, TestMagicArea_CreateAndDelete)
	{
		// TODO(isnara): BuffEntity로 변경되면서 임시로 주석처리 합니다. 익성씨가 수정해주세요~ --pyo
		//XMyPlayer* pMyPlayer = GetMyPlayer();

		//int nCurrentCount = m_pGame->GetCurrentGameState()->GetObjectManager()->GetCount();

		//// 생성
		//TD_UPDATE_CACHE_BUFFENTITY infoTD;
		//infoTD.UID = global.system->GenerateLocalUID();
		//infoTD.vPos = pMyPlayer->GetPosition();
		//infoTD.fReaminTime = 5.0f;
		//infoTD.nTalentID = 25400;

		//m_pNetAgent->OnRecv(MC_FIELD_IN_MAGICAREA, 1, NEW_SINGLE_BLOB(&infoTD, sizeof(TD_UPDATE_CACHE_BUFFENTITY)));

		//int nIsChangeCount = m_pGame->GetCurrentGameState()->GetObjectManager()->GetCount();

		//// 늘어났나?
		//CHECK(nCurrentCount < nIsChangeCount);

		//// 종료
		//m_pNetAgent->OnRecv(MC_FIELD_OUT_MAGICAREA, 1, NEW_UID(infoTD.UID));
		//
		//Update(0.1f);

		//nIsChangeCount = m_pGame->GetCurrentGameState()->GetObjectManager()->GetCount();

		//// 줄어들었나?
		//CHECK(nCurrentCount == nIsChangeCount);
	}
}

