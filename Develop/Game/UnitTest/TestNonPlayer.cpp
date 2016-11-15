#include "stdafx.h"

#include "XNPCInfo.h"
#include "XUnitTestList.h"
#include "TestCommonFixture.h"
#include "XModuleMotion.h"
#include "XModuleEntity.h"
#include "XSystem.h"
#include "XGlobal.h"
#include "XGame.h"
#include "XNonPlayer.h"
#include "TestModule.h"
#include "MockCharacter.h"
#include "XUnitTestUtil.h"

SUITE(NonPlayer)
{
	class XTestNonPlayer : public XNonPlayer
	{
	public:
		XTestNonPlayer(MUID uid, UIID nUIID) : XNonPlayer(uid, nUIID)
		{

		}
		virtual ~XTestNonPlayer()
		{

		}
		void SetRadius_ForTest(float fRadius)
		{
			m_pModuleCollision->SetRadius_ForTest(fRadius);
		}
	};
	class FNonPlayer 
	{
	public:
		FNonPlayer()
		{
			MUID uidNPC = global.system->GenerateLocalUID();
			m_pNPC = new XTestNonPlayer(uidNPC, 1);
			m_pNPC->InitNPC(&m_NPCInfo, NULL);
		}
		~FNonPlayer()
		{
			delete m_pNPC;
		}
	
		XNPCInfo			m_NPCInfo;
		XTestNonPlayer*		m_pNPC;
	};

	TEST_FIXTURE(SimpleTestFixture, NPCAnimationCheckWhenEntry)
	{
		int nNPCID = 1;
		vec3 vPos = vec3(1000.0f, 1000.0f, 0.0f);
		MUID uidNPC = global.system->GenerateLocalUID();

		TD_UPDATE_CACHE_NPC td_update_cache_npc = XUnitTestHelper::Make_TD_UPDATE_CACHE_NPC(uidNPC,
													nNPCID,
													vPos);

		// nStatusFlag가 0일 때 애니메이션 체크
		m_Helper.InNPCs(&td_update_cache_npc, 1, &m_NPCAniDataSet);

		XNonPlayer* pNPC = gg.omgr->FindNPC(uidNPC);
		CHECK(pNPC != NULL);

		if (pNPC == NULL) return;


		// nStatusFlag에 아무값이 없으므로 가만히 서있는 idle 애니메이션이다.
		CHECK(pNPC->GetModuleMotion()->GetCurrMotion() == wstring(MOTION_NAME_NPC_IDLE));

		gg.game->Update(0.1f);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);

		CHECK(pNPC->GetModuleMotion()->GetCurrMotion() == wstring(MOTION_NAME_NPC_IDLE));

		// 달리는 상태 ------------
		uidNPC = global.system->GenerateLocalUID();

		td_update_cache_npc.uid = uidNPC;
		td_update_cache_npc.nUIID = XUnitTestUtil::NewUIID();
		SetBitSet(td_update_cache_npc.nStatusFlag, UNS_RUN);		// 달리는 상태
		td_update_cache_npc.nSpeed = 100;
		td_update_cache_npc.vTarPos = vec3(0.0f, 0.0f, 0.0f);

		m_Helper.InNPCs(&td_update_cache_npc, 1, &m_NPCAniDataSet);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);

		pNPC = gg.omgr->FindNPC(uidNPC);
		CHECK(pNPC != NULL);
		if (pNPC == NULL) return;

		// 달리는 중이므로 run 모션
		CHECK(pNPC->GetModuleMotion()->GetCurrMotion() == wstring(MOTION_NAME_NPC_RUN));

		// 걷는 상태 ------------
		uidNPC = global.system->GenerateLocalUID();

		td_update_cache_npc.uid = uidNPC;
		td_update_cache_npc.nUIID = XUnitTestUtil::NewUIID();
		td_update_cache_npc.nStatusFlag = 0;
		SetBitSet(td_update_cache_npc.nStatusFlag, UNS_WALK);		// 달리는 상태
		td_update_cache_npc.nSpeed = 100;
		td_update_cache_npc.vTarPos = vec3(0.0f, 0.0f, 0.0f);

		m_Helper.InNPCs(&td_update_cache_npc, 1, &m_NPCAniDataSet);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);
		gg.game->Update(0.1f);

		pNPC = gg.omgr->FindNPC(uidNPC);
		CHECK(pNPC != NULL);
		if (pNPC == NULL) return;



		// 걷는 중이므로 walk 모션
		CHECK(pNPC->GetModuleMotion()->GetCurrMotion() == wstring(MOTION_NAME_NPC_WALK));


		// Lootable 가능한 죽음 상태 ------------
		uidNPC = global.system->GenerateLocalUID();

		td_update_cache_npc.uid = uidNPC;
		td_update_cache_npc.nUIID = XUnitTestUtil::NewUIID();
		td_update_cache_npc.nStatusFlag = 0;
		SetBitSet(td_update_cache_npc.nStatusFlag, UNS_DEAD);
		SetBitSet(td_update_cache_npc.nStatusFlag, UNS_LOOTABLE);
		td_update_cache_npc.nSpeed = 100;
		td_update_cache_npc.vTarPos = vec3(0.0f, 0.0f, 0.0f);

		m_Helper.InNPCs(&td_update_cache_npc, 1, &m_NPCAniDataSet);
		gg.game->Update(0.1f);

		pNPC = gg.omgr->FindNPC(uidNPC);
		CHECK(pNPC != NULL);
		if (pNPC == NULL) return;

		// 걷는 중이므로 walk 모션
		CHECK(pNPC->GetModuleMotion()->GetCurrMotion() == wstring(MOTION_NAME_NPC_DIE));

		// 그냥 죽음 상태 ------------
		uidNPC = global.system->GenerateLocalUID();

		td_update_cache_npc.uid = uidNPC;
		td_update_cache_npc.nUIID = XUnitTestUtil::NewUIID();
		td_update_cache_npc.nStatusFlag = 0;
		SetBitSet(td_update_cache_npc.nStatusFlag, UNS_DEAD);
		td_update_cache_npc.nSpeed = 100;
		td_update_cache_npc.vTarPos = vec3(0.0f, 0.0f, 0.0f);

		m_Helper.InNPCs(&td_update_cache_npc, 1, &m_NPCAniDataSet);
		gg.game->Update(0.1f);

		pNPC = gg.omgr->FindNPC(uidNPC);
		CHECK(pNPC != NULL);
		if (pNPC == NULL) return;

		// 걷는 중이므로 walk 모션
		CHECK(pNPC->GetModuleMotion()->GetCurrMotion() == wstring(MOTION_NAME_NPC_DIE));

	}

	TEST_FIXTURE(SimpleTestFixture, TestNonPlayerProcessMoveException_1_1)
	{
		TestModuleContext	testContext(this);

		//testContext.GetNetAgent().IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		XNonPlayer* pNPC = testContext.CreateNPCAndEnterToGame(1, vec3(1000.0f, 1000.0f, 0.0f));
		if (pNPC == NULL) return;

		vec3 pNonPlayerPos = pNPC->GetPosition();
		TD_NPC_MOVE moveData;

		vec3 vCurrentNonPlayerPos(0.0f, 0.0f, 0.0f);

		//////////////////////////////////////////////////////////////////////////
		// 1. 거리차에 의한 예외처리 체크 (워프)
		//
		const float fUpdateTick = 0.1f;

		moveData.nUIID		= pNPC->GetUIID();
		moveData.vTarPos	= pNPC->GetPosition() + pNPC->GetDirection() * (PROCESS_MOVE_EXCEPTION_DISTANCE + 1.0f);
		moveData.svDir		= pNPC->GetDirection();
		moveData.nSpeed		= 100;

		// 의도된 현재 좌표
		float fNonPlayerDist = moveData.nSpeed * fUpdateTick;
		vCurrentNonPlayerPos = pNPC->GetPosition() + pNPC->GetDirection() * fNonPlayerDist;

		m_pNetAgent->OnRecv(MC_NPC_MOVE, 1,
			NEW_SINGLE_BLOB(&moveData, sizeof(TD_NPC_MOVE))
			);

		gg.game->Update(fUpdateTick);

		// 거리가 30m가 넘어서 워프가 되었는가? (새로운 좌표로 셋팅이 되어 있어야 한다.)
		vec3 vExceptionNonPlayerPos = pNPC->GetPosition();

		CHECK_CLOSE(vExceptionNonPlayerPos.x, vCurrentNonPlayerPos.x, 0.001f);		
		CHECK_CLOSE(vExceptionNonPlayerPos.y, vCurrentNonPlayerPos.y, 0.001f);		
		CHECK_CLOSE(vExceptionNonPlayerPos.z, vCurrentNonPlayerPos.z, 0.001f);	
	}

	TEST_FIXTURE(SimpleTestFixture, TestNonPlayerProcessMoveException_1_2)
	{
		TestModuleContext	testContext(this);
		//testContext.GetNetAgent().IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		XNonPlayer* pNPC = testContext.CreateNPCAndEnterToGame(1, vec3(1000.0f, 1000.0f, 0.0f));
		if (pNPC == NULL) return;

		vec3 pNonPlayerPos = pNPC->GetPosition();
		TD_NPC_MOVE moveData;

		vec3 vCurrentNonPlayerPos(0.0f, 0.0f, 0.0f);

		//////////////////////////////////////////////////////////////////////////
		// 1. 거리차에 의한 예외처리 체크 (일반)
		//
		const float fUpdateTick = 0.1f;

		moveData.nUIID		= pNPC->GetUIID();
		moveData.vTarPos	= pNPC->GetPosition() + pNPC->GetDirection() * (PROCESS_MOVE_EXCEPTION_DISTANCE * 2);
		moveData.svDir		= pNPC->GetDirection();
		moveData.nSpeed		= 100;

		// 의도된 현재 좌표
		float fNonPlayerDist = moveData.nSpeed * fUpdateTick;
		vCurrentNonPlayerPos = pNonPlayerPos;
		vCurrentNonPlayerPos += pNPC->GetDirection() * fNonPlayerDist;

		m_pNetAgent->OnRecv(MC_NPC_MOVE, 1,
			NEW_SINGLE_BLOB(&moveData, sizeof(TD_NPC_MOVE))
			);

		gg.game->Update(fUpdateTick);

		// 거리가 30m가 안넘어서 그냥 이동 처리가 되었는가? (기존좌표에 변화가 없어야 한다.)
		vec3 vExceptionNonPlayerPos = pNPC->GetPosition();

		CHECK_CLOSE(vExceptionNonPlayerPos.x, vCurrentNonPlayerPos.x, 0.001f);		
		CHECK_CLOSE(vExceptionNonPlayerPos.y, vCurrentNonPlayerPos.y, 0.001f);		
		CHECK_CLOSE(vExceptionNonPlayerPos.z, vCurrentNonPlayerPos.z, 0.001f);	
	}

	TEST_FIXTURE(SimpleTestFixture, TestNonPlayerProcessMoveException_2_1)
	{
		TestModuleContext	testContext(this);
		//testContext.GetNetAgent().IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		XNonPlayer* pNPC = testContext.CreateNPCAndEnterToGame(1, vec3(1000.0f, 1000.0f, 0.0f));
		if (pNPC == NULL) return;

		pNPC->SetPosition(vec3(1000.0f, 1000.0f, -5.0f));
		vec3 pNonPlayerPos = pNPC->GetPosition();
		TD_NPC_MOVE moveData;

		vec3 vCurrentNonPlayerPos(0.0f, 0.0f, 0.0f);

		//////////////////////////////////////////////////////////////////////////
		// 1. 높이차에 의한 예외처리 체크 (워프)
		//
		const float fUpdateTick = 0.1f;
	
		moveData.nUIID		= pNPC->GetUIID();
		moveData.vTarPos	= pNPC->GetPosition() + pNPC->GetDirection() * (PROCESS_MOVE_EXCEPTION_DISTANCE * 2);
		moveData.svDir		= pNPC->GetDirection();
		moveData.nSpeed		= 100;

		// 의도된 현재 좌표
		float fNonPlayerDist = moveData.nSpeed * fUpdateTick;
		vCurrentNonPlayerPos = pNPC->GetPosition() + vec3(0.0f, 0.0f, 10.0f) + pNPC->GetDirection() * fNonPlayerDist;

		m_pNetAgent->OnRecv(MC_NPC_MOVE, 1,
			NEW_SINGLE_BLOB(&moveData, sizeof(TD_NPC_MOVE))
			);

		gg.game->Update(fUpdateTick);

		// 높이의 차가 생겨서 워프한다. (새로운 좌표로 셋팅이 되어 있어야 한다.)
		vec3 vExceptionNetPlayerPos = pNPC->GetPosition();

		CHECK_CLOSE(vExceptionNetPlayerPos.x, vCurrentNonPlayerPos.x, 0.001f);		
		CHECK_CLOSE(vExceptionNetPlayerPos.y, vCurrentNonPlayerPos.y, 0.001f);		
		CHECK_CLOSE(vExceptionNetPlayerPos.z, 0.0f, 0.001f);
	}

	TEST_FIXTURE(SimpleTestFixture, TestNonPlayerDashMove)
	{
		TestModuleContext	testContext(this);
		//testContext.GetNetAgent().IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		XNonPlayer* pNPC = testContext.CreateNPCAndEnterToGame(1, vec3(1000.0f, 1000.0f, 0.0f));
		if (pNPC == NULL) return;

		XModuleEntity* pModuleEntity = testContext.GetModule<XModuleEntity>(pNPC);
		CHECK(pModuleEntity);

		MockCharacter * pMockCharacter = (MockCharacter *)(pModuleEntity->GetActor());
		CHECK(pMockCharacter);
		pMockCharacter->SetDummyLocData(L"dummy_loc", vec3(1000.0f, 1000.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 600.0f);
			
		vec3 pNonPlayerStartPos = pNPC->GetPosition();

		XUnitTestHelper helper;
		helper.CreateTalentInfo(211006, L"<TALENT name=\"대시\" npc=\"110\" TalentType=\"skill\" Category=\"melee\" SkillType=\"melee\" \
									EffectTiming=\"hit_this_talent\" EffectRadius=\"2\" HitTestType=\"area\" HitMotionDelay=\"0\" TargetsAllowed=\"enemy\" \
									HPCost=\"0\" APCost=\"0\" ENCost=\"0\" STACost=\"0\" HPTickCost=\"0\" ENTickCost=\"0\" STATickCost=\"0\" CoolTime=\"5\" \
									UseAni=\"atk_d\" AttackDirection=\"front\" IsMoving=\"True\" IsSetDirection=\"True\" MF_BEATEN=\"50\" MF_WEIGHT_BEATEN=\"2000\" \
									MF_KNOCKBACK=\"0\" MF_WEIGHT_KNOCKBACK=\"0\" MF_STUN=\"0\" MF_WEIGHT_STUN=\"0\" MF_DEFLECT=\"0\" MF_WEIGHT_DEFLECT=\"0\" \
									MF_THROWUP=\"200\" MF_WEIGHT_THROWUP=\"1000\" MF_KNOCKDOWN=\"0\" MF_WEIGHT_KNOCKDOWN=\"0\" \
									MF_UPPERED=\"0\" MF_WEIGHT_UPPERED=\"0\" MinDamage=\"75\" MaxDamage=\"80\" DamageAttrib=\"blunt\" Guardable=\"True\" Cancelable=\"False\" />"
									);
		//////////////////////////////////////////////////////////////////////////
		// 1. NPC 탤런트 사용
		//	
		m_pNetAgent->OnRecv( MC_ACTION_USE_TALENT, 4,
			NEW_USHORT( pNPC->GetUIID() ),
			NEW_INT(211006),
			NEW_SVEC( vec3(1.0f, 0.0f, 0.0f) ),
			NEW_VEC( vec3(1000.0f, 1000.0f, 0.0f))
			);

		gg.game->Update(1.0f);

		//////////////////////////////////////////////////////////////////////////
		// 2. NPC dummy_loc에 의한 이동 체크
		//	
		vec3 vCurrentPos = pNPC->GetPosition();
		//CHECK((vCurrentPos - pNonPlayerStartPos).Length() > 100.0f);	
		
		helper.Clear();
	}

	// 이펙트 문제를 해결해야 할듯...
	//TEST_FIXTURE(SimpleTestFixture, TestNonPlayerKnockbackDie)
	//{
	//	TestModuleContext	testContext(this);
	//	//testContext.GetNetAgent().IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

	//	XNonPlayer* pNPC = testContext.CreateNPCAndEnterToGame(1, vec3(1000.0f, 1000.0f, 0.0f));
	//	if (pNPC == NULL) return;

	//	XModuleEntity* pModuleEntity = testContext.GetModule<XModuleEntity>(pNPC);
	//	CHECK(pModuleEntity);

	//	MockCharacter * pMockCharacter = (MockCharacter *)(pModuleEntity->GetActor());
	//	CHECK(pMockCharacter);
	//	pMockCharacter->SetDummyLocData("dummy_loc", vec3(1000.0f, 1000.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 600.0f);

	//	vec3 pNonPlayerStartPos = pNPC->GetPosition();

	//	XUnitTestHelper helper;
	//	helper.CreateTalentInfo(211006, "<TALENT name=\"대시\" npc=\"110\" TalentType=\"skill\" Category=\"melee\" SkillType=\"melee\" \
	//									EffectTiming=\"hit_this_talent\" EffectRadius=\"2\" HitTestType=\"area\" HitMotionDelay=\"0\" TargetsAllowed=\"enemy\" \
	//									HPCost=\"0\" APCost=\"0\" ENCost=\"0\" STACost=\"0\" HPTickCost=\"0\" ENTickCost=\"0\" STATickCost=\"0\" CoolTime=\"5\" \
	//									UseAni=\"atk_d\" AttackDirection=\"front\" IsMoving=\"True\" IsSetDirection=\"True\" MF_BEATEN=\"50\" MF_WEIGHT_BEATEN=\"2000\" \
	//									MF_KNOCKBACK=\"0\" MF_WEIGHT_KNOCKBACK=\"0\" MF_STUN=\"0\" MF_WEIGHT_STUN=\"0\" MF_DEFLECT=\"0\" MF_WEIGHT_DEFLECT=\"0\" \
	//									MF_THROWUP=\"200\" MF_WEIGHT_THROWUP=\"1000\" MF_KNOCKDOWN=\"0\" MF_WEIGHT_KNOCKDOWN=\"0\" \
	//									MF_UPPERED=\"0\" MF_WEIGHT_UPPERED=\"0\" MinDamage=\"75\" MaxDamage=\"80\" DamageAttrib=\"blunt\" Guardable=\"True\" Cancelable=\"False\" />"
	//									);

	//	//////////////////////////////////////////////////////////////////////////
	//	// 0. 애니메이션 셋팅
	//	TestAniDataSet		ani_set;
	//	ani_set.AddAni("die", 3.0f, false);
	//	ani_set.AddAni("MF_KB1", 0.5f, false);
	//	ani_set.AddAni("MF_KB2", 1.0f, false);
	//	ani_set.AddAni("MF_KB3", 0.5f, false);
	//	ani_set.AddAni("MF_TU1", 0.8f, false);
	//	ani_set.AddAni("MF_TU2", 1.0f, false);

	//	MockAnimationController		mock_ac(NULL);
	//	mock_ac.SetAniDataSet(&ani_set);

	//	mock_ac.SetAnimation(string("idle"));

	//	//////////////////////////////////////////////////////////////////////////
	//	// 1. NPC Die 설정
	//	//	
	//	m_pNetAgent->OnRecv( MC_ACTION_DIE, 2,
	//		NEW_UID( pNPC->GetUID() ),
	//		NEW_UID( testContext.GetMyPlayer()->GetUID())
	//		);

	//	//////////////////////////////////////////////////////////////////////////
	//	// 2. NPC 히트 설정
	//	//	

	//	// 넉백 히트를 보낸다.
	//	TD_TALENT_HIT	td_talent_hit;
	//	td_talent_hit.uidAttacker = XGetMyUID();
	//	td_talent_hit.uidVictim = pNPC->GetUID();
	//	td_talent_hit.nTalentID = 211006;
	//	td_talent_hit.nDamage = 45;
	//	td_talent_hit.nFlags = 0;
	//	td_talent_hit.nMFState = MF_KNOCKBACK;
	//	td_talent_hit.nMFWeight = 0;
	//	td_talent_hit.nHitTime = global.system->GetTimer().GetNowGlobalTime();

	//	m_pNetAgent->OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT), 1));

	//	gg.game->Update(0.1f);

	//	//////////////////////////////////////////////////////////////////////////
	//	// 3. 체크
	//	//
	//	XModuleMotion* pModuleMotion = testContext.GetModule<XModuleMotion>(pNPC);
	//	CHECK(pModuleMotion);
	//	CHECK(pModuleMotion->GetCurrMotion() == "die");

	//	helper.Clear();
	//}

	TEST_FIXTURE(FNonPlayer, TestNonPlayer_GetValidInteractionDistance)
	{
		// 1차
		float fRadius = DEFAULT_ENTITY_RADIUS;
		m_pNPC->SetRadius_ForTest(fRadius);
		float fValidDist = m_pNPC->GetValidInteractionDistance();
		CHECK_EQUAL((fRadius * GAME_INTERACTION_DISTANCE_XY_MULTIPLICATION) + GAME_INTERACTION_DISTANCE_XY_ADDITION , fValidDist);

		// 2차
		fRadius = 200.0f;
		m_pNPC->SetRadius_ForTest(fRadius);
		fValidDist = m_pNPC->GetValidInteractionDistance();
		CHECK_EQUAL((fRadius * GAME_INTERACTION_DISTANCE_XY_MULTIPLICATION) + GAME_INTERACTION_DISTANCE_XY_ADDITION , fValidDist);

		// 3차
		fRadius = 1000.0f;
		m_pNPC->SetRadius_ForTest(fRadius);
		fValidDist = m_pNPC->GetValidInteractionDistance();
		CHECK_EQUAL((fRadius * GAME_INTERACTION_DISTANCE_XY_MULTIPLICATION) + GAME_INTERACTION_DISTANCE_XY_ADDITION , fValidDist);
	}

	TEST_FIXTURE(FNonPlayer, TestNonPlayer_GetValidInteractionDistance_NPCInfoInteractionDistance)
	{
		// 1차
		m_NPCInfo.nInteractionDistance = 1;
		float fRadius = DEFAULT_ENTITY_RADIUS;
		m_pNPC->SetRadius_ForTest(fRadius);
		float fValidDist = m_pNPC->GetValidInteractionDistance();
		CHECK_EQUAL(1.0f, fValidDist);

		// 2차
		m_NPCInfo.nInteractionDistance = 40;
		fValidDist = m_pNPC->GetValidInteractionDistance();
		CHECK_EQUAL(40.0f, fValidDist);

		// 3차
		m_NPCInfo.nInteractionDistance = 2000;
		fValidDist = m_pNPC->GetValidInteractionDistance();
		CHECK_EQUAL(2000.0f, fValidDist);
	}

	TEST_FIXTURE(SimpleTestFixture, CreateNPCButNPCDontUseGravity)
	{
		TestModuleContext	testContext(this);

		XNPCInfo* pNpcInfo = info.npc->Get(1);
		pNpcInfo->nNpcType = NPC_TYPE_OBJECT;

		XNonPlayer* pNPC = testContext.CreateNPCAndEnterToGame(1, vec3(100.0f, 100.0f, 100.0f));
		if (pNPC == NULL) return;

		gg.game->Update(1.0f);
		
		CHECK_EQUAL(100.0f, pNPC->GetPosition().z);
	}

	TEST_FIXTURE(SimpleTestFixture, CreateNPCAndNPCUseGravity)
	{
		TestModuleContext	testContext(this);

		XNPCInfo* pNpcInfo = info.npc->Get(1);
		pNpcInfo->nNpcType = NPC_TYPE_MONSTER;

		XNonPlayer* pNPC = testContext.CreateNPCAndEnterToGame(1, vec3(100.0f, 100.0f, 100.0f));
		if (pNPC == NULL) return;

		gg.game->Update(1.0f);

		CHECK_CLOSE(0.0f, pNPC->GetPosition().z, 0.001f);
	}

}

/////////////////////////////////////////////////////////////////////////

