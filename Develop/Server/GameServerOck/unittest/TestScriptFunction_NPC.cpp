#include "stdafx.h"
#include "GFieldInfoMgr.h"
#include "GUTHelper.h"
#include "GEntityNPC.h"
#include "GNPCMgr.h"
#include "GTestWrapper.h"
#include "GScriptGroup_NPC.h"
#include "MockField.h"
#include "GFieldMgr.h"
#include "GModuleAI.h"
#include "GJob_Talent.h"
#include "GJobMgr.h"
#include "GJob_Patrol.h"
#include "GJob_Idle.h"
#include "GNPCScriptBuilder.h"
#include "GTestForward.h"
#include "GJob_MoveToPos.h"
#include "GJob_MoveToEntity.h"
#include "GJob_Roam.h"
#include "GServer.h"
#include "GGluePlayer.h"
#include "GJobMgr.h"
#include "GBehaviorStateMachine.h"
#include "GBehaviorState.h"
#include "GNPCInteraction.h"
#include "FBaseMockLink.h"
#include "FBasePlayer.h"
#include "GEntityPlayer.h"
#include "GNPCSwallow.h"
#include "GModuleCombat.h"
#include "GHateTable.h"
#include "GNPCSensorChecker.h"
#include "GNPCWaitFor.h"
#include "GNPCTarget.h"
#include "GNPCMinion.h"
#include "GCombatFlee.h"
#include "GBehaviorCombat.h"
#include "GGlueNPC.h"
#include "GNPCBParts.h"
#include "GNPCMove.h"
#include "GNPCLoot.h"
#include "GNPCRewardOwner.h"
#include "GDropList.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(TestScriptFunction_NPC)
{
	struct Fixture : public FBaseMockLink, public FBasePlayer
	{
	public :
		Fixture()
		{
			gsys.pScriptManager->Init();

			m_NpcScriptGroup.RegisterGlues();
			
			AddFieldInfo();
			AddSpawnInfo();
			AddMockField();		
			AddNPCInfo();
			AddNPC();

			MWLua::class_<MockField>(WLUA->GetState(), "MockField")		
				.inh<GField>()
				.def(MWLua::constructor<>())
				;
		}

		~Fixture()
		{
			m_pField->Update(0.0f);
			m_pField->Destroy();
			gmgr.pFieldMgr->DeleteField(m_pField->GetUID());
			gmgr.pFieldMgr->Clear();
			
			//gmgr.pNPCMgr->RemoveNPC(m_pNPC);
			

			gsys.pScriptManager->Fini();
			SAFE_DELETE(m_pSpawnInfo);

			gmgr.pTalentInfoMgr->Clear();
		}

		

		void AddFieldInfo()
		{
			m_pFieldInfo = new GFieldInfo;
			m_pFieldInfo->m_nFieldID = 100;
			m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
			{
				m_vecSensorPos = vec3(100, 101, 0);

				m_SensorInfo.nID = 777;
				m_SensorInfo.bStartingEnable = true;
				m_SensorInfo.vPoint = m_vecSensorPos;
				m_pFieldInfo->InsertSensor(m_SensorInfo);
			}

			m_nMarkerID		= 5;
			m_vec3MarkerPos = vec3(100, 100, 0);
			
			m_MarkerInfo.nID = m_nMarkerID;
			m_MarkerInfo.vPoint = m_vec3MarkerPos;
			m_pFieldInfo->InsertMarkerInfo(m_MarkerInfo);

			gmgr.pFieldInfoMgr->Insert(m_pFieldInfo);
		}

		void AddSpawnInfo()
		{
			m_nInvalidSpawnID = -1;
			m_pSpawnInfo = new SPAWN_INFO();
			m_pSpawnInfo->nID = 1122;
			m_pSpawnInfo->bCheckSpawnPointValidation = false;
			m_pSpawnInfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
			m_pFieldInfo->InsertSpawnInfo(*m_pSpawnInfo);
		}

		void AddMockField()
		{
			m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
			gmgr.pFieldMgr->AddField(m_pField);		
		}

		void AddNPCInfo()
		{
			m_nInvalidNPCID = -1;
			m_pNPCInfo = new GNPCInfo;
			m_pNPCInfo->nID = 1;
			m_pNPCInfo->nMaxHP = 50000;
			GNPCInfo* pMode1Info = new GNPCInfo();
			pMode1Info->nID = 1;
			pMode1Info->nMode = NPC_MODE_1;
			pMode1Info->ModeCopyFromParent(m_pNPCInfo);
			m_pNPCInfo->vecModes.push_back(pMode1Info);	// mode1도 있음

			wchar_t szBuff[128];
			swprintf_s(szBuff, L"NPC_%d", m_pNPCInfo->nID);
			m_pNPCInfo->strNameInLua = szBuff;
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);

			m_pNPCInfo->m_bUseScriptTimer = true;	// Timer 스크립트도 테스트함

			m_pNPCInfo->Cooking();

			GNPCScriptBuilder nsb(m_pNPCInfo);
			nsb.Build();
			nsb.BuildLuaSubScript();
			GetScriptNPC()->OnInit(m_pNPCInfo);
		}

		void AddNPC()
		{
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField, 1);
			m_uidNPC = m_pNPC->GetUID();

			m_pNPC->SetNPCInfo(m_pNPCInfo);
			m_pNPC->SetPos(vec3(0, 0, 0));
			m_pNPC->InitFromNPCInfo(m_pNPCInfo, m_pSpawnInfo);

			gmgr.pNPCMgr->AddNPC(m_pNPC);	
		}

		GTalentInfo* MakeSimpleDmgAtkTalent(const int nDamage, int nTalentID=INVALID_TALENT_ID)
		{
			if (nTalentID == INVALID_TALENT_ID)		nTalentID = INT_MAX-2;

			GTalentInfo* pAtkTalent = gmgr.pTalentInfoMgr->Find(nTalentID);
			if (pAtkTalent)			
			{
				m_Helper.SetTalentDamage(pAtkTalent, nDamage);
				return pAtkTalent;
			}

			pAtkTalent = m_Helper.NewMeleeTalentInfo(nTalentID);
			m_Helper.SetTalentDamage(pAtkTalent, nDamage);
			m_Helper.AddSimpleHitInfo(pAtkTalent);

			return pAtkTalent;
		}

	protected :
		MUID								m_uidNPC;
		GUTHelper							m_Helper;
		GEntityNPC*							m_pNPC;

		MockField*							m_pField;

		GNPCInfo*							m_pNPCInfo;		
		GFieldInfo*							m_pFieldInfo;

		int									m_nMarkerID;
		vec3								m_vec3MarkerPos;
		vec3								m_vecSensorPos;
		
		int									m_nInvalidNPCID;
		SPAWN_INFO*							m_pSpawnInfo;		
		int									m_nInvalidSpawnID;
		SENSOR_INFO							m_SensorInfo;
		MARKER_INFO							m_MarkerInfo;

		GScriptGroup_NPC					m_NpcScriptGroup;

		GTestSysWrapper<GScriptManager>		m_ScriptMgr;
		GTestMgrWrapper<GNPCMgr>			m_NPCMgr;
		GTestMgrWrapper<GFieldMgr>			m_FieldMgr;
		GTestMgrWrapper<GFieldInfoMgr>		m_FieldInfoMgr;
		GTestMgrWrapper<GNPCInfoMgr>		m_NPCInfoMgr;
		DECLWRAPPER_SimpleCombatCalc;
	};


	TEST_FIXTURE(Fixture, NPC_EnableCombat )
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:EnableCombat();\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		m_pNPCInfo->nAA = NAAT_ALWAYS;
		m_pNPCInfo->nSightRange = 1000;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->ChangeAttackable(NAT_NONE);
		CHECK_EQUAL(true, pNPC->IsNoneAttackable());
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(true, pNPC->IsAlwaysAttackable());
	}

	TEST_FIXTURE(Fixture, NPC_EnableCombatWithFaction )
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:EnableCombatWithFaction();\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		m_pNPCInfo->nAA = NAAT_ALWAYS;
		m_pNPCInfo->nSightRange = 1000;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->ChangeAttackable(NAT_NONE);
		CHECK_EQUAL(true, pNPC->IsNoneAttackable());
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(true, pNPC->IsFactionAttackable());
	}
	

	TEST_FIXTURE(Fixture, NPC_DisableCombat )
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:DisableCombat();\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		m_pNPCInfo->nAA = NAAT_ALWAYS;
		m_pNPCInfo->nSightRange = 1000;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->ChangeAttackable(NAT_ALWAYS);
		CHECK_EQUAL(true, pNPC->IsAlwaysAttackable());
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(true, pNPC->IsNoneAttackable());
	}

	TEST_FIXTURE(Fixture, NPC_GetSpawnPos)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetSpawnPos();\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		// 스폰 정보가 없는 경우
		{
			GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
			vec3 ret;
			WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
			CHECK_EQUAL(ret.IsEqual(vCommonPos), true);
			pNPC->Destroy();
			SAFE_DELETE(pNPC);
		}

		// 스폰 정보가 있는 경우
		{
			SPAWN_INFO* pSpawninfo = new SPAWN_INFO();
			pSpawninfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
			GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vec3(1234.1f, 5678.2f, 9012.3f), vCommonDir, pSpawninfo);
			vec3 ret;
			WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
			CHECK_EQUAL(ret.IsEqual(vec3(1234.1f, 5678.2f, 9012.3f)), true);
			pNPC->Destroy();
			SAFE_DELETE(pNPC);
			SAFE_DELETE(pSpawninfo);
		}
	}

	TEST_FIXTURE(Fixture, NPC_Say)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Say(\"Test\");\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		MockLink* pLink = NewLink(pPlayer);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);	// 큐에 있는 잡을 시작

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_SAY, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pNPC->GetID(), pLink->GetParam<int>(0, 1));
		CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 2));
		CHECK_EQUAL(GConst::SPEAK_MAINTAIN_TIME,	pLink->GetParam<float>(0, 3));

		GJob* pCurJob = pNPC->GetModuleAI()->GetJobMgr()->GetCurrJob();
		CHECK(pCurJob);	// 대기시간동안 잡이 남아있음
	}

	TEST_FIXTURE(Fixture, NPC_Ballon)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Balloon(\"Test\");\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		MockLink* pLink = NewLink(pPlayer);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);	// 큐에 있는 잡을 시작

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_BALLOON, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pNPC->GetID(), pLink->GetParam<int>(0, 1));
		CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 2));
		CHECK_EQUAL(GConst::SPEAK_MAINTAIN_TIME,	pLink->GetParam<float>(0, 3));

		GJob* pCurJob = pNPC->GetModuleAI()->GetJobMgr()->GetCurrJob();
		CHECK(pCurJob);	// 대기시간동안 잡이 남아있음
	}

	TEST_FIXTURE(Fixture, NPC_Shout)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Shout(\"Test\");\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		MockLink* pLink = NewLink(pPlayer);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);	// 큐에 있는 잡을 시작

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_SHOUT, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pNPC->GetID(), pLink->GetParam<int>(0, 1));
		CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 2));
		CHECK_EQUAL(GConst::SPEAK_MAINTAIN_TIME,	pLink->GetParam<float>(0, 3));
	}

	TEST_FIXTURE(Fixture, NPC_NonDelaySay)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:NonDelaySay(\"Test\");\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		MockLink* pLink = NewLink(pPlayer);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);	// 큐에 있는 잡을 시작

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_SAY, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pNPC->GetID(), pLink->GetParam<int>(0, 1));
		CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 2));
		CHECK_EQUAL(0.0f,	pLink->GetParam<float>(0, 3));

		GJob* pCurJob = pNPC->GetModuleAI()->GetJobMgr()->GetCurrJob();
		CHECK(pCurJob == NULL);	// 즉시 사용되어 없어짐
	}

	TEST_FIXTURE(Fixture, NPC_NonDelayBalloon)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:NonDelayBalloon(\"Test\");\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		MockLink* pLink = NewLink(pPlayer);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);	// 큐에 있는 잡을 시작

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_BALLOON, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pNPC->GetID(), pLink->GetParam<int>(0, 1));
		CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 2));
		CHECK_EQUAL(0.0f,	pLink->GetParam<float>(0, 3));

		GJob* pCurJob = pNPC->GetModuleAI()->GetJobMgr()->GetCurrJob();
		CHECK(pCurJob == NULL);	// 즉시 사용되어 없어짐
	}

	TEST_FIXTURE(Fixture, NPC_NonDelayShout)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:NonDelayShout(\"Test\");\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);

		MockLink* pLink = NewLink(pPlayer);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);	// 큐에 있는 잡을 시작

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_SHOUT, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(pNPC->GetID(), pLink->GetParam<int>(0, 1));
		CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 2));
		CHECK_EQUAL(0.0f,	pLink->GetParam<float>(0, 3));

		GJob* pCurJob = pNPC->GetModuleAI()->GetJobMgr()->GetCurrJob();
		CHECK(pCurJob == NULL);	// 즉시 사용되어 없어짐
	}


	TEST_FIXTURE(Fixture, NPC_GetDamagedInfo)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetDamagedInfo();\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		//공격 탤런트
		const int ATTACK_TALENT_DAMAGE = 100;
		GTalentInfo* pAtkTalent = MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);
		pAtkTalent->m_setNPCID.insert(1);
		pAtkTalent->m_nDamageAttrib = DA_SLASH;

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GModuleAI* pmAI = pNPC->GetModuleAI();
		if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }

		pPlayer->doUseTalent(pAtkTalent);
		m_pField->Update(1.0f);

		GDamageInfo ret;
		CHECK_EQUAL(ret.nType,DA_NONE);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret.nType,DA_SLASH);
		CHECK(ret.nAmount>0);
	}

	TEST_FIXTURE(Fixture, NPC_UserData)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function NPC_1:Init(ID)\n"
			"	NPC_ReserveUserData(ID, 3);\n"
			"end\n"
			"function %s(NPC)\n"
			"	NPC:SetUserData(2, NPC:GetUserData(1)+1);\n"
			"end\n"
			"function NPC_1:OnUserdataChanged(thisNPC, index, data)\n"
			"	TEST_VAR = index*100+data;\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		GetScriptNPC()->OnInit(m_pNPCInfo);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		pNPC->SetUserdata(1, 6);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(pNPC->GetUserdata(1), 6);
		CHECK_EQUAL(pNPC->GetUserdata(2), 7);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 2*100+7);
	}

	TEST_FIXTURE(Fixture, NPC_IsNowCombat)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:IsNowCombat()\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		bool bIsCombat;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &bIsCombat);
		CHECK_EQUAL(bIsCombat, false);

		pNPC->doAttack(pPlayer->GetUID(), 10);
		gsys.pServer->Update(0.1f);

		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &bIsCombat);
		CHECK_EQUAL(bIsCombat, true);
	}

	TEST_FIXTURE(Fixture, NPC_IsEnableCombat)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:IsEnableCombat()\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		bool ret;
		pNPC->ChangeAttackable(NAT_NONE);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret, false);

		pNPC->ChangeAttackable(NAT_ALWAYS);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret, true);
	}

	TEST_FIXTURE(Fixture, NPC_Aggro)
	{
		DECLWRAPPER_SimpleCombatCalc;

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:Aggro(\"flip\", 0)\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GEntityPlayer* pAttacker1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityPlayer* pAttacker2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(1);		
		test.talent.UseTalent(pAttacker1, pAtkTalent, pNPC->GetUID());
		test.talent.UseTalent(pAttacker1, pAtkTalent, pNPC->GetUID());	// 2회 공격해서 어그로 획득
		test.talent.UseTalent(pAttacker2, pAtkTalent, pNPC->GetUID());

		CHECK_EQUAL(pNPC->GetTargetActor(), pAttacker1);		
		GGlueActor* pRet=NULL;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &pRet);	// 어그로 반대로 뒤집기
		CHECK_EQUAL(pNPC->GetTargetActor(), pAttacker2);
		CHECK_EQUAL(dynamic_cast<GGluePlayer*>(pRet), pAttacker2->GetDelegator());
	}

	TEST_FIXTURE(Fixture, NPC_BreakedPart)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, Attacker)\n"
			"	return NPC:BreakPart(1, Attacker);\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		CHECK_EQUAL(false, m_pNPC->GetNPCBParts()->IsBroken(1));
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), m_pNPC->GetDelegator(), WNULL);
		m_pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(1));
	}

	TEST_FIXTURE(Fixture, NPC_CheckBPart)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:CheckBPart(1);\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		bool ret;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret, false);
		m_pNPC->GetNPCBParts()->SetBreak(1);
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret, true);
	}


	TEST_FIXTURE(Fixture, NPC_GetCurrentTalent)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetCurrentTalent()\n"
			"end\n"
			;
		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityPlayer* pAttacker1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(1, 777);		
		pAtkTalent->m_fCastingTime = 10.0f;
		pAtkTalent->Cooking();

		test.talent.UseTalent(m_pNPC, pAtkTalent, pAttacker1->GetUID());
		int nTalentID=0;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nTalentID);
		CHECK_EQUAL(777, nTalentID);
	}

	TEST_FIXTURE(Fixture, NPC_GetHP)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetHP()\n"
			"end\n"
			;
		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		m_pNPC->SetHP(787);
		int nRet=0;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);
		CHECK_EQUAL(787, nRet);
	}

	TEST_FIXTURE(Fixture, NPC_GetMaxHP)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetMaxHP()\n"
			"end\n"
			;
		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		m_pNPC->GetChrInfo()->nMaxHP = 7787;
		int nRet=0;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);
		CHECK_EQUAL(7787, nRet);
	}

	TEST_FIXTURE(Fixture, NPC_GetNPCMode)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetMode()\n"
			"end\n"
			;
		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GNPCInfo* m_pNPCInfo_Mode1 = new GNPCInfo;
		m_pNPCInfo->vecModes.push_back(m_pNPCInfo_Mode1);
		m_pNPCInfo->nMode = NPC_MODE_DEFAULT;
		m_pNPCInfo_Mode1->nMode = NPC_MODE_1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->SetNPCInfo(m_pNPCInfo);

		int nRet=0;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &nRet);
		CHECK_EQUAL(0, nRet);
		pNPC->ChangeMode(NPC_MODE_1);
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &nRet);
		CHECK_EQUAL(1, nRet);
	}

	TEST_FIXTURE(Fixture, NPC_SetTimer)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:SetTimer(111, 10, true);\n"
			"end\n"
			"TEST_VAR = 0\n"
			"function NPC_1:OnTimer(NPC, TimerID)\n"
			"	TEST_VAR = TEST_VAR + TimerID\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);
		m_pNPC->Update(10.0f);
		CHECK_EQUAL(111, WLUA->GetVar<int>("TEST_VAR"));
		m_pNPC->Update(10.0f);
		CHECK_EQUAL(222, WLUA->GetVar<int>("TEST_VAR"));
		m_pNPC->Update(10.0f);
		CHECK_EQUAL(333, WLUA->GetVar<int>("TEST_VAR"));
	}

	TEST_FIXTURE(Fixture, NPC_KillTimer)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:KillTimer(111);\n"
			"end\n"
			"TEST_VAR = 0\n"
			"function NPC_1:OnTimer(NPC, TimerID)\n"
			"	TEST_VAR = TEST_VAR + TimerID\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		m_pNPC->GetNPCInfo()->Cooking();


		m_pNPC->SetTimer(111, 10.0f, true);
		m_pNPC->Update(10.0f);
		CHECK_EQUAL(111, WLUA->GetVar<int>("TEST_VAR"));
		m_pNPC->Update(10.0f);
		CHECK_EQUAL(222, WLUA->GetVar<int>("TEST_VAR"));
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);
		m_pNPC->Update(10.0f);
		CHECK_EQUAL(222, WLUA->GetVar<int>("TEST_VAR"));
	}

	TEST_FIXTURE(Fixture, NPC_Wait)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, WaitTime)\n"
			"	NPC:Wait(WaitTime)\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), 3.0f, WNULL);
		GModuleAI* pmAI = m_pNPC->GetModuleAI();
		pmAI->Update(0.0f);	// 큐에 있는 잡을 시작
		TVALID(pmAI);
		GJobMgr* pJobMgr = pmAI->GetJobMgr();
		GJob* pJob = pJobMgr->GetCurrJob();
		TVALID(pJob);
		CHECK_EQUAL(pJob->GetID(), GJOB_IDLE);

		GJob_Idle* pIdleJob = static_cast<GJob_Idle*>(pJob);
		CHECK_EQUAL(pIdleJob->GetIdleTime(), 3.0f);
	}

	TEST_FIXTURE(Fixture, NPC_GetSightRange)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, Enemy)\n"
			"	return NPC:GetSightRange(Enemy)\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		// 시야값 설정
		m_pNPCInfo->nSightRange = 777;

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GEntityNPC* pEnemy = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pEnemy->SetLevel(pNPC->GetLevel());

		int nRet=0;
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), pEnemy->GetDelegator(), &nRet);
		CHECK_EQUAL(777, nRet);
	}

	TEST_FIXTURE(Fixture, TSFN_Glue_Patrol)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Patrol({5}, 0)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();		
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());
		TVALID_EQ(1, m_pNPC->GetMissionJobs().size());
		GJob_Patrol* pJob = reinterpret_cast<GJob_Patrol*>(m_pNPC->GetMissionJobs()[0]);
		CHECK_EQUAL(GJOB_PATROL, pJob->GetID());

		MARKER_INFO mi;
		pJob->GetCurrentPatrolMarker(mi);
		CHECK_EQUAL(5, mi.nID);
	}

	TEST_FIXTURE(Fixture, TSFN_Glue_PatrolInstant)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:PatrolInstant({vec3(111,111,111), vec3(222,222,222)}, 0)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);
		CHECK(m_pNPC->HasMissionJob());
		TVALID_EQ(1, m_pNPC->GetMissionJobs().size());
		GJob_Patrol* pJob = reinterpret_cast<GJob_Patrol*>(m_pNPC->GetMissionJobs()[0]);
		CHECK_EQUAL(GJOB_PATROL, pJob->GetID());

		MARKER_INFO mi;
		pJob->GetCurrentPatrolMarker(mi);
		CHECK_EQUAL(true, mi.vPoint.IsEqual(vec3(111,111,111)));
	}

	TEST_FIXTURE(Fixture, TSFN_Glue_Roam)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Roam(1.0, 5)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();		
		CHECK_EQUAL(0, pJobMgr->GetCount());		

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());
		TVALID_EQ(1, m_pNPC->GetMissionJobs().size());

		GJob_Roam* pJob = reinterpret_cast<GJob_Roam*>(m_pNPC->GetMissionJobs()[0]);
		CHECK(NULL != pJob);

		CHECK_EQUAL(GJOB_ROAM, pJob->GetID());
		CHECK_EQUAL(1.0, pJob->GetRadius());
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_Follow)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Follow(NPC)\n" // 날따라 가자 -_-;;
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();		
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());
		TVALID_EQ(1, m_pNPC->GetMissionJobs().size());

		//GJob_MoveToEntity* pJob = reinterpret_cast<GJob_MoveToEntity*>(pJobMgr->GetCurrJob());
		//CHECK(NULL != pJob);
		//CHECK_EQUAL(GJOB_MOVE_TO_ENTITY, pJob->GetID());
		//CHECK(pJob->GetTargetEntityPos(m_pNPC->GetPos()));
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_ClearJob)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:ClearJob()\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasMissionJob());
		
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();				
		CHECK_EQUAL(0, pJobMgr->GetCount());		

		GTalentInfo TInfo;
		TInfo.m_nID = 0;
		pJobMgr->Push(new GJob_UseTalent(m_pNPC->GetModuleAI(), &TInfo));

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		
		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);
		m_pNPC->Update(0.0f);

		CHECK(!m_pNPC->HasMissionJob());
		CHECK_EQUAL(0, pJobMgr->GetCount());	
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_ReturnToHome)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:ReturnToHome()\n"
			"end\n"
			;

		const vec3 vec3HomePos(100, 0, 100);
		m_pNPC->SetHomePoint(vec3HomePos, vec3::ZERO);

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());
		TVALID_EQ(1, m_pNPC->GetMissionJobs().size());

		GJob_MoveToPos* pJob = reinterpret_cast<GJob_MoveToPos*>(m_pNPC->GetMissionJobs()[0]);
		CHECK(NULL != pJob);
		CHECK_EQUAL(GJOB_MOVE_TO_POS, pJob->GetID());
		CHECK_EQUAL(vec3HomePos, pJob->GetTarPos());
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_Despawn)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Despawn(false)\n"
			"end\n"
			;

		CHECK(!m_pNPC->IsDeleteMe());
		CHECK(!m_pNPC->HasMissionJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		TVALID_EQ(0, m_pNPC->GetMissionJobs().size());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());		
		TVALID_EQ(1, m_pNPC->GetMissionJobs().size());
		m_pNPC->GetModuleAI()->Update(0.0f);

		m_pNPC->GetModuleAI()->Update(0.0f);
		TVALID_EQ(0, m_pNPC->GetMissionJobs().size());
		CHECK(m_pNPC->IsDeleteMe());
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_Die)
	{
		GLootInfo* pLootInfo = test.loot.NewLootInfo();
		LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo, 100, 10, 10);
		m_pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;

		GEntityPlayer* pPlayer = test.player.NewPlayer(m_pField, vCommonPos);

		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, Actor)\n"
			"	NPC:Die(Actor)\n"
			"end\n"
			;

		CHECK(!m_pNPC->IsDead());
		CHECK(!m_pNPC->HasMissionJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		TVALID_EQ(0, m_pNPC->GetMissionJobs().size());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pPlayer->GetDelegator(),  &nRet);
		CHECK(m_pNPC->IsDead());
		CHECK_EQUAL(true, m_pNPC->GetNPCLoot().GetDropList().IsViewableCID(pPlayer->GetCID()));
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_Attack)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, Target)\n"
			"	NPC:Attack(Target)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasMissionJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		GEntityNPC* pTarget = GUTHelper_NPC::SpawnNPC(m_pField, 1);
		CHECK(NULL != pTarget);
		
		pTarget->SetHP(100);

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pTarget->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_ATTACK, m_pNPC->GetMissionJobs()[0]->GetID());
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_Assist)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, AlliedNPC)\n"
			"	NPC:Assist(AlliedNPC)\n"
			"end\n"
			;

		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		GEntityNPC* pAlliedNPC = GUTHelper_NPC::SpawnNPC(m_pField, 1);
		CHECK(NULL != pAlliedNPC);

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pAlliedNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_ASSIST, m_pNPC->GetMissionJobs()[0]->GetID());
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_FleeFromEnemy)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:FleeFromEnemy(777)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(!m_pNPC->HasJob());		

		GModuleAI* pmAI = m_pNPC->GetModuleAI();
		CHECK(NULL != pmAI);

		CHECK_EQUAL(0, pJobMgr->GetCount());			
		CHECK(NULL == pJobMgr->GetCurrJob());

		GBehaviorStateMachine* pFSM = pmAI->GetBehaviorFSM();
		CHECK_EQUAL(AI_BEHAVIOR_COMBAT, pFSM->GetCurrentStateID());
		CHECK_EQUAL(AI_COMBAT_FLEE, pFSM->GetCurrentCombatStateID());

		GBehaviorCombat* pCombatBehavior = dynamic_cast<GBehaviorCombat*>(pFSM->FindState(AI_BEHAVIOR_COMBAT));
		TVALID(pCombatBehavior);
		GCombatFlee* pCombatFlee = dynamic_cast<GCombatFlee*>(pCombatBehavior->FindCombatState(AI_COMBAT_FLEE));
		TVALID(pCombatFlee);
		CHECK_EQUAL(777, pCombatFlee->GetDurationTime());
	}

	TEST_FIXTURE(Fixture, TSFN_Glue_FleeToHome)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:FleeToHome(777)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(!m_pNPC->HasJob());		

		GModuleAI* pmAI = m_pNPC->GetModuleAI();
		CHECK(NULL != pmAI);

		CHECK_EQUAL(0, pJobMgr->GetCount());			
		CHECK(NULL == pJobMgr->GetCurrJob());

		GBehaviorStateMachine* pFSM = pmAI->GetBehaviorFSM();
		CHECK_EQUAL(AI_BEHAVIOR_COMBAT, pFSM->GetCurrentStateID());
		CHECK_EQUAL(AI_COMBAT_FLEE, pFSM->GetCurrentCombatStateID());

		GBehaviorCombat* pCombatBehavior = dynamic_cast<GBehaviorCombat*>(pFSM->FindState(AI_BEHAVIOR_COMBAT));
		TVALID(pCombatBehavior);
		GCombatFlee* pCombatFlee = dynamic_cast<GCombatFlee*>(pCombatBehavior->FindCombatState(AI_COMBAT_FLEE));
		TVALID(pCombatFlee);
		CHECK_EQUAL(777, pCombatFlee->GetDurationTime());
	}

	TEST_FIXTURE(Fixture, TSFN_Glue_Yell)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Yell(10)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_YELL, m_pNPC->GetMissionJobs()[0]->GetID());
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_Move)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Move(vec3(100, 100, 0))\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_MOVE_TO_POS, m_pNPC->GetMissionJobs()[0]->GetID());
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_MoveToMarker)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:MoveToMarker(5)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_MOVE_TO_POS, m_pNPC->GetMissionJobs()[0]->GetID());
	}

	TEST_FIXTURE(Fixture, TSFN_Glue_MoveToSensor)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:MoveToSensor(777)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_MOVE_TO_POS, m_pNPC->GetMissionJobs()[0]->GetID());
	}

	TEST_FIXTURE(Fixture, TSFN_Glue_UseTalentSelf)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:UseTalentSelf(1)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());


		GTalentInfo* pTInfo = new GTalentInfo;
		pTInfo->m_nID = 1;
		pTInfo->m_nSkillType = ST_MELEE;
		pTInfo->m_setNPCID.insert(m_pNPCInfo->nID);
		gmgr.pTalentInfoMgr->Insert(pTInfo);


		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_TALENT, m_pNPC->GetMissionJobs()[0]->GetID());
	}


	TEST_FIXTURE(Fixture, TSFN_Glue_UseTalent)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC, Actor)\n"
			"	NPC:UseTalent(1, Actor)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		GTalentInfo* pTInfo = new GTalentInfo;
		pTInfo->m_nID = 1;
		pTInfo->m_nSkillType = ST_MELEE;
		pTInfo->m_setNPCID.insert(m_pNPCInfo->nID);
		gmgr.pTalentInfoMgr->Insert(pTInfo);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);	

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nRet;
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pNPC->GetDelegator(), &nRet);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_MOVE_TO_ENTITY, m_pNPC->GetMissionJobs()[0]->GetID());
		CHECK_EQUAL(GJOB_TALENT, m_pNPC->GetMissionJobs()[1]->GetID());
	}


	TEST_FIXTURE(Fixture, TestGlue_NPC_MoveToActor)
	{
		const char* pszTestFuncName = "_Test_NPC_MoveToTargetCloser";
		const char* pszScript = 
			"function %s(NPC, TargetActor, TalentID)\n"
			"	NPC:MoveToActor(TargetActor, TalentID);\n"
			"end";

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		//공격 탤런트
		const int		ATTACK_TALENT_DAMAGE	= 100;
		GTalentInfo*	pAtkTalent				= MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);
		GEntityPlayer*	pPlayer					= m_Helper.GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		pPlayer->SetDir(vCommonDir);
		pPlayer->SetPos(vec3(10000,10000,0));
		
		WCALL3(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pPlayer->GetDelegator(), pAtkTalent->m_nID, WNULL);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_MOVE_TO_ENTITY, m_pNPC->GetMissionJobs()[0]->GetID());
	}


	TEST_FIXTURE(Fixture, TestGlue_NPC_MoveToActorWithDuration)
	{
		const char* pszTestFuncName = "_Test_NPC_MoveToTargetCloserWithDuration";
		const char* pszScript = 
			"function %s(NPC, TargetActor, TalentID)\n"
			"	NPC:MoveToActorWithDuration(TargetActor, TalentID, 10);\n"	// 제한시간 10초
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		//공격 탤런트
		const int		ATTACK_TALENT_DAMAGE	= 100;
		GTalentInfo*	pAtkTalent				= MakeSimpleDmgAtkTalent(ATTACK_TALENT_DAMAGE);
		GEntityPlayer*	pPlayer					= m_Helper.GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		pPlayer->SetDir(vCommonDir);
		pPlayer->SetPos(vec3(10000,10000,0));
		
		CHECK(WCALL3(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pPlayer->GetDelegator(), pAtkTalent->m_nID, WNULL));
		
		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_MOVE_TO_ENTITY, m_pNPC->GetMissionJobs()[0]->GetID());
	}


	TEST_FIXTURE(Fixture, SetCustomDecayTime)
	{
		const char* pszTestFuncName = "_Test_NPC_SetCustomDecay";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:SetDecayTime(100)\n"	// 제한시간 10초
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		CHECK(WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL));

		m_pNPC->doDie();
		m_pNPC->Update(99);
		
		CHECK(!m_pNPC->IsDeleteMe());

		m_pNPC->Update(1);

		CHECK(m_pNPC->IsDeleteMe());
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_FaceTo)
	{
		const char* pszTestFuncName = "_TestFaceTo";
		const char* pszScript = 
			"function %s(NPC, TargetActor)\n"
			"	return NPC:FaceTo(TargetActor);\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		vec3 vNPCPos(1000, 1000, 1000);
		vec3 vTargetPlayerPos(2000, 2000, 2000);
		m_pNPC->SetPos(vNPCPos);
		GEntityPlayer* pTargetPlayer = GUTHelper::NewEntityPlayer(m_pField, vTargetPlayerPos);
		MockLink* pTargetPlayerLink = NewLink(pTargetPlayer);
		MockLink* pDummyChracterLink = NewLink(GUTHelper::NewEntityPlayer(m_pField, vCommonPos));
		pTargetPlayerLink->ResetCommands();
		pDummyChracterLink->ResetCommands();

		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pTargetPlayer->GetDelegator(), WNULL);

		m_pNPC->UpdateAI(.0f);

		vec3 vLookDir = vTargetPlayerPos - vNPCPos;
		GMath::NormalizeZ0(vLookDir);
		vec3 vExpectDir = vLookDir;

		CHECK_EQUAL(vExpectDir,		m_pNPC->GetDir());
		CHECK_EQUAL(1,				pTargetPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_FACE_TO,	pTargetPlayerLink->GetCommand(0).GetID());

		CHECK_EQUAL(1,				pDummyChracterLink->GetCommandCount());
		CHECK_EQUAL(MC_NPC_FACE_TO,	pDummyChracterLink->GetCommand(0).GetID());
	}

	TEST_FIXTURE(Fixture, SetAlwaysRun)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC, IsRunnable)\n"
			"	NPC:SetAlwaysRun(IsRunnable)\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		CHECK_EQUAL(false, m_pNPC->GetNPCMove().IsRun());		
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), true, WNULL);
		CHECK_EQUAL(true, m_pNPC->GetNPCMove().IsRun());		
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), false, WNULL);
		CHECK_EQUAL(false, m_pNPC->GetNPCMove().IsRun());		
	}

	TEST_FIXTURE(Fixture, EnableInteraction)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:EnableInteraction()\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		m_pNPC->GetNPCInteraction().Disable();

		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);		
		CHECK_EQUAL(true, m_pNPC->GetNPCInteraction().IsEnable());
	}

	TEST_FIXTURE(Fixture, DisableInteraction)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:DisableInteraction()\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		m_pNPC->GetNPCInteraction().Enable();

		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);		
		CHECK_EQUAL(false, m_pNPC->GetNPCInteraction().IsEnable());
	}

	TEST_FIXTURE(Fixture, GetLastInteractPlayer)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetLastInteractPlayer()\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityPlayer* pPlayer =  m_Helper.NewEntityPlayer(m_pField);

		m_pNPC->GetNPCInteraction().Begin(pPlayer);

		GGluePlayer* pLastInteractPlayer = NULL;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &pLastInteractPlayer);
		CHECK(pLastInteractPlayer == pPlayer->GetDelegator());
	}

	class FSwallow: public Fixture
	{
	public:
		FSwallow()
		{
			// 먹는 NPC
			SwallowedNPCInfo.nID = 1;
			SwallowedNPCInfo.nMaxHP = 50000;
			SwallowedNPCInfo.nEatOwnerBuffID = 560;
			SwallowedNPCInfo.nEatTargetBuffID = 561;
			SwallowedNPCInfo.fEatPeriod = 2.0f;
			wchar_t szBuff[128];
			swprintf_s(szBuff, L"NPC_%d", SwallowedNPCInfo.nID);
			SwallowedNPCInfo.strNameInLua = szBuff;

			pNPC = m_pField->SpawnTestNPC(&SwallowedNPCInfo, vCommonPos, vCommonDir);
			pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

			// 먹기 탤런트
			pSwallowedTalentInfo = m_Helper.NewTalentInfo();
			pSwallowedTalentInfo->m_nSkillType = ST_EXTRA_ACTIVE;
			pSwallowedTalentInfo->m_nExtraActive = TEAT_NPC_ONLY_EAT;
			pSwallowedTalentInfo->m_nTiming = TC_HIT_TALENT;
			pSwallowedTalentInfo->m_EffectInfo.m_fRadius = 1.0f;
			pSwallowedTalentInfo->m_fDurationTime = 2.0f;		// aitime 2초
			pSwallowedTalentInfo->m_fExtraPhaseTime = 2.0f;		// 잡은 후 2초 걸림
			pSwallowedTalentInfo->m_nForceMF = MF_SWALLOWED;
			pSwallowedTalentInfo->m_nForceMFWeight = 2000;
			pSwallowedTalentInfo->Cooking();
			m_Helper.SetTalentDamage(pSwallowedTalentInfo, 100);		// 데미지 100로 설정
			m_Helper.AddSimpleHitInfo(pSwallowedTalentInfo, 0.0f);	// 바로 판정하는 탤런트임
		}

	protected:
		GNPCInfo		SwallowedNPCInfo;
		GEntityNPC*		pNPC;
		GEntityPlayer*	pPlayer;
		GTalentInfo*	pSwallowedTalentInfo;
	};

	TEST_FIXTURE(FSwallow, IsSwallowable)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:IsSwallowable()\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		bool bRet;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &bRet);
		CHECK_EQUAL(true, bRet);

		// 먹기
		TALENT_TARGET_INFO target_info;
		target_info.uidTarget = pPlayer->GetUID();
		pNPC->GetModuleCombat()->UseTalent(pSwallowedTalentInfo, target_info, false);
		m_pField->Update(0.0f);

		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &bRet);
		CHECK_EQUAL(false, bRet);
	}

	TEST_FIXTURE(FSwallow, Spewup)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Spewup()\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		// 먹기
		TALENT_TARGET_INFO target_info;
		target_info.uidTarget = pPlayer->GetUID();
		pNPC->GetModuleCombat()->UseTalent(pSwallowedTalentInfo, target_info, false);
		m_pField->Update(0.0f);
		CHECK_EQUAL(false, pNPC->GetNPCSwallow().IsSwallowable());
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(true, pNPC->GetNPCSwallow().IsSwallowable());
	}

	TEST_FIXTURE(FSwallow, SetTauntable)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, Enable)\n"
			"	NPC:SetTauntable(Enable)\n"
			"end\n"
			;

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityNPC* pNPC = GUTHelper_NPC::SpawnNPC(m_pField);
		GEntityPlayer* m_pPlayer = NewEntityPlayer(m_pField);
		GEntityPlayer* pOtherPlayer = NewEntityPlayer(m_pField);
		GTalentInfo* pActiveTalent = test.talent.NewTalentInfo();
		test.talent.SetExtraActiveTalentInfo(pActiveTalent, 1, TEAT_TAUNT);
		pActiveTalent->m_nExtraActiveParam1 = 10;	// 10초 동안 고정
		pActiveTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pActiveTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;		

		int nDamage = 1000;
		float fHateFactor = 0.5f;
		pNPC->GetHateTable().AddPoint_GainDamage(m_pPlayer, nDamage, fHateFactor);
		CHECK_EQUAL(m_pPlayer->GetUID(), pNPC->GetTarget());
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), false, WNULL);
		test.talent.UseTalent(pOtherPlayer, pActiveTalent, pNPC->GetUID());
		CHECK_EQUAL(m_pPlayer->GetUID(), pNPC->GetTarget());	// 바뀌지 않음
	}

	TEST_FIXTURE(Fixture, EnableSensorCheck)
	{
		CHECK(NULL != m_pNPC);

		const SENSOR_INFO* pSensorInfo = GUTHelper_Field::InsertNewSensorInfo(m_pField->GetInfo(), 1);
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:EnableSensorCheck(1)\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(true, m_pNPC->GetNPCSensorChecker()->IsEnable(pSensorInfo));
	}

	TEST_FIXTURE(Fixture, GetName)
	{
		CHECK(NULL != m_pNPC);

		m_pNPC->GetNPCInfo()->strName = L"TestName";
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	TEST_VAR = NPC:GetName()\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);

		TVALID(WLUA->IsExistGlobal("TEST_VAR"));
		CHECK_EQUAL(string("TestName"), WLUA->GetVar<const char*>("TEST_VAR"));
	}

	TEST_FIXTURE(Fixture, WaitFor)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, Ohter)\n"
			"	NPC:WaitFor(Ohter)\n"
			"end\n"
			;

		GEntityNPC* pOhterNPC = GUTHelper_NPC::SpawnNPC(m_pField);

		CHECK_EQUAL(false, m_pNPC->GetNPCWaitFor().IsWaiting());
		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pOhterNPC->GetDelegator(), WNULL);
		m_pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(true, m_pNPC->GetNPCWaitFor().IsWaiting());
	}

	TEST_FIXTURE(Fixture, NextAction)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, Ohter)\n"
			"	NPC:WaitFor(Ohter)\n"
			"	Ohter:NextAction()\n"
			"end\n"
			;

		GEntityNPC* pOhterNPC = GUTHelper_NPC::SpawnNPC(m_pField);

		CHECK_EQUAL(false, m_pNPC->GetNPCWaitFor().IsWaiting());
		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pOhterNPC->GetDelegator(), WNULL);
		m_pNPC->GetModuleAI()->Update(0.0f);
		pOhterNPC->GetModuleAI()->Update(0.0f);
		m_pNPC->GetModuleAI()->Update(1.0f); // 대기체크할 시간
		CHECK_EQUAL(false, m_pNPC->GetNPCWaitFor().IsWaiting());
	}

	TEST_FIXTURE(Fixture, SetTarget)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC, Ohter)\n"
			"	NPC:SetTarget(Ohter)\n"
			"end\n"
			;

		GEntityNPC* pOhterNPC = GUTHelper_NPC::SpawnNPC(m_pField);

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pOhterNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(pOhterNPC->GetUID(), m_pNPC->GetNPCTarget().GetTargetUID());
	}

	TEST_FIXTURE(Fixture, GetTarget)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetTarget()\n"
			"end\n"
			;

		GEntityNPC* pOhterNPC = GUTHelper_NPC::SpawnNPC(m_pField);

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		m_pNPC->GetNPCTarget().SetTargetUID(pOhterNPC->GetUID());
		GGlueActor* pRet=NULL;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &pRet);
		CHECK_EQUAL(pOhterNPC->GetUID(), pRet->GetUID());
	}

	TEST_FIXTURE(Fixture, Summon)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:Summon(111, vec3(0,0,0))\n"
			"end\n"
			;

		GUTHelper_NPC::NewNPCInfo(111);	// 111 NPC정보 만듬
		CHECK_EQUAL(0, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(0, m_pField->GetNPCQty(111));
		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);
		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_SUMMON, m_pNPC->GetMissionJobs()[0]->GetID());
	}

	TEST_FIXTURE(Fixture, SummonNow)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:SummonNow(111, vec3(0,0,0))\n"
			"end\n"
			;

		GUTHelper_NPC::NewNPCInfo(111);	// 111 NPC정보 만듬
		CHECK_EQUAL(0, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(0, m_pField->GetNPCQty(111));
		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(1, m_pNPC->GetNPCMinion().GetMinionCount());
		CHECK_EQUAL(1, m_pField->GetNPCQty(111));
	}

	TEST_FIXTURE(Fixture, GetSummonCount)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetSummonCount()\n"
			"end\n"
			;

		GUTHelper_NPC::NewNPCInfo(111);	// 111 NPC정보 만듬
		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		int ret = -1;
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret, 0);	// 소환수 갯수: 0
		m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret, 1);	// 소환수 갯수: 1
	}

	TEST_FIXTURE(Fixture, GetSummoner)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetSummoner()\n"
			"end\n"
			;

		GUTHelper_NPC::NewNPCInfo(111);	// 111 NPC정보 만듬
		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		GEntityNPC* pMinion = m_pNPC->GetNPCMinion().Summon(111, vec3(0,0,0));
		GGlueNPC* ret = NULL;
		WCALL1(WLUA, pszTestFuncName, pMinion->GetDelegator(), &ret);
		CHECK_EQUAL(m_pNPC, ret->GetOwnerNPC());
	}

	TEST_FIXTURE(Fixture, StayawayFrom)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC, Target)\n"
			"	NPC:StayawayFrom(Target, 100)\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityPlayer* pEnemy = test.player.NewPlayer(m_pField);
		pEnemy->SetPos(vec3(1000,0,0));
		WCALL2(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), pEnemy->GetDelegator(), WNULL);

		CHECK(m_pNPC->HasMissionJob());		
		CHECK_EQUAL(GJOB_MOVE_TO_ENTITY, m_pNPC->GetMissionJobs()[0]->GetID());
	}

	TEST_FIXTURE(Fixture, ResetNPC)
	{
		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:ResetNPC()\n"
			"end\n"
			;

		// 기존 상태
		int nBuffID = 123;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		test.buff.GainBuffSelf(m_pNPC, pBuff);
		CHECK_EQUAL(true, m_pNPC->HasBuff(nBuffID));
		m_pNPC->SetHP(1);
		m_pNPC->SetEN(1);
		m_pNPC->SetSTA(1);
		m_pNPC->GetChrInfo()->nMaxHP = 111;
		m_pNPC->GetChrInfo()->nMaxEN = 222;
		m_pNPC->GetChrInfo()->nMaxSTA = 333;
		CHECK_EQUAL(1, m_pNPC->GetHP());
		CHECK_EQUAL(1, m_pNPC->GetEN());
		CHECK_EQUAL(1, m_pNPC->GetSTA());
		m_pNPC->GetNPCBParts()->SetBreak(1);
		m_pNPC->GetNPCBParts()->SetBreak(2);
		m_pNPC->GetNPCBParts()->SetBreak(3);
		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(1));
		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(2));
		CHECK_EQUAL(true, m_pNPC->GetNPCBParts()->IsBroken(3));
		//GEntityPlayer* pEnemy = test.player.NewPlayer(m_pField);
		//CHECK_EQUAL(false, m_pNPC->IsNowCombat());
		//CHECK_EQUAL(false, pEnemy->IsNowCombat());
		//m_pNPC->doAttack(pEnemy->GetUID());
		//CHECK_EQUAL(false, m_pNPC->GetHateTable().IsEmpty());
		//CHECK_EQUAL(true, m_pNPC->IsNowCombat());
		//CHECK_EQUAL(true, pEnemy->IsNowCombat());

		// 초기화
		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);

		// 사후 상태
		CHECK_EQUAL(false, m_pNPC->HasBuff(nBuffID));
		CHECK_EQUAL(m_pNPC->GetMaxHP(), m_pNPC->GetHP());
		CHECK_EQUAL(m_pNPC->GetMaxEN(), m_pNPC->GetEN());
		CHECK_EQUAL(m_pNPC->GetMaxSTA(), m_pNPC->GetSTA());
		CHECK_EQUAL(false, m_pNPC->GetNPCBParts()->IsBroken(1));
		CHECK_EQUAL(false, m_pNPC->GetNPCBParts()->IsBroken(2));
		CHECK_EQUAL(false, m_pNPC->GetNPCBParts()->IsBroken(3));
		//CHECK_EQUAL(true, m_pNPC->GetHateTable().IsEmpty());
		//CHECK_EQUAL(false, m_pNPC->IsNowCombat());
		//CHECK_EQUAL(false, pEnemy->IsNowCombat());
	}

	TEST_FIXTURE(Fixture, Attr_MaxHP)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s()\n"
			"	return NPC_1.MaxHP\n"
			"end\n"
			;

		CHECK(!m_pNPC->HasJob());
		GJobMgr* pJobMgr = m_pNPC->GetJobMgr();
		CHECK_EQUAL(0, pJobMgr->GetCount());

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nMaxHP = 0;
		WCALL(WLUA, pszTestFuncName, &nMaxHP);

		CHECK_EQUAL(m_pNPCInfo->nMaxHP, nMaxHP);
	}	

	TEST_FIXTURE(Fixture, ChangeAA)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:ChangeAA(AA_FACTION)\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		m_pNPC->GetNPCStatus().nAA = NAAT_ALWAYS;
		CHECK_EQUAL(NAAT_ALWAYS, m_pNPC->GetAAType());
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(NAAT_FACTION, m_pNPC->GetAAType());
	}	

	TEST_FIXTURE(Fixture, SaveHomePoint)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:SaveHomePoint()\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		vec3 vOldHomePoint(123,456,789);
		vec3 vNewHomePoint(987,456,123);
		vec3 vHomeDir(0,0,0);
		m_pNPC->SetHomePoint(vOldHomePoint, vHomeDir);
		m_pNPC->SetPos(vNewHomePoint);
		CHECK_EQUAL(vOldHomePoint, m_pNPC->GetHomePoint().vPoint);
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), WNULL);
		CHECK_EQUAL(vNewHomePoint, m_pNPC->GetHomePoint().vPoint);
	}	

	TEST_FIXTURE(Fixture, GetLevel)
	{
		CHECK(NULL != m_pNPC);

		const char* pszTestFuncName = "_Test";
		// target은 적당히 멀게 잡아줘야 길을 찾는다.
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetLevel()\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		int nLevel = 0;
		int nExpectedLevel = 11;
		m_pNPC->SetLevel(nExpectedLevel);
		WCALL1(WLUA, pszTestFuncName, m_pNPC->GetDelegator(), &nLevel);
		CHECK_EQUAL(nLevel, nExpectedLevel);
		
	}	
}
