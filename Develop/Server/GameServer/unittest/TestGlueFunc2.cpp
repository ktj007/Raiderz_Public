#include "stdafx.h"
#include "GUnitTestUtil.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GModuleAI.h"
#include "GJob_MoveToPos.h"
#include "GJob_MoveToEntity.h"
#include "MockEntityNPC.h"
#include "GFieldSystem.h"
#include "GDynamicFieldGroup.h"
#include "GDynamicField.h"
#include "GFatigueSystem.h"
#include "CCommandTable.h"
#include "GInteractionSystem.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 VCommonDir = vec3(0,1,0);

class TestFieldMgr : public GFieldMgr
{
public:
	virtual MUID	NewUID()
	{
		return GUnitTestUtil::NewUID();
	}
};

SUITE(GlueFunc2)
{
	struct Fixture
	{
		Fixture()
		{ 
			gsys.pScriptManager->Init();

			m_pFieldInfo = new GFieldInfo;
			m_pFieldInfo->m_nFieldID = 100;
			m_pFieldInfo->m_fMaxX = 50000.0f;//MockField::s_FieldInfo.m_fMaxX;
			m_pFieldInfo->m_fMaxY = 50000.0f;//MockField::s_FieldInfo.m_fMaxY;
			{
				SENSOR_INFO sensor;
				sensor.nID = 777;
				sensor.bStartingEnable = true;
				m_pFieldInfo->InsertSensor(sensor);
			}
			gmgr.pFieldInfoMgr->Insert(m_pFieldInfo);

			m_pField = m_Helper.NewMockField(m_pFieldInfo);
			gmgr.pFieldMgr->AddField(m_pField);

			m_pNPCInfo = new GNPCInfo;
			m_pNPCInfo->nID = 1;
			m_pNPCInfo->nOccupation = 777;
			m_pNPCInfo->nMaxHP = 50000;
			char szBuff[128];
			sprintf_s(szBuff, "NPC_%07d", m_pNPCInfo->nID);
			m_pNPCInfo->strNameInLua = szBuff;
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);
		}

		~Fixture() 
		{
			m_pField->Update(1.0f);
			m_pField->Destroy();
			gmgr.pFieldMgr->DeleteField(m_pField->GetUID());
			gmgr.pFieldMgr->Clear();

			gsys.pScriptManager->Fini();
		}

		GTalentInfo* MakeSimpleDmgAtkTalent(const int nDamage, int nTalentID=INVALID_TALENT_ID)
		{
			if (nTalentID == INVALID_TALENT_ID)		nTalentID = INT_MAX-2;

			GTalentInfo* pAktTalent = gmgr.pTalentInfoMgr->Get(nTalentID);
			if (pAktTalent)			
			{
				m_Helper.SetTalentDamage(pAktTalent, nDamage);
				return pAktTalent;
			}

			pAktTalent = m_Helper.NewMeleeTalentInfo(nTalentID);
			m_Helper.SetTalentDamage(pAktTalent, nDamage);
			m_Helper.AddSimpleTalentInfoHitInfo(pAktTalent);
			m_Helper.InsertTalentInfoToMgr(pAktTalent);

			return pAktTalent;
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GFieldInfo*		m_pFieldInfo;
		GNPCInfo*		m_pNPCInfo;

		GTestSysWrapper<GScriptManager>				m_ScriptMgr;
		GTestMgrWrapper2<GFieldMgr, TestFieldMgr>	m_FieldMgr;
		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
	};

	bool RunLuaString( const char* szFormat, ... )
	{
		char szBuff[1024];

		va_list args;
		va_start(args,szFormat);
		vsprintf(szBuff,szFormat,args);
		va_end(args);

		return WRUNSTRING(WLUA, szBuff);
	}

	TEST_FIXTURE(Fixture, TestGlue_NPC_ReturnToHomePoint)
	{
		const char* pszTestFuncName = "_Test_NPC_ReturnToHomePoint";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:ReturnToHomePoint();\n"
			"end";

		if (!RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, VCommonDir);
		pNPC->SetPos(vec3(7737.0f, 3321.0f, 2134.0f));
		GModuleAI* pmAI = pNPC->GetModuleAI();
		if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }

		/*WCALL1(WLUA, pszTestFuncName, pNPC, WNULL);*/
		pNPC->ReturnToHomePoint();
		m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.

		GJob* pJob = pmAI->GetCurrentJob();
		if (!pJob) 	{ CHECK(FALSE && "Job is null.");	 return; }
		CHECK_EQUAL(pJob->GetID(), GJOB_MOVE_TO_POS);
		GJob_MoveToPos* pMoveJob = static_cast<GJob_MoveToPos*>(pJob);
		CHECK_EQUAL(pMoveJob->GetTarPos().IsEqual(vCommonPos), true);

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	//TEST_FIXTURE(Fixture, TestGlue_NPC_ClearTasks)
	//{
	//	const char* pszTestFuncName = "_Test_NPC_ClearTasks";
	//	const char* pszScript = 
	//		"function %s(NPC)\n"
	//		"	NPC:ClearTasks();\n"
	//		"end";

	//	if (!RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

	//	GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, VCommonDir);
	//	GModuleAI* pmAI = pNPC->GetModuleAI();
	//	if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }

	//	pNPC->Glue_Move(vec3(0,0,0));
	//	m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.
	//	CHECK(pmAI->GetCurrentJob() != NULL);
	//	WCALL1(WLUA, pszTestFuncName, pNPC, WNULL);
	//	CHECK_EQUAL(pmAI->GetCurrentJob(), (GJob*)NULL);

	//	pNPC->Destroy();
	//	SAFE_DELETE(pNPC);
	//}

	//TEST_FIXTURE(Fixture, TestGlue_NPC_Patrol)
	//{
	//	const char* pszTestFuncName = "_Test_NPC_Patrol";
	//	const char* pszScript = 
	//		"function %s(NPC)\n"
	//		"	local MarkerList = MARKER_LIST();"
	//		"	MarkerList:Add(10);"
	//		"	MarkerList:Add(11);"
	//		"	MarkerList:Add(12);"
	//		"	MarkerList:Add(13);"
	//		"	NPC:Patrol(MarkerList);\n"
	//		"end";

	//	if (!RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

	//	{
	//		MARKER_INFO Marker;
	//		Marker.nFieldID = 100;
	//		for (float i=0; i<4; i++)
	//		{
	//			Marker.nID = 10+(int)i;	Marker.vPoint = vec3(10+i, 10+i, 10+i);	m_pFieldInfo->InsertMarkerInfo(Marker);
	//		}
	//	}

	//	GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, VCommonDir);
	//	WCALL1(WLUA, pszTestFuncName, pNPC, WNULL);
	//	m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.
	//	GModuleAI* pmAI = pNPC->GetModuleAI();
	//	if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }
	//	GJob* pJob = pmAI->GetCurrentJob();
	//	if (!pJob) 	{ CHECK(FALSE && "Job is null.");	 return; }
	//	CHECK_EQUAL(pJob->GetID(), GJOB_PATROL);

	//	GJob_Patrol* pMoveJob = static_cast<GJob_Patrol*>(pJob);
	//	MARKER_INFO Marker;
	//	int nMarkerQty = 0;
	//	pMoveJob->GoBackMoveMarker();
	//	for (float i=0; i<10; i++, nMarkerQty++, pMoveJob->AdvanceMoveMarker())
	//	{
	//		if (!pMoveJob->GetCurrentPatrolMarker(Marker))
	//			break;

	//		CHECK_EQUAL(Marker.nID, 10+(int)i);
	//		CHECK_EQUAL(Marker.vPoint.IsEqual(vec3(10+i, 10+i, 10+i)), true);
	//	}

	//	CHECK_EQUAL(nMarkerQty, 4);	// 마커 갯수는 4개

	//	pNPC->Destroy();
	//	SAFE_DELETE(pNPC);
	//}

	//TEST_FIXTURE(Fixture, TestGlue_NPC_MoveToMarker)
	//{
	//	const char* pszTestFuncName = "_Test_NPC_MoveToMarker";
	//	const char* pszScript = 
	//		"function %s(NPC)\n"
	//		"	local Marker = MARKER_INFO();\n"
	//		"	if GetMarker(100, 777, Marker) == true then\n"
	//		"		NPC:MoveToMarker(Marker);\n"
	//		"	end\n"
	//		"end";

	//	if (!RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

	//	MARKER_INFO MarkerInfo;
	//	MarkerInfo.nID = 777;
	//	MarkerInfo.nFieldID = 100;
	//	MarkerInfo.vPoint = vec3(7737.0f, 3321.0f, 2134.0f);
	//	m_pFieldInfo->InsertMarkerInfo(MarkerInfo);

	//	GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, VCommonDir);
	//	WCALL1(WLUA, pszTestFuncName, pNPC, WNULL);
	//	m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.
	//	GModuleAI* pmAI = pNPC->GetModuleAI();
	//	if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }
	//	GJob* pJob = pmAI->GetCurrentJob();
	//	if (!pJob) 	{ CHECK(FALSE && "Job is null.");	 return; }
	//	CHECK_EQUAL(pJob->GetID(), GJOB_MOVE_TO_POS);
	//	//		GTaskBroker::PushTask_MoveStraight(pNPC, vec3(0,0,0));	// MoveJob에서 길찾기 실패로 태스크를 넣을 수 없으므로, 임시로 넣어둔다.

	//	GJob_MoveToPos* pMoveJob = static_cast<GJob_MoveToPos*>(pJob);
	//	CHECK_EQUAL(pMoveJob->GetTarPos().IsEqual(vec3(7737.0f, 3321.0f, 2134.0f)), true);

	//	pNPC->Destroy();
	//	SAFE_DELETE(pNPC);
	//}

	//TEST_FIXTURE(Fixture, TestGlue_NPC_Move)
	//{
	//	const char* pszTestFuncName = "_Test_NPC_Move";
	//	const char* pszScript = 
	//		"function %s(NPC)\n"
	//		"	NPC:Move(vec3(742, 512, 0));\n"
	//		"end";

	//	if (!RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

	//	GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, VCommonDir);
	//	WCALL1(WLUA, pszTestFuncName, pNPC, WNULL);
	//	m_pField->Update(1.0f);		// 잡이 큐에서 Current로 이동된다.
	//	GModuleAI* pmAI = pNPC->GetModuleAI();
	//	if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }
	//	GJob* pJob = pmAI->GetCurrentJob();
	//	if (!pJob) 	{ CHECK(FALSE && "Job is null.");	 return; }
	//	CHECK_EQUAL(pJob->GetID(), GJOB_MOVE_TO_POS);

	//	GJob_MoveToPos* pMoveJob = static_cast<GJob_MoveToPos*>(pJob);
	//	CHECK_EQUAL(pMoveJob->GetTarPos().IsEqual(vec3(742, 512, 0)), true);

	//	pNPC->Destroy();
	//	SAFE_DELETE(pNPC);
	//}

}
