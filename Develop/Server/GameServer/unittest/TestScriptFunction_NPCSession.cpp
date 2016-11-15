#include "stdafx.h"
#include "MockField.h"
#include "GTestForward.h"
#include "GJobMgr.h"
#include "GFieldNPCSession.h"
#include "GJobRunner.h"
#include "GJob_Idle.h"
#include "GFieldNPCSession.h"
#include "GFieldNPCSessionMgr.h"
#include "FBaseScriptCallback.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(TestScriptFunction_NPCSession)
{
	struct Fixture : FBaseScriptCallback
	{
	public :
		Fixture()
		{
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);			
		}

		~Fixture()
		{
		}

		GFieldNPCSession* NewSession(string strName = "test_session")
		{
			return new GFieldNPCSession(SUnitTestUtil::NewUID(), m_pField->GetUID(), strName);
		}

	protected:
		GEntityNPC* m_pNPC;
		GUTHelper m_Helper;
	};


	TEST_FIXTURE(Fixture, NPCSession_AddNPC)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Session, NPC)\n"
			"	Session:AddNPC(NPC)\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);

		CHECK_EQUAL(0, session->GetNPCQty());
		WCALL2(WLUA, pszTestFuncName, session, m_pNPC->GetDelegator(), WNULL);
		session->Update(0.0f);
		CHECK_EQUAL(1, session->GetNPCQty());
	}

	TEST_FIXTURE(Fixture, NPCSession_RemoveNPC)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Session, NPC)\n"
			"	Session:RemoveNPC(NPC)\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);

		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		CHECK_EQUAL(1, session->GetNPCQty());
		WCALL2(WLUA, pszTestFuncName, session, m_pNPC->GetDelegator(), WNULL);
		session->Update(0.0f);
		CHECK_EQUAL(0, session->GetNPCQty());
	}

	TEST_FIXTURE(Fixture, NPCSession_GetNPCQty)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Session, NPC)\n"
			"	return Session:GetNPCQty()\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);

		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);

		int nQty = 0;
		WCALL2(WLUA, pszTestFuncName, session, m_pNPC->GetDelegator(), &nQty);
		CHECK_EQUAL(1, nQty);
	}

	TEST_FIXTURE(Fixture, NPCSession_Blocking)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Session, NPC)\n"
			"	return Session:Blocking()\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->NonBlocking();
		
		session->Update(0.0f);
		CHECK_EQUAL(false, session->IsBlocking());
		WCALL2(WLUA, pszTestFuncName, session, m_pNPC->GetDelegator(), WNULL);
		session->Update(0.0f);
		CHECK_EQUAL(true, session->IsBlocking());
	}

	TEST_FIXTURE(Fixture, NPCSession_NonBlocking)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Session, NPC)\n"
			"	return Session:NonBlocking()\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		session->NonBlocking();

		CHECK_EQUAL(true, session->IsBlocking());
		WCALL2(WLUA, pszTestFuncName, session, m_pNPC->GetDelegator(), WNULL);
		session->Update(0.0f);
		CHECK_EQUAL(false, session->IsBlocking());
	}

	TEST_FIXTURE(Fixture, NPCSession_EndSession)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Session, NPC)\n"
			"	Session:EndSession()\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);

		session->AddNPC(m_pNPC->GetUID());
		session->Update(0.0f);
		CHECK_EQUAL(1, session->GetNPCQty());
		WCALL2(WLUA, pszTestFuncName, session, m_pNPC->GetDelegator(), WNULL);
		session->Update(0.0f);
		session->Update(0.0f);
		CHECK_EQUAL(0, session->GetNPCQty());
	}

	TEST_FIXTURE(Fixture, NPCSession_Wait)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Session, NPC)\n"
			"	Session:Wait(10.0)\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GFieldNPCSession* session = NewSession();
		m_pField->GetSession()->Add(session);
		WCALL2(WLUA, pszTestFuncName, session, m_pNPC->GetDelegator(), WNULL);
		session->Update(0.0f);
		CHECK_EQUAL(10.0f, session->GetWaitRemainTime());
	}

	TEST_FIXTURE(Fixture, NPCSession_BeginState)
	{
		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function Field_100:OnSessionScene_TestSession_Begin(field, session)\n"
			"	session:Blocking()\n"
			"	TEST_VAR = 777\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GFieldNPCSession* session = NewSession("TestSession");
		m_pField->GetSession()->Add(session);
		//WCALL2(WLUA, pszTestFuncName, session, m_pNPC->GetDelegator(), WNULL);
		m_pField->GetSession()->Update(0.0f);
		CHECK_EQUAL(777, WLUA->GetVar<int>("TEST_VAR"));
	}
}
