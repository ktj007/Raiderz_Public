#include "stdafx.h"
#include "FBaseScriptCallback.h"
#include "GScriptGroup_Quest.h"
#include "GScriptUtil.h"
#include "GPlayerQuest.h"
#include "GNullGlueQuest.h"
#include "GNullGlueNPC.h"
#include "GPlayerFieldLazyGateway.h"
#include "CSQObjectiveInfo.h"
#include "GQuestInfo.h"
#include "GTestForward.h"


const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(ScriptCallback_Quest)
{
	struct Fixture: public FBaseScriptCallback
	{
		Fixture()			{}
		virtual ~Fixture()	{}

		GTestMgrWrapper<GQuestInfoMgr> QuestInfoMgr;
		DECLWRAPPER_TalentMgr;
		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
	};

	TEST_FIXTURE(Fixture, Quest_OnBegin)
	{
		int nQuestID = 100;
		string strTableName = MAKE_LUATABLE_QUEST(nQuestID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Quest_100:OnBegin()\n"
			"	TEST_VAR = 1;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC*	pNPC = m_Helper.NewEntityNPC(m_pField);
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);
		
		GetScriptQuest()->OnBegin(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), TO_SAFE_LUAPTR(pNPC));
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
	}

	TEST_FIXTURE(Fixture, Quest_OnEnd)
	{
		int nQuestID = 100;
		string strTableName = MAKE_LUATABLE_QUEST(nQuestID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Quest_100:OnEnd()\n"
			"	TEST_VAR = 1;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC*	pNPC = m_Helper.NewEntityNPC(m_pField);
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);

		GetScriptQuest()->OnEnd(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), TO_SAFE_LUAPTR(pNPC));

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
	}

	TEST_FIXTURE(Fixture, Quest_OnComplete)
	{
		int nQuestID = 100;
		string strTableName = MAKE_LUATABLE_QUEST(nQuestID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Quest_100:OnComplete()\n"
			"	TEST_VAR = 1;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC*	pNPC = m_Helper.NewEntityNPC(m_pField);
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);

		GetScriptQuest()->OnComplete(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator());

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
	}

	TEST_FIXTURE(Fixture, Quest_OnFail)
	{
		int nQuestID = 100;
		string strTableName = MAKE_LUATABLE_QUEST(nQuestID);
		MAKE_TABLE(strTableName.c_str());


		const char* pszScript = 
			"function Quest_100:OnFail()\n"
			"	TEST_VAR = 1;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC*	pNPC = m_Helper.NewEntityNPC(m_pField);
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);

		GetScriptQuest()->OnFail(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator());

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
	}

	TEST_FIXTURE(Fixture, Quest_OnObjComplete)
	{
		int nQuestID = 100;
		string strTableName = MAKE_LUATABLE_QUEST(nQuestID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Quest_100:OnObjComplete(this, Player, ObjectiveID)\n"
			"	TEST_VAR = 1;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC*	pNPC = m_Helper.NewEntityNPC(m_pField);
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);

		GetScriptQuest()->OnObjectiveComplete(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), 1, true);	

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
	}

	TEST_FIXTURE(Fixture, Quest_OnObjUpdate)
	{
		int nQuestID = 100;
		string strTableName = MAKE_LUATABLE_QUEST(nQuestID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Quest_100:OnObjUpdate(this, ObjectiveID, Progress)\n"
			"	TEST_VAR = 1;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC*	pNPC = m_Helper.NewEntityNPC(m_pField);
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);

		GetScriptQuest()->OnObjectiveUpdate(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), 1, true, 1);

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
	}

	TEST_FIXTURE(Fixture, Quest_OnObjProgress)
	{
		int nQuestID = 100;
		string strTableName = MAKE_LUATABLE_QUEST(nQuestID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Quest_100:OnObjProgress(this, ObjectiveID)\n"
			"	TEST_VAR = 1;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC*	pNPC = m_Helper.NewEntityNPC(m_pField);
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer, nQuestID);

		GetScriptQuest()->OnObjectiveProgress(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), 1, true);

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
	}

	TEST_FIXTURE(Fixture, Quest_OnObjComplete_ByKillNPC_PlayerGatesToMarker)
	{
		int nTestQuestID = 100;
		string strTableName = MAKE_LUATABLE_QUEST(nTestQuestID);
		MAKE_TABLE(strTableName.c_str());

		/// 퀘스트 목적이 완료되면 플레이어를 101 번 필드로 이동시킴.
		const char* pszScript = 
			"function Quest_100:OnObjComplete(this, Player, ObjectiveID)\n"
			"	Player:GateToMarker(101, 1)\n"
			"end\n"
			;
		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		//////////////////////////////////////////////////////////////////////////
		/// 테스트 환경 설정 - Quest 추가, NPC 추가, DestField 추가
		///		Quest 목적 : 플레이어가 NPC 를 죽이면 퀘스트 목적 완료
		int nTestNPCID = 100;
		GQuestInfo* pQuestInfo = new GQuestInfo();
		pQuestInfo->nID = nTestQuestID;
		CSQObjectiveInfo* pNewQObejctiveInfo = new CSQObjectiveInfo();
		pNewQObejctiveInfo->nID = 1;
		pNewQObejctiveInfo->nQuestID = pQuestInfo->nID;
		pNewQObejctiveInfo->nType = QOT_DESTROY;
		pNewQObejctiveInfo->vecParam1.push_back(nTestNPCID);
		pNewQObejctiveInfo->nParam2 = 1;
		pQuestInfo->Insert(pNewQObejctiveInfo);
		gmgr.pQuestInfoMgr->MakeQOInfoCache_Destroy(nTestNPCID, pNewQObejctiveInfo);
		gmgr.pQuestInfoMgr->Insert(pQuestInfo);

		int nDestFieldID = 101;
		GFieldInfo* pDestFieldInfo = GUTHelper_Field::NewFieldInfo(nDestFieldID);
		GUTHelper_Field::InsertNewMarkerInfo(pDestFieldInfo, 1);
		GUTHelper_Field::NewSharedField(pDestFieldInfo);

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC*	pNPC = m_Helper.NewEntityNPC(m_pField, vec3(0,0,0), nTestNPCID);
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer, nTestQuestID);
		//////////////////////////////////////////////////////////////////////////

		GTalentInfo* pPCTalentInfo = m_Helper.NewMeleeTalentInfo(1);
		m_Helper.SetTalentDamage(pPCTalentInfo, 10000);
		m_Helper.AddSimpleHitInfo(pPCTalentInfo);

		CHECK_EQUAL(false, pNPC->IsDead());
		pPlayer->doUseTalent(1);							///< 플레이어가 NPC 를 죽임.
		m_pField->Update(.0f);
		CHECK_EQUAL(true, pNPC->IsDead());
		CHECK_EQUAL(INVALID_ID, pPlayer->GetFieldID());	///< 플레이어 퀘스트 목적 완료로 필드 이동이 시작됨. (스크립트의 Player:GateToMarker(101, 1) 함수 호출됨.)

		pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
		CHECK_EQUAL(nDestFieldID, pPlayer->GetFieldID());	///< 필드 이동 완료.
	}

}


