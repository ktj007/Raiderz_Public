#include "stdafx.h"
#include "FBaseScriptGlue.h"
#include "MockField.h"
#include "GGluePlayer.h"
#include "GGlueNPC.h"
#include "GFieldInfoMgr.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(TestScriptFunction_Utility)
{
	struct Fixture: public FBaseScriptGlue
	{
		Fixture()			{}
		virtual ~Fixture()	{}
	};

	TEST_FIXTURE(Fixture, TestGlue_LocalPosToWorldPos)
	{
		const char* pszTestFuncName = "_Test_LocalPosToWorldPos";
		const char* pszScript = 
			"function %s()\n"
			"	return Math_LocalToWorld(vec3(0,1,0), vec3(1000,1000,1000), vec3(1000,0,0));\n"			
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		vec3 vEstimatePos(2000,1000,1000);
		vec3 ret;
		WCALL(WLUA, pszTestFuncName, &ret);
		CHECK_EQUAL(ret.IsEqual(vEstimatePos), true);
	}

	TEST_FIXTURE(Fixture, TestGlue_GetDistance)
	{
		const char* pszTestFuncName = "_Test_GetDistance";
		const char* pszScript = 
			"function %s()\n"
			"	return Math_GetDistance(vec3(1000, 0, 0), vec3(5000, 0, 0));\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		float ret;
		WCALL(WLUA, pszTestFuncName, &ret);
		CHECK_EQUAL(ret, 4000.0f);
	}	

	TEST_FIXTURE(Fixture, Math_GetPolygonPoints)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Point, Dir, Distance, AngleQty)\n"
			"	return Math_GetPolygonPoints(Point, Dir, Distance, AngleQty)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vec3(1000.0f,1000.0f,1000.0f), vCommonDir);

		MWLua::table t;
		WCALL4(WLUA, pszTestFuncName, pNPC->GetPos(), pNPC->GetDir(), 100.0f, 2, &t);

		t.iteration();
		vec3 vPos;
		t.next<vec3>(&vPos);
		CHECK(vPos.IsEqual(vec3(1000.0f, 1100.0f, 1000.0f)));
		t.next<vec3>(&vPos);
		CHECK(vPos.IsEqual(vec3(1000.0f, 900.0f, 1000.0f)));
		
		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, TestGlue_AsPlayer)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return AsPlayer(Actor)\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer EntityPlayer;
		GGluePlayer Player(&EntityPlayer);
		GGluePlayer* Ret = NULL;
		WCALL1(WLUA, pszTestFuncName, Player, &Ret);
		CHECK_EQUAL(Ret->GetOwnerPlayer(), Player.GetOwnerPlayer());
	}	

	TEST_FIXTURE(Fixture, TestGlue_AsNPC)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return AsNPC(Actor)\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC EntityNPC;
		GGlueNPC NPC(&EntityNPC);
		GGlueNPC* Ret = NULL;
		WCALL1(WLUA, pszTestFuncName, NPC, &Ret);
		CHECK_EQUAL(Ret->GetOwnerNPC(), NPC.GetOwnerNPC());
	}

	TEST_FIXTURE(Fixture, TestGlue_GetDistancePoint)
	{
		const char* pszTestFuncName = "_Test_GetDistance";
		const char* pszScript = 
			"function %s()\n"
			"	return Math_GetDistancePoint(vec3(1000, 1000, 1000), vec3(0.5, 0.5, 0), 1000);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		vec3 ret;
		WCALL(WLUA, pszTestFuncName, &ret);
		CHECK_EQUAL(ret.x, 1500.0f);
		CHECK_EQUAL(ret.y, 1500.0f);
		CHECK_EQUAL(ret.z, 1000.0f);
	}	
	
	TEST_FIXTURE(Fixture, TestGlue_Format)
	{
		const char* pszTestFuncName = "_Test_Format";
		const char* pszScript =
			"function %s()\n"
			"	return Format(\"FormatMessage\", \"test1\", 2, \"test3\")\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{CHECK(FALSE && "WRunString Failed.");	return;	}

		const char* szResult;		
		WCALL(WLUA, pszTestFuncName, &szResult);
		CHECK("FormatMessage{param=test1|2|test3}" == string(szResult));
	}
}
