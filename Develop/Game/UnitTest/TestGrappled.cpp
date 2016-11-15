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
#include "XModuleNetControl.h"
#include "TestModule.h"
#include "FTestGame.h"

SUITE(TestGrappled)
{
	class TestGrappled : public FTestGame
	{
	public:
		TestGrappled()
		{
			info.motion->AddMotionForTest(L"<MOTION name=\"mf_grappled\" stance=\"all\" loop=\"false\" cancelEnabled=\"true\" cameraLook=\false\">\
											<Default>\
											<Sequence ani=\"MF_KB2\" event=\"mf_throw_event\" />\
											</Default>\
											</MOTION>");

			info.motion->AddMotionForTest(L"<MOTION name=\"mf_grappled_collision\" stance=\"all\" loop=\"false\" cancelEnabled=\"true\" cameraLook=\"false\">\
											<Default>\
											<Sequence ani=\"MF_TU2\">\
											<PostTrigger if=\"Dead\" then=\"Hold\"/>\
											</Sequence>\
											</Default>\
											</MOTION>");
			info.motion->AddMotionForTest(L"  <MOTION name=\"mf_stand_down\" stance=\"all\" loop=\"false\" cancelEnabled=\"true\" cameraLook=\"false\">\
											<Default>\
											<Sequence ani=\"MF_KD2\" />\
											</Default>\
											</MOTION>");
			info.motion->AddMotionForTest(L"  <MOTION name=\"mf_stand_up\" stance=\"all\" loop=\"false\" cancelEnabled=\"true\" cameraLook=\"false\">\
											<Default>\
											<Sequence ani=\"MF_KD3\" />\
											</Default>\
											</MOTION>");
			m_pNetAgent->IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

			MakeTestTalents();
			MakeNPCInfo();

		}
		virtual ~TestGrappled()
		{

		}
		void MakeTestTalents()
		{
			m_nAttackTalentID = 1;

			// 탤런트 정보 추가
			m_Helper.CreateTalentInfo(211015, L"<TALENT id=\"211015\" name=\"잡아 던지기\" npc=\"110\" TalentType=\"skill\" Category=\"melee\" SkillType=\"extra_active\"\
														 EffectTiming=\"hit_this_talent\" ExtraActive=\"GRAPPLE\" ExtraActiveValue=\"GRAPPLE\" HitMotionDelay=\"0\" HPCost=\"0\" \
														 APCost=\"0\" ENCost=\"0\" STACost=\"0\" HPTickCost=\"0\" ENTickCost=\"0\" STATickCost=\"0\" CoolTime=\"0\" UseAni=\"atk_grip_fail\" \
														 ExtraAni=\"atk_grip\" AttackDirection=\"front\" FORCE_MF=\"Grappled\" FORCE_MF_WEIGHT=\"2500\" MF_BEATEN=\"0\" MF_WEIGHT_BEATEN=\"0\"\
														 MF_KNOCKBACK=\"0\" MF_WEIGHT_KNOCKBACK=\"0\" MF_STUN=\"0\" MF_WEIGHT_STUN=\"0\" MF_DEFLECT=\"0\" MF_WEIGHT_DEFLECT=\"0\" \
														 MF_THROWUP=\"0\" MF_WEIGHT_THROWUP=\"0\" MF_KNOCKDOWN=\"0\" MF_WEIGHT_KNOCKDOWN=\"0\" MF_UPPERED=\"0\" \
														 MF_WEIGHT_UPPERED=\"0\" MinDamage=\"30\" MaxDamage=\"40\" DodgeDifficulty=\"240\" CriticalEnable=\"True\" DamageAttrib=\"blunt\" \
														 Guardable=\"True\" Cancelable=\"False\" UseBloodEffect=\"False\" EffecteePoint=\"hitcapsule\" EffecteeRelation=\"enemy\" />");


		}
		void MakeNPCInfo()
		{
			NewNPCInfo(110, L"SampleNPC", 3, L"Ogre_B", L"Monster/Ogre_B");
		}

		int m_nAttackTalentID;
	};

	TEST_FIXTURE(TestGrappled, TestMyPlayerGrappled_Start)
	{
		// non player
		XNonPlayer* pNonPlayer = NewNonPlayer(110, vec3(0.f, 0.f, 0.0f), vec3(1.f, 0.f, 0.f));
		CHECK(pNonPlayer != NULL);

		TD_TALENT_HIT	td_talent_hit;
		td_talent_hit.nAttackerUIID	= pNonPlayer->GetUIID();
		td_talent_hit.nVictimUIID	= GetMyPlayer()->GetUIID();

		td_talent_hit.nTalentID		= m_nAttackTalentID;
		td_talent_hit.nDamage		= 45;
		td_talent_hit.nFlags		= 0;
		td_talent_hit.nMFState		= MF_GRAPPLED;
		td_talent_hit.nMFWeight		= 0;
		td_talent_hit.nHitTime		= 0;

		m_pNetAgent->OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));

		XModuleMyControl* pModuleMyControl = m_pTestContext->GetModule<XModuleMyControl>(GetMyPlayer());
		CHECK( pModuleMyControl != NULL);
		if (pModuleMyControl == NULL) return;

		CHECK ( pModuleMyControl->GetCurrentActionStateID() == ACTION_STATE_GRAPPLED);
	}

	TEST_FIXTURE(TestGrappled, TestNetPlayerGrappled_Start)
	{
		// non player
		XNonPlayer* pNonPlayer = NewNonPlayer(110, vec3(0.f, 0.f, 0.0f), vec3(1.f, 0.f, 0.f));
		CHECK(pNonPlayer != NULL);

		// net player
		XNetPlayer* pNetPlayer = NewNetPlayer(vec3(0.f, 0.f, 0.0f), vec3(1.f, 0.f, 0.f), SEX_FEMALE);
		CHECK(pNetPlayer != NULL);

		TD_TALENT_HIT	td_talent_hit;
		td_talent_hit.nAttackerUIID	= pNonPlayer->GetUIID();
		td_talent_hit.nVictimUIID	= pNetPlayer->GetUIID();
		td_talent_hit.nTalentID		= m_nAttackTalentID;
		td_talent_hit.nDamage		= 45;
		td_talent_hit.nFlags		= 0;
		td_talent_hit.nMFState		= MF_GRAPPLED;
		td_talent_hit.nMFWeight		= 0;
		td_talent_hit.nHitTime		= 0;

		m_pNetAgent->OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));

		XModuleNetControl* pModuleNetControl = m_pTestContext->GetModule<XModuleNetControl>(pNetPlayer);
		CHECK( pModuleNetControl != NULL);
		if (pModuleNetControl == NULL) return;

		CHECK ( pModuleNetControl->GetCurrentActionStateID() == NET_ACTION_STATE_GRAPPLED);
	}
}
