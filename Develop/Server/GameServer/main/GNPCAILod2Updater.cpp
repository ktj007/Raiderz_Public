#include "stdafx.h"
#include "GNPCAILod2Updater.h"
#include "GNPCAI.h"
#include "GBehaviorStateMachine.h"
#include "GAIState.h"
#include "GAIProfiler.h"

GNPCAILod2Updater::GNPCAILod2Updater(GNPCAI* pSubject)
: GNPCAIUpdater(pSubject)
, m_fActionCumulateDeltaSeconds(0.0f)
, m_fTargetCumulateDeltaSeconds(0.0f)
{		
	m_ActionUpdateRegulator.SetTickTime(0.5f);
}

GNPCAILod2Updater::~GNPCAILod2Updater()
{
	// do nothing
}

void GNPCAILod2Updater::Update(float fDelta)
{
	AI_PFI_BLOCK(APT_ID_LOD_LEVEL_2, L"LOD 2");

	// 업데이트 프레임인지 확인
	bool isAction = m_ActionUpdateRegulator.IsReady(fDelta);
	bool isTarget = false;

	if (AI_BEHAVIOR_PEACE == m_pSubject->GetBehaviorFSM()->GetCurrentStateID())
	{
		m_TargetUpdateRegulator.SetTickTime(1.0f);
		isTarget = m_TargetUpdateRegulator.IsReady(fDelta);
	}
	else if (AI_BEHAVIOR_MISSION== m_pSubject->GetBehaviorFSM()->GetCurrentStateID() ||
			 AI_BEHAVIOR_SESSION== m_pSubject->GetBehaviorFSM()->GetCurrentStateID())
	{
		m_TargetUpdateRegulator.SetTickTime(0.5f);
		isTarget = m_TargetUpdateRegulator.IsReady(fDelta);
	}


	// 누적된 DeltaTime만큼 업데이트
	m_fActionCumulateDeltaSeconds += fDelta;
	m_fTargetCumulateDeltaSeconds += fDelta;

	if (isAction)	m_pSubject->UpdateBehaviorFSM(m_fActionCumulateDeltaSeconds);
	if (isTarget)	m_pSubject->UpdateFindEnemy(m_fTargetCumulateDeltaSeconds);
	
	// 스크립트 관련, 이동과 이동 사이 멈추는 딜레이를 없에기 위해 항상 업데이트합니다.
	m_pSubject->UpdateJobMgr(fDelta);		// 수정 시, fDelta 주의

	if (isAction)	m_fActionCumulateDeltaSeconds = 0.0f;
	if (isTarget)	m_fTargetCumulateDeltaSeconds = 0.0f;
}

float GNPCAILod2Updater::ResetRemainDelta(void)
{
	float fRemainDelta = m_fActionCumulateDeltaSeconds;

	m_fActionCumulateDeltaSeconds = 0.0f;
	m_fTargetCumulateDeltaSeconds = 0.0f;

	return fRemainDelta;
}