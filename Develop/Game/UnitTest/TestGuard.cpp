#include "stdafx.h"
#include "XNPCInfo.h"
#include "XUnitTestHelper.h"
#include "XSystem.h"
#include "XCharacterCaption.h"
#include "MockNetClient.h"
#include "XNonPlayer.h"
#include "XUnitTestUtil.h"
#include "XMyPlayer.h"
#include "XModuleMovable.h"
#include "XModuleAction.h"
#include "XModuleMotion.h"
#include "TestModule.h"
#include "FTestGame.h"

SUITE(TestGuard)
{
	class FTestGuard : public FTestGame
	{
	public:
		FTestGuard()
		{
			info.motion->AddMotionForTest(L"	<MOTION name='guarded' stance='battle'>		\
										  <Default>															\
										  <Sequence ani='guard_block'/>												\
										  </Default>															\
										  </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='idle' stance='all'>		\
										  <Default>															\
										  <Sequence ani='idle'/>												\
										  </Default>															\
										  </MOTION>");
			info.motion->AddMotionForTest(L"<MOTION name='guard_r' stance='battle' loop='false' cancelEnabled='true' cameraLook='true'><Default><Sequence ani='guard_end'/></Default></MOTION>");

			m_pNetAgent->IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

			MakeTestTalents();

		}
		virtual ~FTestGuard()
		{

		}
		void MakeTestTalents()
		{
			m_nAttackTalentID = 211000111;
			m_nGuardTalentID = 101060;

			// 탤런트 정보 추가
			m_Helper.CreateTalentInfo(m_nAttackTalentID, L"<TALENT id=\"211000111\" name=\"엉덩방아\" npc=\"110001\" TalentType=\"skill\" Category=\"melee\" SkillType=\"melee\" \
										  EffectTiming=\"hit_this_talent\" EffectRadius=\"2\" HitTestType=\"area\" HitMotionDelay=\"0\" TargetsAllowed=\"enemy\" HPCost=\"0\"\
										  APCost=\"0\" ENCost=\"0\" STACost=\"0\" HPTickCost=\"0\" ENTickCost=\"0\" STATickCost=\"0\" CoolTime=\"5\" UseAni=\"talent_fatalgas\" \
										  AttackDirection=\"front\" MF_BEATEN=\"0\" MF_WEIGHT_BEATEN=\"0\" MF_KNOCKBACK=\"0\" MF_WEIGHT_KNOCKBACK=\"0\" MF_STUN=\"150\" MF_WEIGHT_STUN=\"5000\" \
										  MF_DEFLECT=\"0\" MF_WEIGHT_DEFLECT=\"0\" MF_THROWUP=\"0\" MF_WEIGHT_THROWUP=\"0\" MF_KNOCKDOWN=\"0\" \
										  MF_WEIGHT_KNOCKDOWN=\"0\" MF_UPPERED=\"0\" MF_WEIGHT_UPPERED=\"0\" MinDamage=\"50\" MaxDamage=\"50\" DamageAttrib=\"poison\" Guardable=\"False\" \
										  Cancelable=\"False\" />"
										  );


			m_Helper.CreateTalentInfo(m_nGuardTalentID, L"<TALENT id=\"101060\" name=\"1hs 부분 가드 Rank0\" Rank=\"1\" TalentType=\"skill\" Category=\"guard\" SkillType=\"guard\" \
										  NeedTraining=\"False\" IgnoreDisabled=\"True\" EffectTiming=\"guard_me\" WeaponAllowed=\"1hs\" Stance=\"combat\" GuardDirection=\"front\" \
										  Guardable=\"True\" Cancelable=\"True\" EffecteeRelation=\"enemy\" />");

		}
		int m_nAttackTalentID;
		int m_nGuardTalentID;
	};
	TEST_FIXTURE(FTestGuard, TestNetPlayerGuard_Start)
	{
		// net player
		XNetPlayer* pNetPlayer = NewNetPlayer(vec3(0.f, 0.f, 0.0f), vec3(1.f, 0.f, 0.f), SEX_FEMALE);
		CHECK(pNetPlayer != NULL);

		m_pNetAgent->OnRecv(MC_ACTION_GUARD, 3, NEW_USHORT(pNetPlayer->GetUIID()), NEW_SVEC2(pNetPlayer->GetDirection()), NEW_VEC(pNetPlayer->GetPosition()));

		CHECK(pNetPlayer->GetGuard());
	}

	TEST_FIXTURE(FTestGuard, TestMyPlayerGuardDefense_KeepGuard)
	{
		// npc 정보 추가
		NewNPCInfo(1);

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		XNonPlayer* pNPC = m_Helper.InNPC(uidNPC, 1, vec3(1000.0f, 1000.0f, 0.0f));

		// My player
		XMyPlayer* pMyPlayer = m_TestGame.GetMyPlayer();

		XModuleMotion* pModuleMotion = m_pTestContext->GetModule<XModuleMotion>(pMyPlayer);
		CHECK( pModuleMotion != NULL);
		if (pModuleMotion == NULL) return;

		XUnitTestHelper::CreateWeaponItemData(1, WEAPON_1H_SLASH);
		XUnitTestHelper::CreateItemAndEquip(pMyPlayer, 1, ITEMSLOT_RWEAPON);

		pMyPlayer->ChangeStance(CS_BATTLE, true);

		MockController* pController = m_TestGame.GetController();
		pController->TestGuardAction();

		TD_TALENT_GUARD td_talent_guard;

		td_talent_guard.nType			= GT_PERFECT;
		td_talent_guard.nAttackerUIID	= pNPC->GetUIID();
		td_talent_guard.nGuardUIID		= pMyPlayer->GetUIID();
		td_talent_guard.nTalentID		= m_nAttackTalentID;
		td_talent_guard.nHitTime		= 0;

		m_pNetAgent->OnRecv(MC_ACTION_GUARD_DEFENSE, 1,  NEW_SINGLE_BLOB(&td_talent_guard, sizeof(TD_TALENT_GUARD)));

		XModuleMyControl* pModuleMyControl = m_pTestContext->GetModule<XModuleMyControl>(pMyPlayer);
		CHECK( pModuleMyControl != NULL);
		if (pModuleMyControl == NULL) return;

		CHECK(pModuleMyControl->IsCurrentGuard());
		CHECK(pModuleMotion->GetCurrMotion() == MOTION_NAME_GUARD_BLOCK);

		m_Helper.Clear();
	}

	TEST_FIXTURE(FTestGuard, TestNetPlayerGuardDefense_KeepGuard)
	{
		// npc 정보 추가
		NewNPCInfo(1);


		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		XNonPlayer* pNPC = m_Helper.InNPC(uidNPC, 1, vec3(1000.0f, 1000.0f, 0.0f));


		// net player
		XNetPlayer* pNetPlayer = NewNetPlayer(vec3(0.f, 0.f, 0.0f), vec3(1.f, 0.f, 0.f), SEX_FEMALE);
		CHECK(pNetPlayer != NULL);

		XModuleMotion* pModuleMotion = m_pTestContext->GetModule<XModuleMotion>(pNetPlayer);
		CHECK( pModuleMotion != NULL);
		if (pModuleMotion == NULL) return;

		pModuleMotion->ChangeWeaponAniType(WEAPON_1H_SLASH);

		m_pNetAgent->OnRecv(MC_ACTION_GUARD, 3, NEW_USHORT(pNetPlayer->GetUIID()), NEW_SVEC2(pNetPlayer->GetDirection()), NEW_VEC(pNetPlayer->GetPosition()));

		TD_TALENT_GUARD td_talent_guard;

		td_talent_guard.nType			= GT_PERFECT;
		td_talent_guard.nAttackerUIID	= pNPC->GetUIID();
		td_talent_guard.nGuardUIID		= pNetPlayer->GetUIID();
		td_talent_guard.nTalentID		= m_nAttackTalentID;
		td_talent_guard.nHitTime		= 0;

		m_pNetAgent->OnRecv(MC_ACTION_GUARD_DEFENSE, 1,  NEW_SINGLE_BLOB(&td_talent_guard, sizeof(TD_TALENT_GUARD)));

		CHECK(pNetPlayer->GetGuard());
		CHECK(pModuleMotion->GetCurrMotion() == MOTION_NAME_GUARD_BLOCK);

		m_Helper.Clear();
	}

	TEST_FIXTURE(FTestGuard, TestMyPlayerGuardDefense_NoGuard)
	{
		// npc 정보 추가
		NewNPCInfo(1);

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		XNonPlayer* pNPC = m_Helper.InNPC(uidNPC, 1, vec3(1000.0f, 1000.0f, 0.0f));

		// My player
		XMyPlayer* pMyPlayer = GetMyPlayer();

		XModuleMotion* pModuleMotion = m_pTestContext->GetModule<XModuleMotion>(pMyPlayer);
		CHECK( pModuleMotion != NULL);
		if (pModuleMotion == NULL) return;

		XUnitTestHelper::CreateWeaponItemData(1, WEAPON_1H_SLASH);
		XUnitTestHelper::CreateItemAndEquip(pMyPlayer, 1, ITEMSLOT_RWEAPON);

		pMyPlayer->ChangeStance(CS_BATTLE, true);

		MockController* pController = m_TestGame.GetController();
		pController->TestGuardAction();

		TD_TALENT_GUARD td_talent_guard;

		td_talent_guard.nType			= GT_PERFECT;
		td_talent_guard.nAttackerUIID	= pNPC->GetUIID();
		td_talent_guard.nGuardUIID		= pMyPlayer->GetUIID();
		td_talent_guard.nTalentID		= m_nGuardTalentID;
		td_talent_guard.nHitTime		= 0;

		m_pNetAgent->OnRecv(MC_ACTION_GUARD_DEFENSE, 1,  NEW_SINGLE_BLOB(&td_talent_guard, sizeof(TD_TALENT_GUARD)));

		XModuleMyControl* pModuleMyControl = m_pTestContext->GetModule<XModuleMyControl>(pMyPlayer);
		CHECK( pModuleMyControl != NULL);
		if (pModuleMyControl == NULL) return;

		CHECK_EQUAL(true, pModuleMyControl->IsCurrentGuard());
		CHECK(MOTION_NAME_GUARD_BLOCK == pModuleMotion->GetCurrMotion());

		Update(0.3f);

		pController->TestGuardRelease();

		Update(0.3f);
		Update(0.3f);
		Update(0.1f);
		CHECK(MOTION_NAME_IDLE == pModuleMotion->GetCurrMotion());

		m_Helper.Clear();
	}

	TEST_FIXTURE(FTestGuard, TestNetPlayerGuardDefense_NoGuard)
	{
		m_pNetAgent->IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		// npc 정보 추가
		NewNPCInfo(1);

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		XNonPlayer* pNPC = m_Helper.InNPC(uidNPC, 1, vec3(1000.0f, 1000.0f, 0.0f));

		// net player
		XNetPlayer* pNetPlayer = NewNetPlayer(vec3(0.f, 0.f, 0.0f), vec3(1.f, 0.f, 0.f), SEX_FEMALE);
		CHECK(pNetPlayer != NULL);

		XModuleMotion* pModuleMotion = m_pTestContext->GetModule<XModuleMotion>(pNetPlayer);
		CHECK( pModuleMotion != NULL);
		if (pModuleMotion == NULL) return;

		pModuleMotion->ChangeWeaponAniType(WEAPON_1H_SLASH);

		TD_TALENT_GUARD td_talent_guard;

		td_talent_guard.nType			= GT_PERFECT;
		td_talent_guard.nAttackerUIID	= pNPC->GetUIID();
		td_talent_guard.nGuardUIID		= pNetPlayer->GetUIID();
		td_talent_guard.nTalentID		= m_nGuardTalentID;
		td_talent_guard.nHitTime		= 0;

		m_pNetAgent->OnRecv(MC_ACTION_GUARD_DEFENSE, 1,  NEW_SINGLE_BLOB(&td_talent_guard, sizeof(TD_TALENT_GUARD)));

		CHECK(pNetPlayer->GetGuard() == false);
		CHECK(pModuleMotion->GetCurrMotion() == MOTION_NAME_GUARD_BLOCK);

		Update(0.3f);
		Update(0.1f);
		CHECK(pModuleMotion->GetCurrMotion() == MOTION_NAME_IDLE);

		m_Helper.Clear();
	}
}
