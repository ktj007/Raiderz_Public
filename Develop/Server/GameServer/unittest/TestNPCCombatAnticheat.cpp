#include "StdAfx.h"
#include "GTestForward.h"

SUITE(NPCCombatAnticheat)
{
	TEST(NPCCombatAnticheat_CheckCheat)
	{
		MockField*	m_pField = GUTHelper_Field::DefaultMockField();
		GEntityNPC* pNPC = GUTHelper_NPC::SpawnNPC(m_pField);
		GEntityNPC* pTargetNPC = GUTHelper_NPC::SpawnNPC(m_pField);
		pNPC->SetPos(vec3(0,0,0));
		pTargetNPC->SetPos(vec3(0,0,0));
		
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);
		test.talent.UseTalent(pTargetNPC, p1DmgTalent, pNPC->GetUID()); // 전투 돌입
		
		CHECK_EQUAL(false, pNPC->IsNowInvincibility());
		
		pTargetNPC->SetPos(vec3(10001,10001,0));
		for (int i=0; i<5; ++i)
			test.talent.UseTalent(pNPC, p1DmgTalent, pTargetNPC->GetUID());

		CHECK_EQUAL(true, pNPC->IsNowInvincibility());
		
	}
}