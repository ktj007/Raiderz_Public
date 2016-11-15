#include "stdafx.h"
#include "GModuleAI.h"
#include "GJobRunner.h"
#include "GJob.h"
#include "GJobMgr.h"
#include "GColtMgr.h"
#include "GHateTable.h"
#include "GBehaviorState.h"
#include "GBehaviorStateMachine.h"
#include "GConst.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GAIProfiler.h"
#include "GNPCDebugger.h"
#include "GField.h"
#include "GEntityNPC.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GNPCAI.h"
#include "GNPCAIEventInvoker.h"
#include "GTestSystem.h"
#include "GGlobal.h"
#include "GTalentInfoMgr.h"
#include "GBuffInfo.h"

/////////////////////////////////////////////////////////////////////////////////////////
// GModuleAI ////////////////////////////////////////////////////////////////////////
GModuleAI::GModuleAI(GEntity* pOwner, GJobRunner* pJobRunner) 
: GModule(pOwner)
, m_pColtMgr(NULL)
, m_bDropCombat(false)
, m_bAssistMode(false)
, m_pJobRunner(pJobRunner)
{
	m_pNPCAI			= new GNPCAI(this);
	m_pColtMgr			= new GColtMgr(m_pNPCAI->GetJobMgr(), pJobRunner);
	m_pAIEventInvoker	= new GNPCAIEventInvoker(this);
}

GModuleAI::~GModuleAI()
{
	SAFE_DELETE(m_pAIEventInvoker);
	SAFE_DELETE(m_pNPCAI);
	SAFE_DELETE(m_pColtMgr);
}

void GModuleAI::OnInitialized()
{
	m_pNPCAI->Initialize();
}

void GModuleAI::OnFinalized()
{
	m_pNPCAI->Finalize();
}

void GModuleAI::OnSubscribeMessage()
{
	GModule::OnSubscribeMessage();

	Subscribe(GMSG_AI_EVT_THINK);
	Subscribe(GMSG_AI_EVT_TRY_ATTACKED);
	Subscribe(GMSG_AI_EVT_ATTACK);
	Subscribe(GMSG_AI_EVT_ATTACKED);
	Subscribe(GMSG_AI_EVT_FIND_TARGET);
	Subscribe(GMSG_AI_EVT_ARRIVED);
	Subscribe(GMSG_AI_EVT_BLOCKED);
	Subscribe(GMSG_AI_EVT_DEAD);
	Subscribe(GMSG_AI_EVT_FORGET_TARGET);
	Subscribe(GMSG_AI_EVT_STARTCOMBAT);
	Subscribe(GMSG_AI_EVT_ENDCOMBAT);
	Subscribe(GMSG_AI_EVT_FLEE);
	Subscribe(GMSG_AI_EVT_MISSION);
	Subscribe(GMSG_AI_EVT_ENDPROLOGUE);
	Subscribe(GMSG_AI_EVT_SESSION);
	Subscribe(GMSG_AI_EVT_SESSION_CHANGE_SCENE);
	Subscribe(GMSG_AI_EVT_SESSION_PUSHJOB);
	Subscribe(GMSG_AI_EVT_SESSION_FINISHED);
	Subscribe(GMSG_AI_EVT_JOB_CLEAR);
	Subscribe(GMSG_AI_EVT_JOB_COMPLETED);
	Subscribe(GMSG_AI_EVT_JOB_CANCELED);
	Subscribe(GMSG_AI_EVT_JOB_TIMEDOUT);
	Subscribe(GMSG_AI_EVT_JOB_FINISHED);
}

	GMessageResult GModuleAI::OnDefaultMessage(GMessage& msg)
{
	// Infomation: 상태 전이 이전에 핸들링되는 함수

	switch (msg.m_nID)
	{
	case GMSG_AI_EVT_TRY_ATTACKED:
		{
			GBehaviorStateMachine* pBehaviorFSM = m_pNPCAI->GetBehaviorFSM();			
			if (GetHateTable().IsEmpty())
			{
				EVENT_ATTACKED_INFO info = *(EVENT_ATTACKED_INFO*)(msg.m_pData);
				if (GetOwnerNPC()->GetField())
				{
					GEntityActor* pAttacker = GetOwnerNPC()->GetField()->FindActor(info.uidAttacker);
					if (pAttacker)
						GetHateTable().AddPoint_FoundEnemy(pAttacker);
				}
			}
		}break;
	case GMSG_AI_EVT_ATTACKED:
		{	
			EVENT_ATTACKED_INFO info = *(EVENT_ATTACKED_INFO*)(msg.m_pData);
			

			// HateFactor 구하기
			float fHateFactor = 1.0f;

			switch(info.nEffectSourceType)
			{
			case EST_NONE:
				fHateFactor = 1.0f;
				break;
			case EST_TALENT:
				{
					GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(info.nEffectSourceID);
					if (pTalentInfo != NULL)
					{
						fHateFactor = pTalentInfo->m_fHateFactor;
					}
				}
				break;
			case EST_BUFF:
				{
					GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(info.nEffectSourceID);
					if (pBuffInfo != NULL)
					{
						fHateFactor = pBuffInfo->m_fHateFactor;
					}
				}
				break;
			default:
				fHateFactor = 1.0f;
				break;
			}


			if (GetOwnerNPC()->GetField())
			{
				GEntityActor* pAttacker = GetOwnerNPC()->GetField()->FindActor(info.uidAttacker);
				if (pAttacker)
					GetHateTable().AddPoint_GainDamage(pAttacker, info.nDamage, fHateFactor);
			}
		}				
		break;
	case GMSG_AI_EVT_FIND_TARGET:
		{
			EVENT_FOUND_ENEMY info = *(EVENT_FOUND_ENEMY*)(msg.m_pData);
			if (GetOwnerNPC()->GetField())
			{
				GEntityActor* pAttacker = GetOwnerNPC()->GetField()->FindActor(info.uidTarget);
				if (pAttacker)
					GetHateTable().AddPoint_FoundEnemy(pAttacker);
			}
		}
		break;
	}

	return MR_FALSE;
}

GMessageResult GModuleAI::OnMessage(GMessage& msg)
{
	VALID_RET(GetOwnerNPC(), MR_FALSE);
	VALID_RET(GetOwnerNPC()->GetNPCInfo(), MR_FALSE);

	GBehaviorStateMachine* pBehaviorFSM = m_pNPCAI->GetBehaviorFSM();
	if (pBehaviorFSM->Message(msg) == MR_TRUE) 
		return MR_TRUE;
	if (OnDefaultMessage(msg) == MR_TRUE)	
		return MR_TRUE;

	return MR_FALSE;
}

void GModuleAI::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	AI_PFI_BLOCK(APT_ID_MODULE_AI, __FUNCTIONW__);
	PFI_BLOCK_THISFUNC(2001);

	// 이동 테스트
	if (IsRunForTest() && GConst::TEST_RANDOM_MOVE)
	{
		HandleMoveTest();
	}
	else
	{
		// 활동할 수 있는 상태이면 AI 업데이트		
		m_pNPCAI->Update(fDelta);
	}


	// 전투이탈 확인
	if (m_bDropCombat)
	{
		m_bDropCombat = false;

		GMessage msg(GMSG_AI_EVT_ENDCOMBAT);
		Message(msg);
	}
}

void GModuleAI::HandleMoveTest()
{
	GJobMgr* pJobMgr = m_pNPCAI->GetJobMgr();

	if (pJobMgr->GetCurrJob()) return;
	if (!GetOwnerNPC())	return;
	if (!GetOwnerNPC()->GetField())	return;
	if (!GetOwnerNPC()->GetField()->GetInfo())	return;

	const GFieldInfo* pInfo = GetOwnerNPC()->GetField()->GetInfo();

	vec3 vTarPos;
	vTarPos.x = MMath::RandomNumber(pInfo->MinX(), pInfo->MaxX());
	vTarPos.y = MMath::RandomNumber(pInfo->MinY(), pInfo->MaxY());
	vTarPos.z = 0.0f;

	if (!GetOwnerNPC()->GetField()->PickingZ(vTarPos, vTarPos))
	{
		gsys.pTestSystem->PickingLog(GetOwnerNPC()->GetUID(), L"pick_fail: GModuleAI::HandleMoveTest");
	}
			
	GJobRunner job_runner;
	job_runner.Move(this, vTarPos);
}

void GModuleAI::Init()
{
	GNPCInfo* pNPCInfo = GetOwnerNPC()->GetNPCInfoMaster();
	if (pNPCInfo)
	{
		m_pColtMgr->Init(&pNPCInfo->ColtInfo);
	}

	m_pNPCAI->BehaviorRecreate();
}

void GModuleAI::Reset()
{
	GJobMgr* pJobMgr = m_pNPCAI->GetJobMgr();
	pJobMgr->CancelCurrJob();
	pJobMgr->Clear(true);
	GNPCInfo* pNPCInfo = GetOwnerNPC()->GetNPCInfo();
	if (pNPCInfo)
	{
		m_pColtMgr->Init(&pNPCInfo->ColtInfo);
	}

	m_pNPCAI->BehaviorReset();
}

inline
GEntityNPC* GModuleAI::GetOwnerNPC() const
{ 
	if (NULL == m_pOwner)	return NULL;
	return static_cast<GEntityNPC*>(m_pOwner); 
}

GHateTable& GModuleAI::GetHateTable() const
{
	return GetOwnerNPC()->GetHateTable();
}

GBehaviorStateMachine*	GModuleAI::GetBehaviorFSM(void)
{
	return m_pNPCAI->GetBehaviorFSM();
}

GJobMgr* GModuleAI::GetJobMgr(void)
{
	return m_pNPCAI->GetJobMgr();
}

void GModuleAI::Logf(const wchar_t* szText, ...)
{
	wchar_t szBuff[1024];

	va_list args;
	va_start(args,szText);
	vswprintf_s(szBuff,szText,args);
	va_end(args);

	GetOwnerNPC()->GetDebugger()->Log(szBuff);
}

void GModuleAI::OnCombatBegin(void)
{
	m_pAIEventInvoker->CombatBegin();
}

void GModuleAI::OnCombatEnd(void)
{
	m_pAIEventInvoker->CombatEnd();
}

void GModuleAI::OnSpawnEmptyField(void)
{
	m_pAIEventInvoker->SpawnEmptyField();
}

void GModuleAI::OnSpawnEmptySector(void)
{
	m_pAIEventInvoker->SpawnEmptySector();
}

void GModuleAI::OnSpawnPlayerSector(void)
{
	m_pAIEventInvoker->SpawnPlayerSector();
}

void GModuleAI::OnMakeMajorLodNPC(void)
{
	m_pAIEventInvoker->MakeMajorLodNPC();
}

AI_LOD_LEVEL GModuleAI::GetLodLevel( void ) const
{
	return m_pNPCAI->GetLodLevel();
}

bool GModuleAI::IsNowInvincibility()
{
	if (GetBehaviorFSM()->GetCurrentStateID() == AI_BEHAVIOR_RETURN)
		return true; // 귀환중에는 무적

	return false;
}

deque<GJob*> GModuleAI::GetMissionJobs()
{
	return m_pNPCAI->GetMissionJobs();
}

bool GModuleAI::HasNPCSession() 
{
	return (GetBehaviorFSM()->GetCurrentStateID() == AI_BEHAVIOR_SESSION);
}
