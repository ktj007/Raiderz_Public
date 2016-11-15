#include "StdAfx.h"
#include "GColtMgr.h"
#include "GColtPicker.h"
#include "GColtInfo.h"
#include "GColtRunner.h"
#include "GEntityNPC.h"
#include "GJobRunner.h"
#include "GJobMgr.h"
#include "GJob_Broker.h"
#include "GTalentInfoMgr.h"
#include "GGlobal.h"
#include "GAIProfiler.h"
#include "GModuleAI.h"
#include "GJob_Talent.h"
#include "GNPCStress.h"
#include "GAIMonitorPort.h"
#include "GConst.h"

GColtMgr::GColtMgr(GJobMgr* pJobMgr, GJobRunner* pJobRunner)
: m_pColtReaction(NULL)
, m_pColtIdle(NULL)
, m_pColtCombat(NULL)
, m_pColtAggro(NULL)
, m_pColtStress(NULL)
, m_pColtPrologue(NULL)
, m_pColtEpilogue(NULL)
, m_pColtJobRunner(NULL)
, m_pColtTaskRunner(NULL)
, m_pJobMgr(pJobMgr)
, m_pJobRunner(pJobRunner)
{
	m_pColtJobRunner	= new GColtRunner(pJobRunner);
	m_pColtTaskRunner	= new GColtTaskRunner(pJobRunner);
}

GColtMgr::~GColtMgr(void)
{
	SAFE_DELETE(m_pColtJobRunner);
	SAFE_DELETE(m_pColtTaskRunner);

	Clear();
}

void GColtMgr::Init(GColtGroupInfo* pColtInfo)
{
	VALID(pColtInfo);

	Clear();

	vector<GColtCheck*>* vecChecks = 
		pColtInfo->vecCombatChecks;

	m_pColtCombat			= new GColtPicker(GColtPicker::SECTION_MULTI, COLT_COMBAT, pColtInfo->vecCombatChecks, pColtInfo->checkCombatDefault);
	m_pColtAggro			= new GColtPicker(GColtPicker::SECTION_SINGLE, COLT_AGGRO, &pColtInfo->vecAggroChecks, &pColtInfo->checkAggroDefault);
	m_pColtIdle				= new GColtPicker(GColtPicker::SECTION_SINGLE, COLT_IDLE, &pColtInfo->vecIdleChecks, &pColtInfo->checkIdleDefault);
	m_pColtReaction			= new GColtPicker(GColtPicker::SECTION_SINGLE, COLT_REACTION, &pColtInfo->vecReactionChecks, NULL);
	m_pColtStress			= new GColtPicker(GColtPicker::SECTION_SINGLE, COLT_STRESS, &pColtInfo->vecStressChecks, NULL);
	m_pColtPrologue			= new GColtPicker(GColtPicker::SECTION_SINGLE, COLT_PROLOGUE, &pColtInfo->vecPrologue, &pColtInfo->checkPrologueDefault);
	m_pColtEpilogue			= new GColtPicker(GColtPicker::SECTION_SINGLE, COLT_EPILOGUE, &pColtInfo->vecEpilogue, &pColtInfo->checkEpilogueDefault);
}

void GColtMgr::Clear()
{
	if (m_pColtReaction)		m_pColtReaction->Reset();
	if (m_pColtIdle)			m_pColtIdle->Reset();
	if (m_pColtCombat)			m_pColtCombat->Reset();
	if (m_pColtAggro)			m_pColtAggro->Reset();
	if (m_pColtStress)			m_pColtStress->Reset();
	if (m_pColtPrologue)		m_pColtPrologue->Reset();
	if (m_pColtEpilogue)		m_pColtEpilogue->Reset();
	

	SAFE_DELETE(m_pColtReaction);
	SAFE_DELETE(m_pColtIdle);
	SAFE_DELETE(m_pColtCombat);
	SAFE_DELETE(m_pColtAggro);
	SAFE_DELETE(m_pColtStress);
	SAFE_DELETE(m_pColtPrologue);
	SAFE_DELETE(m_pColtEpilogue);
}

bool GColtMgr::RunColtCombat( GEntityNPC* pThisNPC, GEntityActor* pOpponentActor )
{
	AI_PFI_BLOCK(APT_ID_COLT_COMBAT, L"ColtCombat");

	GNPCInfo* pNPCInfo = pThisNPC->GetNPCInfoMaster();
	VALID_RET(pNPCInfo, false);

	const GColtGroupInfo& ColtInfo = pNPCInfo->ColtInfo;
	const GColtGroupInfo::CombatAttr& combat_attr = ColtInfo.attrCombat[pThisNPC->GetMode()];
	int nDownAttackTalentID = combat_attr.nDownAttackTalentID;

	// 다운 공격
	if (IsUsableDownAttack(nDownAttackTalentID, pThisNPC, pOpponentActor))
	{
		USETALENT_PARAM param;
		param.bCheckValidate = true;
		param.bGainStress = true;
		m_pJobRunner->UseTalent(pThisNPC, pOpponentActor, nDownAttackTalentID, param);
	}
	// 콜트 시나리오
	else
	{
		GColtCheck* pCheck = m_pColtCombat->PickCheck(pThisNPC);
		if (!pCheck)	return false;		

		RUNNING_ACTION_LIST vecRunningAction = 
			m_pColtCombat->PickAction(*pCheck);

		RunColtActionList(vecRunningAction, pThisNPC, pOpponentActor);
	}

	return true;
}

bool GColtMgr::RunColtReaction(GEntityNPC* pThisNPC, GEntityActor* pOpponentActor)
{
	VALID_RET(pThisNPC, false);
//	VALID_RET(pOpponentActor, false);	// 디버프로 인해 공격받을때는 상대방이 없을 수 있음

	AI_PFI_BLOCK(APT_ID_COLT_REACTION, L"ColtReaction");

	if (pThisNPC->GetTickVars().bProceedReactionColt)	return false;

	pThisNPC->GetTickVars().bProceedReactionColt = true;
	
	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(pThisNPC->GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("Colt: Reaction");
	}

	return RunColt(m_pColtReaction, pThisNPC, pOpponentActor);
}

bool GColtMgr::RunColtAggro(GEntityNPC* pThisNPC, GEntityActor* pOpponentActor)
{
	VALID_RET(pThisNPC, false);
	VALID_RET(pOpponentActor, false);

	AI_PFI_BLOCK(APT_ID_COLT_AGGRO, L"ColtAggro");

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(pThisNPC->GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("Colt: Aggro");
	}

	return RunColt(m_pColtAggro, pThisNPC, pOpponentActor);
}

bool GColtMgr::RunColtPrologue(GEntityNPC* pThisNPC, GEntityActor* pOpponentActor)
{
	VALID_RET(pThisNPC, false);

	AI_PFI_BLOCK(APT_ID_COLT_AGGRO, L"ColtPrologue");

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(pThisNPC->GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("Colt: Prologue");
	}

	return RunColt(m_pColtPrologue, pThisNPC, pOpponentActor);
}

bool GColtMgr::RunColtEpilogue(GEntityNPC* pThisNPC, GEntityActor* pOpponentActor)
{
	VALID_RET(pThisNPC, false);

	AI_PFI_BLOCK(APT_ID_COLT_AGGRO, L"ColtEpilogue");

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(pThisNPC->GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("Colt: Epilogue");
	}

	return RunColt(m_pColtEpilogue, pThisNPC, pOpponentActor);
}


bool GColtMgr::RunColtIdle(GEntityNPC* pThisNPC)
{
	PFI_BLOCK_THISFUNC(9071);

	VALID_RET(pThisNPC, false);

	AI_PFI_BLOCK(APT_ID_COLT_IDLE, L"ColtIdle");

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(pThisNPC->GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("Colt: Idle");
	}

	return RunColt(m_pColtIdle, pThisNPC, NULL);
}

bool GColtMgr::RunColtStress(GEntityNPC* pThisNPC)
{
	VALID_RET(pThisNPC, false);

	AI_PFI_BLOCK(APT_ID_COLT_STRESS, L"ColtStress");

	GEntityActor* pOpponentActor = pThisNPC->GetTargetActor();
	if (!pOpponentActor)		return false;

	GColtCheck* pCheck = m_pColtStress->PickCheck(pThisNPC);
	if (!pCheck)			return false;

	RUNNING_ACTION_LIST vecRunningAction = 
		m_pColtStress->PickAction(*pCheck);

	if (vecRunningAction.empty())	return false;

	//회전 탤런트가 수행중이면 현재잡 취소하기	
	GJob* pJob = pThisNPC->GetJobMgr()->GetCurrJob();	
	if (pJob)	
	{
		if (pJob->GetID() == GJOB_TALENT)
		{
			GJob_UseTalent* pJobTalent = static_cast<GJob_UseTalent*>(pJob);
			if (!pJobTalent->IsRunMainTalent())
			{
				pThisNPC->GetJobMgr()->CancelCurrJob();
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (GConst::AIMON_ENABLE &&
			GAIMonitorPort::Inst().IsSetedNPC(pThisNPC->GetUID()))
		{
			GAIMonitorPort::Inst().OnJobEvent(false, "ClearJob - cause by 'Run ColtStress'");
		}
		pThisNPC->GetJobMgr()->Clear(true);
	}

	// 스트레스 초기화
	pThisNPC->GetNPCStress().Clear();

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(pThisNPC->GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("Colt: Stress");
	}

	return RunColtActionList(vecRunningAction, pThisNPC, pOpponentActor);;
}

bool GColtMgr::RunColt(GColtPicker* pColt, GEntityNPC* pThisNPC, GEntityActor* pOpponentActor)
{
	GColtCheck* pCheck = pColt->PickCheck(pThisNPC);
	if (!pCheck)			return false;

	RUNNING_ACTION_LIST vecRunningAction = 
		pColt->PickAction(*pCheck);

	return RunColtActionList(vecRunningAction, pThisNPC, pOpponentActor);
}

GTask* GColtMgr::RunColtIdleForJob(GEntityNPC* pThisNPC)
{
	GColtCheck* pCheck = m_pColtIdle->PickCheck(pThisNPC);
	if (!pCheck)				return NULL;

	RUNNING_ACTION_LIST vecRunningAction = 
		m_pColtIdle->PickAction(*pCheck);

	if (vecRunningAction.empty())	return NULL;

	return m_pColtTaskRunner->RunAIAction(vecRunningAction[0].first, vecRunningAction[0].second, pThisNPC, NULL);
}

bool GColtMgr::IsUsableDownAttack(int nTalentID, GEntityNPC* pThisNPC, GEntityActor* pTargetActor)
{
	if (nTalentID == INVALID_TALENT_ID)							return false;
	if (!pTargetActor->IsKnockdowned())							return false;
	if (!pThisNPC->CheckTalentRange(pTargetActor, nTalentID))	return false;
	if (!gmgr.pTalentInfoMgr->Find(nTalentID))						
	{
		dlog("Invalid DownAttackTalentID = %d\n", nTalentID);
		return false;
	}

	return true;
}

bool GColtMgr::RunColtActionList(const RUNNING_ACTION_LIST& vecRunningAction, GEntityNPC* pThisNPC, GEntityActor* pOpponentActor)
{
	if (vecRunningAction.empty())		return false;

	for (size_t i=0; i<vecRunningAction.size(); i++)
	{
		const pair<const GColtAction*, GColtActionInst*>& RunningAction = vecRunningAction[i];
		const GColtAction*			pNextAction		= RunningAction.first;
		GColtActionInst*	pNextInstInfo	= RunningAction.second;

		if (!pNextAction)	continue;

		m_pColtJobRunner->RunAction(pNextAction, pNextInstInfo, pThisNPC, pOpponentActor);
	}

	return true;
}

bool GColtMgr::HasPrologueColt() const
{
	if (!m_pColtPrologue)
		return false;

	return m_pColtPrologue->Has();
}

bool GColtMgr::HasEpilogueColt() const
{
	if (!m_pColtEpilogue)
		return false;

	return m_pColtEpilogue->Has();
}
