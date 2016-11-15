#include "stdafx.h"

#include "TestModule.h"

#include "XMyPlayer.h"

#include "XNetwork.h"
#include "XModuleMyControl.h"
#include "XModuleMovable.h"
#include "XUnitTestUtil.h"
#include "RMesh.h"

#include "XTalentHitParam.h"

SUITE (MyPlayer)
{
	TEST( TestCreateMyControl )
	{
		TestGame tGame;
		XMyPlayer* pMyPlayer = tGame.GetMyPlayer();
		CHECK(pMyPlayer != NULL);
		if (pMyPlayer == NULL) return;

		XModuleMyControl* pModuleMyControl = static_cast<XModuleMyControl*>(pMyPlayer->GetModule( XMID_PLAYER_CONTROL ));
		CHECK(pModuleMyControl != NULL);
		if (pModuleMyControl)
		{
			// OnHit
			XTalentHitParam talentHitParam;

			talentHitParam.nAttackerUIID = XUnitTestUtil::NewUIID();
			talentHitParam.nDamage = 0;
			talentHitParam.nTalentID = 0;
			talentHitParam.nMFState = MF_BEATEN;
			talentHitParam.nMFWeight = 0;
			talentHitParam.nFlags = 0;

			pModuleMyControl->OnHit(&talentHitParam);

			pMyPlayer->Update(0.5f);
		}
	}

	TEST( TestMyPlayerTerrainHeightCollisionCheck )
	{
		// 지형검사시 최대높이(-100000.0f)를 체크하는 것이다.
		TestGame tGame;
		minet::MTestNetAgent tNetAgent(global.net->GetClient());
		TestModuleContext testContext(&tGame, &tNetAgent);

		XMyPlayer* pMyPlayer = testContext.GetMyPlayer();
		CHECK(pMyPlayer != NULL);

		// 초기 위치 셋팅
		pMyPlayer->SetPosition(vec3(1.0f, 1.0f, 0.0f));

		// 워프
		XModuleMyControl* pModuleMyControl = testContext.GetModule<XModuleMyControl>(pMyPlayer);
		CHECK(pModuleMyControl != NULL);
		if (pModuleMyControl)
		{
			pModuleMyControl->OnWarp(vec3(1.0f, 1.0f, 25000.0f), pMyPlayer->GetDirection());
			pMyPlayer->Update(0.1f);
		}

		// 떨어지는 상태 확인
		XModuleMovable* pModuleMovable = testContext.GetModule<XModuleMovable>(pMyPlayer);
		const XMovableFactor::SState& sCurrentGeneralState = pModuleMovable->GetMovableFactor().GENERAL_STATE;

		CHECK_EQUAL(sCurrentGeneralState.m_bFallingButNotJumpReserved, false);
		CHECK_EQUAL(sCurrentGeneralState.m_bFallingButNotJump, true);
	}

	TEST(TestMyPlayerNormalMovingForward)
	{
		minet::MTestNetAgent	net_agent(global.net->GetClient());
		net_agent.IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		TestGame		tg;

		XMyPlayer* pMyPlayer = tg.GetMyPlayer();
		if (pMyPlayer == NULL) return;

		MockController* pController = tg.GetController();

		XUnitTestHelper helper;
		TestModuleContext testContext(&tg, &net_agent);

		XModuleEntity* pPlayerModuleEntity = testContext.GetModule<XModuleEntity>(pMyPlayer);
		CHECK(pPlayerModuleEntity);

		// Controller
		XModuleMyControl* pModuleMyControl = pMyPlayer->GetModuleMyControl();
		CHECK(pModuleMyControl != NULL);
		if (pModuleMyControl == NULL) return;

		// Movable
		XModuleMovable* pModuleMovable = (XModuleMovable*)pMyPlayer->GetModule(XMID_MOVABLE);
		CHECK(pModuleMovable != NULL);
		if (pModuleMovable == NULL) return;

		//////////////////////////////////////////////////////////////////////////

		pMyPlayer->SetPosition(vec3(1000.0f, 1000.0f, 0.0f));

		vec3 vMyDir = vec3(0.0f, 1.0f, 0.0f);

		pController->TestSetCameraDir(vMyDir);
		pModuleMyControl->SyncPlayerDirToCameraDir();

		CHECK_CLOSE(pMyPlayer->GetPosition().x, 1000.0f, 0.001f);
		CHECK_CLOSE(pMyPlayer->GetPosition().y, 1000.0f, 0.001f);
		CHECK_CLOSE(pMyPlayer->GetPosition().z, 0.0f, 0.001f);

		// 메쉬 충돌 정보 입력

		// 앞키 누름
		pController->TestSetCameraDir(vec3(0.0f, 1.0f, 0.0f));
		pController->TestKeyPress(VKEY_FORWARD);

		const float fDelta = 1.0f;
		tg.Update(fDelta);

		pController->TestKeyRelease(VKEY_FORWARD);
		float fMyPlayerDist = pMyPlayer->GetMoveSpeed(MOVEMENTFLAG_FORWARD) * fDelta;
		CHECK_CLOSE(pMyPlayer->GetPosition().y, 1000.0f + fMyPlayerDist, 0.001f);

		// MyCharacter가 이동중일때 사용할수 있는 MovableProc 확인하기.
		const XMovableFactor& movableFactor = pModuleMovable->GetMovableFactor();
		const XMovableFactor::SInterpolation& sInterpolation = movableFactor.INTERPOLATION;
		CHECK_EQUAL(sInterpolation.m_bWorking, false);

		CHECK_EQUAL(sInterpolation.m_bFinishMove, true);
		CHECK_EQUAL(sInterpolation.m_bFinishRotate, true);
	}

	TEST(TestMyPlayerNormalJump)
	{
		minet::MTestNetAgent	net_agent(global.net->GetClient());
		net_agent.IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		TestGame		tg;

		XMyPlayer* pMyPlayer = tg.GetMyPlayer();
		if (pMyPlayer == NULL) return;

		pMyPlayer->SetPosition(vec3(1000.0f, 1000.0f, 0.0f));
		pMyPlayer->SetDirection(vec3(0.0f, 1.0f, 0.0f));

		XModuleMyControl* pModuleMyControl = pMyPlayer->GetModuleMyControl();
		CHECK(pModuleMyControl != NULL);
		if (pModuleMyControl == NULL) return;
		XModuleMovable* pModuleMovable = (XModuleMovable*)pMyPlayer->GetModule(XMID_MOVABLE);
		CHECK(pModuleMovable != NULL);
		if (pModuleMovable == NULL) return;

		// 현재속도 / 가속도
		const vec3& vCurrentVel = pModuleMovable->GetMovableFactor().VELOCITY.m_vVelocity;
		const vec3& vCurrentAccel = pModuleMovable->GetMovableFactor().VELOCITY.m_vAccel;
		const XMovableFactor::SState& sCurrentGeneralState = pModuleMovable->GetMovableFactor().GENERAL_STATE;

		MockController* pController = tg.GetController();
		pController->TestSetCameraDir(vec3(0.0f, 1.0f, 0.0f));

		// 초기 속도 - 0, 0, 0
		vec3 vInitVel = pModuleMovable->GetMovableFactor().VELOCITY.m_vVelocity;
		CHECK_CLOSE(vInitVel.x, 0.f, 0.001);
		CHECK_CLOSE(vInitVel.y, 0.f, 0.001);
		CHECK_CLOSE(vInitVel.z, 0.f, 0.001);

		// 초기 가속도 0, 0, GRAVITY
		vec3 vInitAccel = pModuleMovable->GetMovableFactor().VELOCITY.m_vAccel;
		CHECK_CLOSE(vInitAccel.x, 0.f, 0.001);
		CHECK_CLOSE(vInitAccel.y, 0.f, 0.001);
		CHECK_CLOSE(vInitAccel.z, GRAVITY, 0.001);

		// 초기 충돌 상태
		CHECK_EQUAL(sCurrentGeneralState.m_bColTerrain, false);
		CHECK_EQUAL(sCurrentGeneralState.m_bColObject, false);

		// 점프 키 누름
		pController->TestKeyPress(VKEY_JUMP);

		// 점프에 의해 설정된 속도
		// 점프 전의 속도는 초기 속도였으므로 0, 0, 0
		vec3 vecJumpVel = vec3(0*XCONST::MOVE_SPEED_NONE, 0*XCONST::MOVE_SPEED_NONE , XCONST::JUMP_SPEED);
		CHECK_CLOSE(vecJumpVel.x, vCurrentVel.x, 0.001);
		CHECK_CLOSE(vecJumpVel.y, vCurrentVel.y, 0.001);
		CHECK_CLOSE(vecJumpVel.z, vCurrentVel.z, 0.001);

		// 점프에 의해 설정되는 가속도. 초기와 동일해야 한다.
		CHECK_CLOSE(vInitAccel.x, vCurrentAccel.x, 0.001);
		CHECK_CLOSE(vInitAccel.y, vCurrentAccel.y, 0.001);
		CHECK_CLOSE(vInitAccel.z, vCurrentAccel.z, 0.001);

		//////////////////////////////////////////////////////////////////////////

		// 시간이 지난후에 속도 계산하기
		float fElapsed = 0.01f;
		vec3 vecExpectedVel = vCurrentVel + (vCurrentAccel * fElapsed);

		// 업데이트
		tg.Update(fElapsed);
		CHECK_EQUAL(pModuleMyControl->GetCurrentActionStateID(), ACTION_STATE_JUMP);
		CHECK(net_agent.IsExistSendCommand(MC_ACTION_JUMP_REQ) == true);

		// 업데이트후 체크
		CHECK(pMyPlayer->GetPosition().z > 0.0f);
		CHECK_CLOSE(vecExpectedVel.x, vCurrentVel.x, 0.001);
		CHECK_CLOSE(vecExpectedVel.y, vCurrentVel.y, 0.001);
		CHECK_CLOSE(vecExpectedVel.z, vCurrentVel.z, 0.001);


		// 0.01이라는 시간이라면, 올라가고 있는 중이다.
		CHECK(vecExpectedVel.z > 0.f);
		CHECK_EQUAL(sCurrentGeneralState.m_bJumpAscending, true);
		CHECK_EQUAL(sCurrentGeneralState.m_bColTerrain, false);
		CHECK_EQUAL(sCurrentGeneralState.m_bColObject, false);


		// 속도의 z값이 0이 되면 떨어지기 시작한다.
		// z 가 0 이하인 fElapsed 구하기
		fElapsed = - vCurrentVel.z / vCurrentAccel.z;
		fElapsed += 0.0001f;

		// 낙하(falling) 할때까지 업데이트
		tg.Update(fElapsed);

		// 이제부터는 낙하
		CHECK_EQUAL(sCurrentGeneralState.m_bJumping, true);
		CHECK_EQUAL(sCurrentGeneralState.m_bJumpAscending, false);

		// 점프해서 떨어지는 것과 그냥 IDLE 상태에서 떨어지는것이 있다.(애니메이션 변경등을 위해 다른 경우로 처리)
		CHECK_EQUAL(sCurrentGeneralState.m_bFallingButNotJump, false);
		CHECK_EQUAL(sCurrentGeneralState.m_bLanding, false);
		CHECK_EQUAL(sCurrentGeneralState.m_bColTerrain, false);
		CHECK_EQUAL(sCurrentGeneralState.m_bColObject, false);

		// 착지(landing) 할때까지 업데이트.
		// 2초라는 시간은 떨어져서 지형에 충돌을 해서 착지하는데 충분한 시간이다.
		// mock world에는 지형은 있으나 물체는 없다.
		tg.Update(2.f);
		CHECK_EQUAL(sCurrentGeneralState.m_bColTerrain, true);
		CHECK_EQUAL(sCurrentGeneralState.m_bColObject, false);

		CHECK_EQUAL(sCurrentGeneralState.m_bJumping, false);
		CHECK_EQUAL(sCurrentGeneralState.m_bJumpAscending, false);
		CHECK_EQUAL(sCurrentGeneralState.m_bFallingButNotJump, false);
		CHECK_EQUAL(sCurrentGeneralState.m_bLanding, true);

		// 착지애니메이션 끝날때까지 업데이트
		// 10초라는 시간은 착지 애니메이션이 끝나기에 충분한 시간이다.
		// 애니메이션이 끝나고 XModuleMotion::PlayDone이 호출되면 XMyAction  상태가 원래인 IDLE로 변경된다.
		tg.Update(10.f);
		CHECK_EQUAL(pModuleMyControl->GetCurrentActionStateID(), ACTION_STATE_IDLE);
	}

	TEST(TestMyPlayerKnockbackDie)
	{
		minet::MTestNetAgent	net_agent(global.net->GetClient());
		net_agent.IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		TestGame		tg;
		TestModuleContext testContext(&tg, &net_agent);
		XUnitTestHelper helper;

		XMyPlayer* pMyPlayer = tg.GetMyPlayer();
		if (pMyPlayer == NULL) return;

		pMyPlayer->SetPosition(vec3(1000.0f, 1000.0f, 0.0f));
		pMyPlayer->SetDirection(vec3(0.0f, 1.0f, 0.0f));

		// npc 정보 추가
		XUnitTestHelper::NewNPCInfo(1);

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		XNonPlayer* pNPC = helper.InNPC(uidNPC, 1, vec3(1000.0f, 1000.0f, 0.0f));

		// 탤런트 정보 추가
		int ID = XUnitTestUtil::NewID();
		helper.CreateTalentInfo(ID, L"<TALENT id=\"211006\" name=\"대시\" npc=\"110\" TalentType=\"skill\" Category=\"melee\" SkillType=\"melee\" \
									EffectTiming=\"hit_this_talent\" EffectRadius=\"2\" HitTestType=\"area\" HitMotionDelay=\"0\" TargetsAllowed=\"enemy\" \
									HPCost=\"0\" APCost=\"0\" ENCost=\"0\" STACost=\"0\" HPTickCost=\"0\" ENTickCost=\"0\" STATickCost=\"0\" CoolTime=\"5\" \
									UseAni=\"atk_d\" AttackDirection=\"front\" IsMoving=\"True\" IsSetDirection=\"True\" MF_BEATEN=\"50\" MF_WEIGHT_BEATEN=\"2000\" \
									MF_KNOCKBACK=\"0\" MF_WEIGHT_KNOCKBACK=\"0\" MF_STUN=\"0\" MF_WEIGHT_STUN=\"0\" MF_DEFLECT=\"0\" MF_WEIGHT_DEFLECT=\"0\" \
									MF_THROWUP=\"200\" MF_WEIGHT_THROWUP=\"1000\" MF_KNOCKDOWN=\"0\" MF_WEIGHT_KNOCKDOWN=\"0\" \
									MF_UPPERED=\"0\" MF_WEIGHT_UPPERED=\"0\" MinDamage=\"75\" MaxDamage=\"80\" DamageAttrib=\"blunt\" Guardable=\"True\" Cancelable=\"False\" />"
								);

		//////////////////////////////////////////////////////////////////////////
		// 넉백 히트를 보낸다.
		TD_TALENT_HIT	td_talent_hit;
		td_talent_hit.nAttackerUIID = pNPC->GetUIID();
		td_talent_hit.nVictimUIID = XGetMyUIID();
		td_talent_hit.nTalentID = 211006;
		td_talent_hit.nDamage = 45;
		td_talent_hit.nFlags = 0;
		td_talent_hit.nMFState = MF_KNOCKBACK;
		td_talent_hit.nMFWeight = 100.0;
		td_talent_hit.nHitTime = 0;

		net_agent.OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));

		// 죽었다.
		net_agent.OnRecv( MC_ACTION_DIE, 2, NEW_USHORT(XGetMyUIID()), NEW_UID(MUID::Invalid()));

		tg.Update(1.0f);

		// 체크
		// 현재 액션 상태가 죽은 상태인가?
		XModuleMyControl* pModuleMyControl = testContext.GetModule<XModuleMyControl>(pMyPlayer);
		CHECK(pModuleMyControl->GetCurrentActionStateID() == ACTION_STATE_DIE);

		helper.Clear();
	}

	TEST(TestMyPlayerStunDie)
	{
		// 스턴 중에 죽었을때...
		minet::MTestNetAgent	net_agent(global.net->GetClient());
		net_agent.IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		TestGame		tg;
		TestModuleContext testContext(&tg, &net_agent);
		XUnitTestHelper helper;

		XMyPlayer* pMyPlayer = tg.GetMyPlayer();
		if (pMyPlayer == NULL) return;

		pMyPlayer->SetPosition(vec3(1000.0f, 1000.0f, 0.0f));
		pMyPlayer->SetDirection(vec3(0.0f, 1.0f, 0.0f));

		// npc 정보 추가
		XUnitTestHelper::NewNPCInfo(1, L"렌고트 병정", 3, L"goblin_mage", L"Monster/goblin_mage", L"monster");

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		XNonPlayer* pNPC = helper.InNPC(uidNPC, 1, vec3(1000.0f, 1000.0f, 0.0f));

		// 탤런트 정보 추가
		int ID = XUnitTestUtil::NewID();
		helper.CreateTalentInfo(ID, L"<TALENT id=\"211000111\" name=\"엉덩방아\" npc=\"110001\" TalentType=\"skill\" Category=\"melee\" SkillType=\"melee\" \
									EffectTiming=\"hit_this_talent\" EffectRadius=\"2\" HitTestType=\"area\" HitMotionDelay=\"0\" TargetsAllowed=\"enemy\" HPCost=\"0\"\
									APCost=\"0\" ENCost=\"0\" STACost=\"0\" HPTickCost=\"0\" ENTickCost=\"0\" STATickCost=\"0\" CoolTime=\"5\" UseAni=\"talent_fatalgas\" \
									AttackDirection=\"front\" MF_BEATEN=\"0\" MF_WEIGHT_BEATEN=\"0\" MF_KNOCKBACK=\"0\" MF_WEIGHT_KNOCKBACK=\"0\" MF_STUN=\"150\" MF_WEIGHT_STUN=\"5000\" \
									MF_DEFLECT=\"0\" MF_WEIGHT_DEFLECT=\"0\" MF_THROWUP=\"0\" MF_WEIGHT_THROWUP=\"0\" MF_KNOCKDOWN=\"0\" \
									MF_WEIGHT_KNOCKDOWN=\"0\" MF_UPPERED=\"0\" MF_WEIGHT_UPPERED=\"0\" MinDamage=\"50\" MaxDamage=\"50\" DamageAttrib=\"poison\" Guardable=\"False\" \
									Cancelable=\"False\" />"
									);

		//////////////////////////////////////////////////////////////////////////
		// 스턴 히트를 보낸다.
		TD_TALENT_HIT	td_talent_hit;
		td_talent_hit.nAttackerUIID = pNPC->GetUIID();
		td_talent_hit.nVictimUIID = XGetMyUIID();
		td_talent_hit.nTalentID = 211000111;
		td_talent_hit.nDamage = 45;
		td_talent_hit.nFlags = 0;
		td_talent_hit.nMFState = MF_STUN;
		td_talent_hit.nMFWeight = 5.0f;
		td_talent_hit.nHitTime = 0;

		net_agent.OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));

		XModuleMyControl* pModuleMyControl = testContext.GetModule<XModuleMyControl>(pMyPlayer);
		//////////////////////////////////////////////////////////////////////////
		// 체크
		// 현재 액션 상태가 히트 상태인가?
		CHECK(pModuleMyControl->GetCurrentActionStateID() == ACTION_STATE_HIT);

		// 죽었다.
		net_agent.OnRecv( MC_ACTION_DIE, 2, NEW_USHORT(XGetMyUIID()), NEW_UID(MUID::Invalid()));

		tg.Update(1.0f);

		//////////////////////////////////////////////////////////////////////////
		// 체크
		// 현재 액션 상태가 죽은 상태인가?
		CHECK(pModuleMyControl->GetCurrentActionStateID() == ACTION_STATE_DIE);

		helper.Clear();

	}
}
