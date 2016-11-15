#include "stdafx.h"
#include "GNPCAI.h"
#include "GModuleAI.h"
#include "GHateTable.h"
#include "GBehaviorStateMachine.h"
#include "GJobMgr.h"
#include "GAIProfiler.h"
#include "GConst.h"
#include "GAIState.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GNPCAILodEventSelector.h"
#include "GGlobal.h"
#include "GAIEventMgr.h"
#include "GNPCAIDefaultUpdater.h"
#include "GNPCAIEmptyFieldUpdater.h"
#include "GNPCAILod1Updater.h"
#include "GNPCAILod2Updater.h"
#include "GNPCAILod3Updater.h"
#include "GNPCAILod4Updater.h"
#include "GNPCAILod5Updater.h"
#include "GConfig.h"


GNPCAI::GNPCAI(GModuleAI* pModuleAI)
: m_pModuleAI(pModuleAI)
, m_uidOwner(MUID::ZERO)
{
	_ASSERT(m_pModuleAI);

	// AI
	m_pBehaviorFSM = new GBehaviorStateMachine(m_pModuleAI);
	m_pJobMgr = new GJobMgr(m_pModuleAI);

	
	// AI LOD
	m_pLod = new GNPCAILod;
	m_pLodSelector = new GNPCAILodEventSelector(m_pModuleAI->GetOwnerNPC());
	m_pLodSelector->RegistHandler(m_pLod);
	m_aiLastUpdateLevel = m_pLod->GetLodLevel();

	// NPC AI Update Delegate
	m_pDefaultUpdater = new GNPCAIDefaultUpdater(this);
	m_pEmptyFieldUpdater = new GNPCAIEmptyFieldUpdater(this);
	m_pLodUpdater[AI_LOD_LEVEL_1] = new GNPCAILod1Updater(this);
	m_pLodUpdater[AI_LOD_LEVEL_2] = new GNPCAILod2Updater(this);
	m_pLodUpdater[AI_LOD_LEVEL_3] = new GNPCAILod3Updater(this);
	m_pLodUpdater[AI_LOD_LEVEL_4] = new GNPCAILod4Updater(this);
	m_pLodUpdater[AI_LOD_LEVEL_5] = new GNPCAILod5Updater(this);
}

GNPCAI::~GNPCAI()
{
	SAFE_DELETE(m_pDefaultUpdater);
	SAFE_DELETE(m_pEmptyFieldUpdater);
	
	for(int i = 0; i < MAX_AI_LOD_LEVEL; i++)
	{
		SAFE_DELETE(m_pLodUpdater[i]);
	}

	m_pLodSelector->UnregistHandler();
	Finalize();

	SAFE_DELETE(m_pLod);
	SAFE_DELETE(m_pLodSelector);

	// 순서를 지켜야합니다. 
	// JobMgr가 BehaviorFSM 보다 먼저 삭제 되어야 함.
	SAFE_DELETE(m_pJobMgr);
	SAFE_DELETE(m_pBehaviorFSM);
}

void GNPCAI::Initialize(void)
{
	_ASSERT(m_uidOwner == MUID::Invalid());

	m_uidOwner = m_pModuleAI->GetOwnerUID();	
	gmgr.pAIEventMgr->AddEventHandler(m_uidOwner, m_pLodSelector);
}

void GNPCAI::Finalize(void)
{
	if (m_uidOwner.IsValid())
	{
		gmgr.pAIEventMgr->RemoveEventHandler(m_uidOwner, m_pLodSelector);
	}
}

void GNPCAI::Update(float fDelta)
{
	PFC_THISFUNC;

	PFI_BLOCK(301, "GNPCAI::Update");
	AI_PFI_BLOCK(APT_ID_NPC_AI, L"NPC AI");

	// AI 최적화 적용여부 확인
	if (!GConst::AI_OPTIMIZATION)
	{
		m_pDefaultUpdater->Update(fDelta);
		return;
	}


	AI_LOD_LEVEL aiLodLevel = m_pLod->GetLodLevel();
	_ASSERT(AI_LOD_LEVEL_1 <= aiLodLevel && aiLodLevel < MAX_AI_LOD_LEVEL);


	// AI LOD 허용 필드 확인
	GEntityNPC* pOwnerNPC = m_pModuleAI->GetOwnerNPC();
	_ASSERT(pOwnerNPC != NULL);

	if ( (!pOwnerNPC->IsAILodEnableFiled()) ||
		 (!pOwnerNPC->GetNPCInfo()->m_bEnableAILod) )
	{
		PFC_BLOCK("GNPCAI::Update - 1");

		// 테스트 코드 ---------------------------------------
		if (CONFIG_AUTO_TEST_AI == GConfig::m_strAutoTestType)
		{
			m_pDefaultUpdater->Update(fDelta);
			return;
		}
		// 테스트 코드 --------------------------------------^

		if (aiLodLevel == AI_LOD_LEVEL_5)
		{
			m_pEmptyFieldUpdater->Update(fDelta);
		}
		else
		{
			m_pDefaultUpdater->Update(fDelta);
		}
		
		return;
	}

	{
		PFC_BLOCK("GNPCAI::Update - 2");

		// AI LOD 적용 업데이트
		if (m_aiLastUpdateLevel != aiLodLevel)
		{
			fDelta += m_pLodUpdater[aiLodLevel]->ResetRemainDelta();
			m_aiLastUpdateLevel = aiLodLevel;
		}

		m_pLodUpdater[aiLodLevel]->Update(fDelta);
	}
}

void GNPCAI::UpdateBehaviorFSM(float fDelta)
{
	PFI_BLOCK(302, "GNPCAI::UpdateBehaviorFSM");
	AI_PFI_BLOCK(APT_ID_BEHAVIOR, L"UpdateBehaviorFSM");

	m_pBehaviorFSM->Update(fDelta);
}

void GNPCAI::UpdateFindEnemy(float fDelta)
{
	PFI_BLOCK(303, "GNPCAI::UpdateFindEnemy");
	AI_PFI_BLOCK(APT_ID_FIND_ENEMY, L"FindEnemy");

	GEntityNPC* pOwnerNPC = m_pModuleAI->GetOwnerNPC();
	_ASSERT(pOwnerNPC != NULL);

	if (pOwnerNPC->IsNowReturn())
		return; // 전투 종료후 돌아가는 중에는 AA 작동 안함

	m_EnemyFinder.Update(fDelta);

	MUID nEnemyUID = m_EnemyFinder.Find(pOwnerNPC);
	if (true == nEnemyUID.IsInvalid()) return;

	EVENT_FOUND_ENEMY param;
	param.uidTarget = nEnemyUID;
	GMessage msg(GMSG_AI_EVT_FIND_TARGET, &param);
	m_pModuleAI->Message(msg);
}

void GNPCAI::UpdateJobMgr(float fDelta)
{
	PFI_BLOCK(305, "GNPCAI::UpdateJobMgr");
	AI_PFI_BLOCK(APT_ID_JOB, L"UpdateJobMgr");

	VALID(m_pModuleAI);
	VALID(m_pModuleAI->GetOwnerNPC());

	m_pJobMgr->Run(fDelta);
}

void GNPCAI::BehaviorRecreate(void)
{
	SAFE_DELETE(m_pBehaviorFSM);

	m_pBehaviorFSM = new GBehaviorStateMachine(m_pModuleAI);

	m_pBehaviorFSM->Init();
}

void GNPCAI::BehaviorReset(void)
{
	m_pBehaviorFSM->ChangeState(AI_BEHAVIOR_PEACE);	
	
	BehaviorRecreate();
	
	m_pBehaviorFSM->ChangeState(AI_BEHAVIOR_PEACE);
}

GBehaviorStateMachine* GNPCAI::GetBehaviorFSM(void)
{
	return m_pBehaviorFSM;
}

GJobMgr* GNPCAI::GetJobMgr(void)
{
	return m_pJobMgr;
}

AI_LOD_LEVEL GNPCAI::GetLodLevel( void ) const
{
	return m_pLod->GetLodLevel();
}

deque<GJob*> GNPCAI::GetMissionJobs()
{
	return m_pBehaviorFSM->GetMissionJobs();
}
