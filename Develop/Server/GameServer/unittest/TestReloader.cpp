#include "stdafx.h"
#include "GFieldInfoMgr.h"
#include "SUnitTestUtil.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GModuleAI.h"
#include "MockEntityNPC.h"
#include "GReloader.h"
#include "GColtPicker.h"
#include "GColtMgr.h"
#include "GTalentInfoMgr.h"
#include "GFieldMgr.h"
#include "GUTHelper_Field.h"
#include "GNPCMgr.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

class FakeLoadingNPCInfoMgr :public GNPCInfoMgr
{
	virtual bool		Load(const wchar_t* szFileName, int nExpectedID=-1) override
	{
		GNPCInfo* m_pNPCInfo = new GNPCInfo;
		m_pNPCInfo->nID = 1;
		m_pNPCInfo->nMaxHP = 50000;
		wchar_t szBuff[128];
		swprintf_s(szBuff, L"NPC_%d", m_pNPCInfo->nID);
		m_pNPCInfo->strNameInLua = szBuff;
		gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);

		return true;
	}
};

SUITE(Reloader)
{
	struct Fixture
	{
		Fixture()
		{ 
			m_pFieldInfo = new GFieldInfo;
			m_pFieldInfo->m_nFieldID = 100;
			m_pFieldInfo->m_strFieldFile = L"100";
			m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
			gmgr.pFieldInfoMgr->Insert(m_pFieldInfo);

			m_pField = GUTHelper_Field::NewMockField();
			m_pField->SetFieldInfo(m_pFieldInfo);
			gmgr.pFieldMgr->AddField(m_pField);

			m_pNPCInfo = new GNPCInfo;
			m_pNPCInfo->nID = 1;
			m_pNPCInfo->nMaxHP = 50000;
			wchar_t szBuff[128];
			swprintf_s(szBuff, L"NPC_%d", m_pNPCInfo->nID);
			m_pNPCInfo->strNameInLua = szBuff;
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);
		}

		~Fixture() 
		{
			m_pField->Update(1.0f);
			gmgr.pFieldMgr->DeleteField(m_pField->GetUID());
		}

		GTalentInfo* MakeSimpleDmgAtkTalent(const int nDamage, int nTalentID=INVALID_TALENT_ID)
		{
			if (nTalentID == INVALID_TALENT_ID)		nTalentID = INT_MAX-2;

			GTalentInfo* pAktTalent = gmgr.pTalentInfoMgr->Find(nTalentID);
			if (pAktTalent)			
			{
				m_Helper.SetTalentDamage(pAktTalent, nDamage);
				return pAktTalent;
			}

			pAktTalent = m_Helper.NewMeleeTalentInfo(nTalentID);
			m_Helper.SetTalentDamage(pAktTalent, nDamage);
			m_Helper.AddSimpleHitInfo(pAktTalent);

			return pAktTalent;
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GFieldInfo*		m_pFieldInfo;
		GNPCInfo*		m_pNPCInfo;

		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper2<GNPCInfoMgr, FakeLoadingNPCInfoMgr>				m_NPCInfoMgr;
	};

	//TEST_FIXTURE(Fixture, NPCReload_Colt)
	//{
	//	gmgr.pNPCMgr->Clear();

	//	GColtCheck Check;
	//	vector<GColtActionInst>* pqActionInstances=NULL;
	//	GColtAction Action1;
	//	GColtAction Action2;
	//	GColtAction Action3;
	//	GColtAction Action4;
	//	Action1.nRate = 0;
	//	Action1.nType = CAT_TALENT;
	//	Action2.nRate = 0;
	//	Action2.nType = CAT_YELL;
	//	Action3.nRate = 100;
	//	Action3.nType = CAT_MOTIONFACTOR;
	//	Action4.nRate = 0;
	//	Action4.nType = CAT_BREAKPART;
	//	Action4.bAutoRun = true;
	//		
	//	Check.vecActions.push_back(Action1);
	//	Check.vecActions.push_back(Action2);
	//	Check.vecActions.push_back(Action3);
	//	Check.vecAutoRunActions.push_back(Action4);

	//	m_pNPCInfo->ColtInfo.checkCombatDefault[NPC_MODE_DEFAULT] = Check;

	//	GColtCheck& CheckCombatDefault = m_pNPCInfo->ColtInfo.checkCombatDefault[NPC_MODE_DEFAULT]; 
	//	
	//	GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
	//	GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
	//	GModuleAI* pmAI = pNPC->GetModuleAI();
	//	
	//	CHECK_EQUAL(pmAI->GetColt()->GetColtCombat()->GetActionInstanceSize(), 0);
	//	CHECK_EQUAL(pmAI->GetColt()->GetColtCombat()->GetAutorunActionInstanceSize(), 0);

	//	pmAI->GetColt()->RunColtCombat(pNPC, pPlayer);
	//	CHECK_EQUAL(pmAI->GetColt()->GetColtCombat()->GetActionInstanceSize(), 1);
	//	CHECK_EQUAL(pmAI->GetColt()->GetColtCombat()->GetAutorunActionInstanceSize(), 1);
	//	const vector<GColtActionInst>& ActionInstances			= pmAI->GetColt()->GetColtCombat()->LookupActionInst(CheckCombatDefault);
	//	const vector<GColtActionInst>& AutorunActionInstances	= pmAI->GetColt()->GetColtCombat()->LookupAutorunActionInst(CheckCombatDefault);
	//	CHECK_EQUAL(ActionInstances.size(), 3);
	//	CHECK_EQUAL(AutorunActionInstances.size(), 1);

	//	GReloader reroader;
	//	reroader.Reload_NPC(1);

	//	CHECK_EQUAL(pmAI->GetColt()->GetColtCombat()->GetActionInstanceSize(), 0);
	//	CHECK_EQUAL(pmAI->GetColt()->GetColtCombat()->GetAutorunActionInstanceSize(), 0);

	//	pNPC->DeleteMe();
	//	pPlayer->DeleteMe();
	//}

	//TEST_FIXTURE(Fixture, NPCReload_Script)
	//{
	//}

	//TEST_FIXTURE(Fixture, FieldReload_Data)
	//{
	//	SPAWN_INFO* pSpawninfo = new SPAWN_INFO();
	//	pSpawninfo->bCheckSpawnPointValidation = false;
	//	pSpawninfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
	//			
	//	MockField* pNewField = GUTHelper_Field::NewMockField(m_pFieldInfo);
	//	gmgr.pFieldMgr->AddField(pNewField);
	//	GEntityNPC* pNPC = pNewField->SpawnTestNPC(m_pNPCInfo, vec3(1234.1f, 5678.2f, 9012.3f), vCommonDir, pSpawninfo);

	//	CHECK_EQUAL(pNewField->GetNPCQty(1), 1);

	//	GReloader reloader;
	//	reloader.Reload_Field(100);

	//	CHECK_EQUAL(pNewField->GetNPCQty(1), 0);

	//	SAFE_DELETE(pSpawninfo);
	//	pNewField->Update(1.0f);
	//	pNewField->Destroy();
	//}
}


