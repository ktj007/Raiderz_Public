
#include "stdafx.h"
#include "GTestForward.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GNullGlueActor.h"
#include "GNullGlueNPC.h"
#include "GNullGluePlayer.h"
#include "GNullGlueField.h"
#include "GNullGlueQuestPVP.h"
#include "GScriptUtil.h"
#include "GQuestPVP.h"


SUITE(ScriptNullObject)
{
	struct Fixture
	{
		Fixture() : m_field(SUnitTestUtil::NewUID(), NULL)
		{
			gsys.pScriptManager->Init();
			m_actor.CreateDelegator();
			m_npc.CreateDelegator();
			m_player.CreateDelegator();
			m_field.CreateDelegator();
		}

		~Fixture() 
		{
			gsys.pScriptManager->Fini();
		}

		GEntityActor m_actor;
		GNullGlueActor m_null_actor;
		GEntityNPC m_npc;
		GNullGlueNPC m_null_npc;
		GEntityPlayer m_player;
		GNullGluePlayer m_null_player;
		GField m_field;
		GNullGlueField m_null_field;
		GNullGlueQuestPVP m_null_questpvp;
	};

	TEST_FIXTURE(Fixture, PassNullActor)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	obj:GetHP()\n"
			"end\n"
			);

		// raw-pointer 전달

		WCALL1(WLUA, "OnEvent", m_actor.GetDelegator(), WNULL);
		WCALL1(WLUA, "OnEvent", m_null_actor, WNULL);	// 크래쉬가 나지 않음

		// wrapped-pointer 전달
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(&m_actor), WNULL);
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GEntityActor*)NULL), WNULL);
	}

	TEST_FIXTURE(Fixture, IsNullActor)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	TEST_VAR = obj:IsNull()\n"
			"end\n"
			);

		CHECK_EQUAL(true, TO_SAFE_LUAPTR((GEntityActor*)NULL)->IsNull());
		CHECK_EQUAL(false, TO_SAFE_LUAPTR(&m_actor)->IsNull());
		CHECK_EQUAL(true, m_null_actor.IsNull());		
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GEntityActor*)NULL), WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(&m_actor), WNULL);	
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", m_null_actor, WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", m_actor.GetDelegator(), WNULL);
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
	}

	TEST_FIXTURE(Fixture, PassNullNPC)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	obj:GetHP()\n"
			"end\n"
			);
		
		// raw-pointer 전달
		WCALL1(WLUA, "OnEvent", m_npc.GetDelegator(), WNULL);
		WCALL1(WLUA, "OnEvent", m_null_npc, WNULL);	// 크래쉬가 나지 않음

		// wrapped-pointer 전달
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(&m_npc), WNULL);
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GEntityNPC*)NULL), WNULL);
	}

	TEST_FIXTURE(Fixture, IsNullNPC)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	TEST_VAR = obj:IsNull()\n"
			"end\n"
			);

		CHECK_EQUAL(true, TO_SAFE_LUAPTR((GEntityNPC*)NULL)->IsNull());
		CHECK_EQUAL(false, TO_SAFE_LUAPTR(&m_npc)->IsNull());
		CHECK_EQUAL(true, m_null_npc.IsNull());		
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GEntityNPC*)NULL), WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(&m_npc), WNULL);	
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", m_null_npc, WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", m_npc.GetDelegator(), WNULL);
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
	}

	TEST_FIXTURE(Fixture, PassNullPlayer)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	obj:GetHP()\n"
			"end\n"
			);

		// raw-pointer 전달
		WCALL1(WLUA, "OnEvent", m_player.GetDelegator(), WNULL);
		WCALL1(WLUA, "OnEvent", m_null_player, WNULL);	// 크래쉬가 나지 않음

		// wrapped-pointer 전달
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(&m_player), WNULL);
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GEntityPlayer*)NULL), WNULL);
	}

	TEST_FIXTURE(Fixture, IsNullPlayer) 
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	TEST_VAR = obj:IsNull()\n"			
			"end\n"
			);

		CHECK_EQUAL(true, TO_SAFE_LUAPTR((GEntityPlayer*)NULL)->IsNull());
		CHECK_EQUAL(false, TO_SAFE_LUAPTR(&m_player)->IsNull());
		CHECK_EQUAL(true, m_null_player.IsNull());		
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GEntityPlayer*)NULL), WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(&m_player), WNULL);	
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", m_null_player, WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", m_player.GetDelegator(), WNULL);
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
	}

	TEST_FIXTURE(Fixture, PassNullField)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	obj:GetID()\n"
			"end\n"
			);

		// raw-pointer 전달
		WCALL1(WLUA, "OnEvent", &m_field, WNULL);
		WCALL1(WLUA, "OnEvent", &m_null_field, WNULL);	// 크래쉬가 나지 않음

		// wrapped-pointer 전달
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(&m_field), WNULL);
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GField*)NULL), WNULL);
	}

	TEST_FIXTURE(Fixture, IsNullField)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	TEST_VAR = obj:IsNull()\n"
			"end\n"
			);

		CHECK_EQUAL(true, TO_SAFE_LUAPTR((GField*)NULL)->IsNull());
		CHECK_EQUAL(false, TO_SAFE_LUAPTR(&m_field)->IsNull());
		CHECK_EQUAL(true, m_null_field.IsNull());
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GField*)NULL), WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", m_field.GetDelegator(), WNULL);
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", &m_null_field, WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(&m_field), WNULL);	
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
	}

	TEST_FIXTURE(Fixture, PassNullQuestPVP)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	obj:GetID()\n"
			"end\n"
			);

		// raw-pointer 전달
		WCALL1(WLUA, "OnEvent", m_field.GetQuestPVP(), WNULL);
		WCALL1(WLUA, "OnEvent", &m_null_questpvp, WNULL);	// 크래쉬가 나지 않음

		// wrapped-pointer 전달
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(m_field.GetQuestPVP()), WNULL);
		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GQuestPVP*)NULL), WNULL);
	}

	TEST_FIXTURE(Fixture, IsNullQuestPVP)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	TEST_VAR = obj:IsNull()\n"
			"end\n"
			);

		CHECK_EQUAL(true, TO_SAFE_LUAPTR((GQuestPVP*)NULL)->IsNull());
		CHECK_EQUAL(false, TO_SAFE_LUAPTR(m_field.GetQuestPVP())->IsNull());
		CHECK_EQUAL(true, m_null_questpvp.IsNull());

		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR((GQuestPVP*)NULL), WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));

		WCALL1(WLUA, "OnEvent", m_field.GetQuestPVP()->GetDelegator(), WNULL);
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
		WCALL1(WLUA, "OnEvent", &m_null_questpvp, WNULL);	
		CHECK_EQUAL(true, WLUA->GetVar<bool>("TEST_VAR"));

		WCALL1(WLUA, "OnEvent", TO_SAFE_LUAPTR(m_field.GetQuestPVP()), WNULL);	
		CHECK_EQUAL(false, WLUA->GetVar<bool>("TEST_VAR"));
	}


	TEST_FIXTURE(Fixture, NullObjectHandleInCppSide) 
	{
		GGlueActor* glue_actor = m_actor.GetDelegator();
		float ret = glue_actor->DistanceTo(GNullGlueActor::GetNull());
		CHECK_EQUAL(-1.0f, ret);
	}

	TEST_FIXTURE(Fixture, NilHandleInCppSide) 
	{
		GGlueActor* glue_actor = m_actor.GetDelegator();
		float ret = glue_actor->DistanceTo(NULL);
		CHECK_EQUAL(-1.0f, ret);
	}

	TEST_FIXTURE(Fixture, PassNullArgument)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj)\n"
			"	TEST_VAR = obj:DistanceTo()\n"
			"end\n"
			);

		WCALL1(WLUA, "OnEvent", m_npc.GetDelegator(), WNULL);
		CHECK_EQUAL(-1, WLUA->GetVar<float>("TEST_VAR"));
	}

	TEST_FIXTURE(Fixture, PassUpcastedPointer)
	{
		WRUNSTRING(WLUA, 
			"function OnEvent(obj, hitinfo)\n"
			"	TEST_VAR = obj:DistanceTo(hitinfo.Attacker)\n"
			"end\n"
			);

		GEntityNPC local_npc;
		local_npc.CreateDelegator();

		GHitInfo hitinfo;
		hitinfo.pAttackerForLua = local_npc.GetDelegator();

		WCALL2(WLUA, "OnEvent", m_npc.GetDelegator(), hitinfo, WNULL);
		CHECK_EQUAL(0, WLUA->GetVar<float>("TEST_VAR"));
	}
}


