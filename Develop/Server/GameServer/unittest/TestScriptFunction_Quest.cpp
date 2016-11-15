#include "stdafx.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "GFieldMgr.h"
#include "GQuestInfoMgr.h"
#include "GUTHelper.h"
#include "GPlayerQuest.h"
#include "FBaseMockLink.h"
#include "GTestForward.h"

SUITE(TestScriptFunction_Quest)
{
	struct Fixture : public FBaseMockLink
	{
		Fixture()
		{ 
			gsys.pScriptManager->Init();

			m_pField = GUTHelper_Field::NewMockField();
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
			m_pQuest = test.quest.GiveNewPlayerQuest(m_pPlayer);
		}

		~Fixture() 
		{
			m_pField->Destroy();
			SAFE_DELETE(m_pField);

			gsys.pScriptManager->Fini();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		GPlayerQuest*	m_pQuest;

		GTestSysWrapper<GScriptManager>		m_ScriptMgr;
		GTestMgrWrapper<GFieldMgr>			m_FieldMgr;
		GTestMgrWrapper<GQuestInfoMgr>		m_QuestInfoMgr;
	};

	TEST_FIXTURE(Fixture, TSFQ_UpdateVar)
	{
		int nVar = 15;

		const char* pszFuncName = "Function";
		const char* pszScript = 
			"function %s(Quest, nVar)\n"
			"	return Quest:UpdateVar(nVar)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		WCALL2(WLUA, pszFuncName, m_pQuest->GetDelegator(), nVar, WNULL);
		CHECK_EQUAL(nVar, m_pQuest->GetQuestVar());
	}

	TEST_FIXTURE(Fixture, TSFQ_GetVar)
	{
		m_pQuest->UpdateQuestVar(15);

		const char* pszFuncName = "Function";
		const char* pszScript = 
			"function %s(Quest)\n"
			"	return Quest:GetVar()\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		int nVar = 0;
		WCALL1(WLUA, pszFuncName, m_pQuest->GetDelegator(), &nVar);
		CHECK_EQUAL(m_pQuest->GetQuestVar(), nVar);		
	}
}
