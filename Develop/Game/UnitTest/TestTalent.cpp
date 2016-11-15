#include "stdafx.h"
#include "XModuleTalent.h"
#include "XUnitTestList.h"
#include "XNPCInfo.h"
#include "XUnitTestHelper.h"
#include "XSystem.h"
#include "XCharacterCaption.h"
#include "MockNetClient.h"
#include "XNonPlayer.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "MockAnimationController.h"
#include "XUnitTestUtil.h"
#include "XMyPlayer.h"
#include "XConsoleCmds.h"
#include "XPost_Training.h"
#include "TestCommonFixture.h"
#include "TestModule.h"
#include "MockItemInfo.h"
#include "FTestGame.h"
#include "XCaptionMgr.h"

SUITE(Talent)
{
	class FTalent : public FTestGame
	{
	public:
		FTalent() : FTestGame()
		{
			XUnitTestHelper::AddNPCMotions();
		}
		virtual ~FTalent() 
		{

		}
	};

	TEST_FIXTURE(FTalent, TestDamageCaption)
	{
		XUnitTestHelper helper;

		// 탤런트 정보 추가
		int ID = XUnitTestUtil::NewID();
		helper.CreateMeleeTalentInfo(ID);

		// npc 정보 추가
		XUnitTestHelper::NewNPCInfo(1, L"샘플");

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		XNonPlayer* pNPC = helper.InNPC(uidNPC, 1, vec3(1000.0f, 1000.0f, 0.0f));

		XCaptionMgr* pCaptionMgr = global.ui->GetCaptionMgr();
		XCaptionMgr::CAP_NODE_MAP& caption_node_map = pCaptionMgr->GetNodeMap();

		int captionsize = caption_node_map.size();
		CHECK_EQUAL(caption_node_map.size(), captionsize);		// CharacterCaption, GuildCaption 이 이미 생성됨

		TD_TALENT_HIT	td_talent_hit;
		td_talent_hit.nAttackerUIID = XGetMyUIID();
		td_talent_hit.nVictimUIID = pNPC->GetUIID();
		td_talent_hit.nTalentID = ID;
		td_talent_hit.nDamage = 45;
		td_talent_hit.nFlags = 0;
		td_talent_hit.nMFState = MF_NONE;
		td_talent_hit.nMFWeight = 0;
		td_talent_hit.nHitTime = 0;

		m_pNetAgent->OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));
		gg.omgr->Update(0.1f);

		CHECK_EQUAL(caption_node_map.size(), captionsize+1);		// CharacterCaption, GuildCaption 이 이미 생성됨

		if ((int)caption_node_map.size() < (captionsize+1)) return;

		XCaptionMgr::CAP_NODE_MAP::iterator itor = caption_node_map.end();
		--itor;

		XCaption* pCaption = (*itor).second;

		CHECK(pCaption->GetText() == wstring(L"45"));	// damage
	}

	TEST_FIXTURE( FTalent, TestNPCTalentChangeAnimation )
	{
		XUnitTestHelper helper;

		// 탤런트 정보 추가
		int ID = XUnitTestUtil::NewID();
		helper.CreateMeleeTalentInfo(ID);

		// npc 정보 추가
		XUnitTestHelper::NewNPCInfo(107025, L"렌고트 궁수", 3, L"goblin_archer", L"Monster/goblin_archer", L"monster");

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		XNonPlayer* pNPC = helper.InNPC(uidNPC, 107025, vec3(1000.0f, 1000.0f, 0.0f));

		// 애니메이션 셋팅
		TestAniDataSet		ani_set;
		ani_set.AddAni(L"idle", 1.0, false);

		// 모션 셋팅
		XTestInfoWrapper<XMotionMgr>	m_MotionInfoWrapper;
		info.motion->AddMotionForTest(L"	<MOTION name='PS_dodge_back' stance='all'>		\
									  <Default>															\
									  <Sequence ani='idle'/>												\
									  </Default>															\
									  </MOTION>");

		m_Helper.SetAniDataSet(pNPC, &ani_set);

		// 탤런트 사용
		//////////////////////////////////////////////////////////////////////////
		// 1. 활 공격 탤런트 사용
		m_pNetAgent->OnRecv( MC_ACTION_USE_TALENT, 4,

			NEW_USHORT( pNPC->GetUIID() ),
			NEW_INT(ID),
			NEW_SVEC( vec3(1.0f, 0.0f, 0.0f) ),
			NEW_VEC( vec3(1000.0f, 1000.0f, 0.0f))
			);

		gg.omgr->Update(2.3f);

		XNonPlayer * pNonPlayer = gg.omgr->FindNPC(uidNPC);
		CHECK(pNonPlayer != NULL);

		XModuleEntity * pModuleEntity = pNonPlayer->GetModuleEntity();
		CHECK(pModuleEntity != NULL);

		CHECK_EQUAL(pModuleEntity->GetAnimationController()->IsPlayDone() , true);

		info.npc->Clear();
		helper.Clear();
	}


	TEST_FIXTURE(FTalent, TestTalentHitDelay)
	{
		XUnitTestHelper helper;

		// 탤런트 정보 추가
		int ID = XUnitTestUtil::NewID();
		helper.CreateMeleeTalentInfo(ID);

		// npc 정보 추가
		XUnitTestHelper::NewNPCInfo(1);

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		XNonPlayer* pNPC = helper.InNPC(uidNPC, 1, vec3(1000.0f, 1000.0f, 0.0f), &m_NPCAniDataSet);

		// beaten 1초.
		TD_TALENT_HIT	td_talent_hit;
		td_talent_hit.nAttackerUIID = XGetMyUIID();
		td_talent_hit.nVictimUIID = pNPC->GetUIID();
		td_talent_hit.nTalentID = ID;
		td_talent_hit.nDamage = 45;
		td_talent_hit.nFlags = 0;
		td_talent_hit.nMFState = MF_BEATEN;
		td_talent_hit.nMFWeight = 1000;
		td_talent_hit.nHitTime = 0;

		m_pNetAgent->OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));

		Update(0.1f);

		pNPC = gg.omgr->FindNPC(uidNPC);
		XModuleMotion* pModuleMotion = pNPC->GetModuleMotion();

		CHECK(wstring(L"npc_mf_beaten_back") == pModuleMotion->GetCurrMotion());

		info.npc->Clear();
		helper.Clear();
	}

	TEST_FIXTURE(FTalent, TestProjectileMagicTalent)
	{
		// 내 캐릭터 선택
		XMyPlayer* pMyPlayer = m_TestGame.GetMyPlayer();
		CHECK(pMyPlayer != NULL);
		if (pMyPlayer == NULL) return;

		info.motion->Init();

		// 초기 위치 셋팅
		pMyPlayer->SetPosition(vec3(500.0f, 500.0f, 0.0f));
		pMyPlayer->SetDirection(vec3(1.0f, 1.0f, 0.0f).Normalize());
		
		// 아이템 추가
		XUnitTestHelper::CreateWeaponItemData(1, WEAPON_1H_SLASH);
		XUnitTestHelper::CreateItemAndEquip(pMyPlayer, 1, ITEMSLOT_RWEAPON);

		// 탤런트 정보 추가
		int ID = XUnitTestUtil::NewID();
		m_Helper.CreateProjectileMagicTalentInfo(ID);

		// 탈렌트 배우기
		XTalentInfo* pTalentInfo = info.talent->Get(ID);
		gvar.MyInfo.Talent.Train(pTalentInfo);

		// npc 정보 추가
		XUnitTestHelper::NewNPCInfo(107025, L"렌고트 궁수", 3, L"goblin_archer", L"Monster/goblin_archer", L"monster");

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		m_Helper.InNPC(uidNPC, 107025, vec3(1000.0f, 1000.0f, 0.0f));

		//////////////////////////////////////////////////////////////////////////
		// 발사체 마법 탤런트 사용 준비
		XModuleTalent* pModuleTalent = m_pTestContext->GetModule<XModuleTalent>(pMyPlayer);
		CHECK(pModuleTalent);

		// 전투 준비로 바꾸어야 한다.
		pMyPlayer->SetStance(CS_BATTLE);

		//////////////////////////////////////////////////////////////////////////
		// 탤런트 사용
		XModuleMyControl* pModuleMyControl = m_pTestContext->GetModule<XModuleMyControl>(pMyPlayer);
		pModuleMyControl->DoActionUseTalent(ID);

		CHECK(pModuleMyControl->GetCurrentActionStateID() == ACTION_STATE_USE_TALENT);
	
		//////////////////////////////////////////////////////////////////////////
		// 탤런트 사용에 대한 응답
		m_pNetAgent->OnRecv( MC_ACTION_USE_TALENT, 4,
			NEW_USHORT( pMyPlayer->GetUIID() ),
			NEW_INT(ID),
			NEW_SVEC( pMyPlayer->GetDirection() ),
			NEW_VEC( pMyPlayer->GetPosition() )
			);

		CHECK(pModuleMyControl->GetCurrentActionStateID() == ACTION_STATE_USE_TALENT);

		gg.omgr->Update(1.8f);

		// 타겟 셋팅
		pModuleTalent->SetTarget(uidNPC, -1, -1);

		// 발사체가 발사되지 않았으므로 오브젝트 수는 2개
		CHECK_EQUAL(2, gg.omgr->GetCount());

		//////////////////////////////////////////////////////////////////////////
		// 발사에 대한 응답
		m_pNetAgent->OnRecv( MC_ACTION_ACT_TALENT, 3,
			NEW_USHORT( pMyPlayer->GetUIID() ),
			NEW_INT(ID),
			NEW_SVEC( pMyPlayer->GetDirection() )
			);

		//////////////////////////////////////////////////////////////////////////
		// 발사 마법 탤런트에 대한 전체 체크
		// 탤런트가 ACT 상태인가?
		gg.omgr->Update(0.1f);
		CHECK_EQUAL(TALENT_PHASE_ACT, pModuleTalent->GetPhase());
	
		// 발사체가 생성이 되었는지... 오브젝트 매니저에서 개수를 체크한다.
		gg.omgr->Update(0.4f);
		gg.omgr->Update(0.01f);
		CHECK_EQUAL(3, gg.omgr->GetCount());

		info.npc->Clear();
		m_Helper.Clear();
	}

}
