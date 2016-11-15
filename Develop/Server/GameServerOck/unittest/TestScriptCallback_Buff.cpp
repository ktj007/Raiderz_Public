#include "stdafx.h"
#include "FBaseScriptCallback.h"
#include "GBuffScript.h"
#include "GModuleBuff.h"
#include "CSTalentInfo.h"

SUITE(ScriptCallback_Buff)
{
	struct Fixture: public FBaseScriptCallback
	{
		Fixture()
		{
			m_pBuffInfo = test.buff.NewBuffInfo(100);
			m_pBuffInfo->m_fDuration = 1.0f;
		}
		virtual ~Fixture()	{}

		GTestMgrWrapper<GBuffInfoMgr>	m_buffInfoMgr;
		GBuffInfo*						m_pBuffInfo;
	};

	TEST_FIXTURE(Fixture, OnGain)
	{
		string strTableName = MAKE_LUATABLE_BUFF(100);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Buff_100:OnGain(User, Target)\n"
			"	TEST_VAR = 1;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = test.player.NewPlayer(m_pField);
		pPlayer->GetModuleBuff()->GainBuff(m_pBuffInfo, CSBuffEnchantInfo(), NULL, pPlayer);

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
	}

	TEST_FIXTURE(Fixture, OnExpired)
	{
		string strTableName = MAKE_LUATABLE_BUFF(100);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Buff_100:OnExpired(User, Target)\n"
			"	TEST_VAR = 1;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = test.player.NewPlayer(m_pField);
		pPlayer->GetModuleBuff()->GainBuff(m_pBuffInfo, CSBuffEnchantInfo(), NULL, pPlayer);
		pPlayer->GetModuleBuff()->Update(m_pBuffInfo->m_fDuration);

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
	}
}


