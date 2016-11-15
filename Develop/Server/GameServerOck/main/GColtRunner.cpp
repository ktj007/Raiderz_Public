#include "stdafx.h"
#include "GColtRunner.h"
#include "CCommandTable.h"
#include "GColtPicker.h"
#include "GEntityNPC.h"
#include "GJob_Talent.h"
#include "GModuleAI.h"
#include "GField.h"
#include "GJob_Broker.h"
#include "GJobRunner.h"
#include "GTalentInfoMgr.h"
#include "GMath.h"
#include "GGlobal.h"
#include "GHateTable.h"
#include "GNPCHitcapsuleGroup.h"
#include "GTalentHitRouter.h"
#include "GColtPicker.h"
#include "GNPCBParts.h"

//////////////////////////////////////////////////////////////////////////
//
//		GColtRunner
//
//////////////////////////////////////////////////////////////////////////

bool GColtRunner::RunColtActionList(const RUNNING_ACTION_LIST& vecActions, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	if (vecActions.empty())		return false;

	for (size_t i=0; i<vecActions.size(); i++)
	{
		const pair<const GColtAction*, GColtActionInst*>& RunningAction = vecActions[i];
		const GColtAction*	pAction		= RunningAction.first;
		GColtActionInst*	pInstInfo	= RunningAction.second;

		if (!pAction)	
			continue;

		RunAction(pAction, pInstInfo, pThisNPC, pOpponent);
	}

	return true;
}

void GColtRunner::RunAction( const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent) ///< 해당 액션 실
{
	PFI_BLOCK_THISFUNC(307);

	if (NULL == pPickedAction)
		return;

	pPickedActionInstance->IncreaseUseCount();

	MUID uidOpponent = (pOpponent)?pOpponent->GetUID():MUID::Invalid();

	switch(pPickedAction->nType)
	{
	case CAT_GROUP:																									break;
	case CAT_NOTHING:				OnNothing(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_TALENT:				OnTalent(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_LUAFUNC:				OnLuaFunc(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_BREAKPART:				OnBreakpart(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_DAMAGE:				OnDamage(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_MOTIONFACTOR:			OnMotionfactor(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);		break;
	case CAT_FLEE:					OnFlee(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);				break;
	case CAT_YELL:					OnYell(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);				break;
	case CAT_GUARD:					OnGuard(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);				break;
	case CAT_ADJUST:				OnAdjust(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_DISTANCE:				OnDistance(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_FOLLOW:				OnFollow(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_MOVE:					OnMove(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);				break;
	case CAT_CHANGE_HIT_CAPSULE:	OnChangeHitCapsule(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);	break;
	case CAT_SAY:					OnSay(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);				break;
	case CAT_BALLOON:				OnBalloon(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_SHOUT:					OnShout(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);				break;
	case CAT_NARRATION:				OnNarration(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	case CAT_AGGRO:					OnAggro(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);				break;
	case CAT_FACETO:				OnFaceTo(pPickedAction, pPickedActionInstance, pThisNPC, pOpponent);			break;
	default:
		{
//			mlog3("잘못된 콜트 액션 타입 (NPCID: %d, Type:%d)\n", pThisNPC->GetID(), pPickedAction->nType);
		}
	}
}

void GColtRunner::OnMotionfactor(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	const MF_STATE nType = (MF_STATE)_wtoi(pPickedAction->qParams[0].c_str());
	const int nWeight = _wtoi(pPickedAction->qParams[1].c_str());;

	pThisNPC->DoMotionFactor(nType, nWeight, pOpponent, false);

	GTalentHitRouter talentHitRouter;
	talentHitRouter.Route_MotionFactor(pOpponent, pThisNPC, nType, nWeight, 0);
}

void GColtRunner::OnBreakpart(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	if (NULL == pOpponent)	return;
	MUID uidOpponent = pOpponent->GetUID();

	uint8 nPartID = _wtoi(pPickedAction->qParams[0].c_str());

	if (pThisNPC->GetNPCBParts())
		pThisNPC->GetNPCBParts()->Break(nPartID, uidOpponent);
}

void GColtRunner::OnDamage(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	if (NULL == pOpponent)	return;
	MUID uidOpponent = pOpponent->GetUID();

	const uint32 nDamageType = _wtoi(pPickedAction->qParams[0].c_str());
	const uint32 nDamage	 = _wtoi(pPickedAction->qParams[1].c_str());

	if (pThisNPC->doDamage(uidOpponent, DAMAGE_ATTRIB(nDamageType), nDamage))
	{
		pThisNPC->doDie();
	}
}

void GColtRunner::OnFlee(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	float fDurationTime = 5.0f;

	wstring strFleeType = pPickedAction->qParams[0];
	FleeType nFleeType = Flee_FromEnemy;
	if (strFleeType == L"from_enemy")
		nFleeType = Flee_FromEnemy;
	else if (strFleeType == L"to_home")
		nFleeType = Flee_ToHome;
	else
		DCHECK(false && L"정의되지 않은 flee type");

	if (pPickedAction->qParams.size() > 0 &&
		pPickedAction->qParams[1].length() > 0)
	{
		fDurationTime = (float)_wtof(pPickedAction->qParams[1].c_str());
	}

	m_pJobRunner->Flee( pThisNPC->GetModuleAI(), nFleeType, fDurationTime );
}

void GColtRunner::OnYell(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	int nRange=0;

	if (pPickedAction->qParams.size() > 0 &&
		pPickedAction->qParams[0].length() > 0)
	{
		nRange = _wtoi(pPickedAction->qParams[0].c_str());
	}
	
	m_pJobRunner->Yell( pThisNPC->GetModuleAI(), nRange );
}

void GColtRunner::OnAdjust(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	wstring strType = pPickedAction->qParams[0];
	int nValue = _wtoi(pPickedAction->qParams[1].c_str());

	assert(pThisNPC->GetNPCInfo());
	if (!pThisNPC->GetNPCInfo())		return;

	GColtGroupInfo& ColtInfo = pThisNPC->GetNPCInfoMaster()->ColtInfo;

	if (NPC_XML_ATTR_VICTORY == strType)
	{
		ColtInfo.attrCombat[pThisNPC->GetMode()].nVictoryTalentID = nValue;
	}
}

void GColtRunner::OnDistance(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	GEntityActor* pTargetActor = pThisNPC->GetTargetActor();
	if (!pTargetActor) return;

	wstring strMinDistance = pPickedAction->qParams[0];
	wstring strMaxDistance = pPickedAction->qParams[1];
	double fDistance = 0.0f;
	if (true == strMinDistance.empty())
	{
		fDistance = _wtof(strMaxDistance.c_str());
	}
	else if(true == strMaxDistance.empty())
	{
		fDistance = _wtof(strMinDistance.c_str());
	}
	else
	{
		double fMinDistance = _wtof(strMinDistance.c_str());
		double fMaxDistance = _wtof(strMaxDistance.c_str());
		fDistance = RandomNumber((float)fMinDistance, (float)fMaxDistance);
	}

	float fDurationTime = pPickedActionInstance->GetDurationTime();
	pThisNPC->MoveToPos(pTargetActor, (float)fDistance, fDurationTime);
}

void GColtRunner::OnFollow(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	VALID(pPickedAction);
	VALID(pPickedActionInstance);

	GEntityActor* pEaTarget = pThisNPC->GetTargetActor();
	if ( NULL == pEaTarget)
		return;

	wstring strDistance = pPickedAction->qParams[ 0];

	float fDistance = 0.0f;
	if ( ! strDistance.empty())
		fDistance = (float)_wtoi( strDistance.c_str());

	float fDurationTime = pPickedActionInstance->GetDurationTime();
	bool bInfinity		= pPickedActionInstance->IsInfinityTime();
	bool bValidate		= false;

	if (bInfinity)
	{
		fDurationTime = 0.0f;
	}

	if ( pThisNPC->GetPos().DistanceTo( pEaTarget->GetPos()) > (float)fDistance)
	{
		m_pJobRunner->Follow( pThisNPC->GetModuleAI(), pEaTarget, fDistance, fDurationTime);
	}
}

void GColtRunner::OnMove(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	GEntityActor* pEaTarget = pThisNPC->GetTargetActor();
	if (NULL == pEaTarget) return;

	wstring strMinMove = pPickedAction->qParams[0];
	wstring strMaxMove = pPickedAction->qParams[1];
	int nMove = 0;
	if (true == strMinMove.empty())
	{
		nMove = _wtoi(strMaxMove.c_str());
	}
	else if(true == strMaxMove.empty())
	{
		nMove = _wtoi(strMinMove.c_str());
	}
	else
	{
		int nMinMove = _wtoi(strMinMove.c_str());
		int nMaxMove = _wtoi(strMaxMove.c_str());
		nMove = RandomNumber(nMinMove, nMaxMove);
	}

	vec3 vMoveDirection = pThisNPC->GetPos() - pEaTarget->GetPos();
	GMath::NormalizeZ0(vMoveDirection);

	vec3 vRightPrep, vLeftPrep;
	vRightPrep.x = -vMoveDirection.y;
	vRightPrep.y = vMoveDirection.x;

	vLeftPrep.x = vMoveDirection.y;
	vLeftPrep.y = -vMoveDirection.x;

	vec3 vMovePoint = vec3::ZERO;
	int nRightRotate = RandomNumber(0, 1);
	if (1 == nRightRotate)
	{
		vMovePoint.x = pThisNPC->GetPos().x + vRightPrep.x * nMove;
		vMovePoint.y = pThisNPC->GetPos().y + vRightPrep.y * nMove;
	}
	else
	{
		vMovePoint.x = pThisNPC->GetPos().x + vLeftPrep.x * nMove;
		vMovePoint.y = pThisNPC->GetPos().y + vLeftPrep.y * nMove;
	}
	float fLimitSumOfPathLength = static_cast<float>(nMove * 4);

	m_pJobRunner->Move(pThisNPC->GetModuleAI(), vMovePoint, vec3::ZERO, fLimitSumOfPathLength);
}

void GColtRunner::OnChangeHitCapsule(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	int8 nCapsuleGroupIndex = _wtoi(pPickedAction->qParams[0].c_str());

	pThisNPC->GetNPCHitcapsuleGroup().ChangeCapsuleGroup_ByTrigger(nCapsuleGroupIndex);
}

void GColtRunner::OnAggro(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	wstring strType = pPickedAction->qParams[0];
	float fDuration = 0.0f;
	if (!pPickedAction->qParams[1].empty())
	{
		fDuration = static_cast<float>(_wtof(pPickedAction->qParams[1].c_str()));
	}
	
	HATE_TARGET_SWAP_EVENT nEventType = HATESWAP_NONE;
	if		(strType == NPC_XML_ATTR_AGGRO_FLIP)		nEventType = HATESWAP_FLIP;
	else if (strType == NPC_XML_ATTR_AGGRO_RANDOM)		nEventType = HATESWAP_RANDOM;
	else if (strType == NPC_XML_ATTR_AGGRO_FAR)			nEventType = HATESWAP_FAR;
	else if (strType == NPC_XML_ATTR_AGGRO_NEAR)		nEventType = HATESWAP_NEAR;
	else if (strType == NPC_XML_ATTR_AGGRO_SHORT)		nEventType = HATESWAP_SHORT;

	pThisNPC->GetHateTable().ApplyEvent(nEventType, fDuration);
}

void GColtRunner::OnTalent(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	VALID(pPickedAction);
	VALID(pPickedActionInstance);

	int nTalentID = _wtoi(pPickedAction->qParams[0].c_str());

	MUID uidTarget;
	if (NULL != pOpponent)
	{
		uidTarget = pOpponent->GetUID();
	}

	bool bGainStress	= pPickedActionInstance->IsGainStress();
	bool bInfinity		= pPickedActionInstance->IsInfinityTime();

	// DurationTime 설정 ------------------------------
	float fDurationTime = 0.0f; 
	if (pPickedActionInstance->IsModifiedDurationTime())
	{
		fDurationTime = pPickedActionInstance->GetDurationTime();
	}
	else
	{
		VALID(pThisNPC->GetNPCInfo());
		fDurationTime = pThisNPC->GetNPCInfo()->fChaseMaxDurationTime;
	}

	if (bInfinity)
		fDurationTime = 0.0f;	// infinity time


	// Validate 설정 -------------------------------
	bool bValidate		= false;
	if (bInfinity || fDurationTime > 0.0f)
	{
		bValidate = true;
	}

	USETALENT_PARAM param;
	param.bCheckValidate = bValidate;
	param.bRotatable = bValidate;
	param.bGainStress = bGainStress;
	param.fChaseLimitTime = fDurationTime;

	m_pJobRunner->UseTalent(pThisNPC, pOpponent, nTalentID, param);
}

void GColtRunner::OnGuard(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	const int nGuardTalentID = _wtoi(pPickedAction->qParams[0].c_str());
	if (nGuardTalentID <= 0) return;

	const float fGuardTime = (float)_wtof(pPickedAction->qParams[1].c_str());
	if (fGuardTime <= 0)	return;

	m_pJobRunner->Guard(pThisNPC->GetModuleAI(), nGuardTalentID, fGuardTime);
}

void GColtRunner::OnNothing(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	float fDelayTime = (float)_wtof(pPickedAction->qParams[0].c_str());
	if (fDelayTime <= 0.0f)	return;

	m_pJobRunner->Idle(pThisNPC->GetModuleAI(), fDelayTime);
}


void GColtRunner::OnLuaFunc(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	wstring strTable;
	wstring strFunc;
	if (!pPickedAction->qParams[1].empty())
	{
		strTable = pPickedAction->qParams[0];	
		strFunc = pPickedAction->qParams[1];	
	}
	else
	{
		strFunc = pPickedAction->qParams[0];	
	}

	m_pJobRunner->RunScript(pThisNPC->GetModuleAI(), pOpponent?pOpponent->GetUID():MUID::Invalid(), strTable, strFunc);
}

void GColtRunner::OnSay(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	wstring strText = pPickedAction->qParams[0];
	float fDelayTime = (float)_wtof(pPickedAction->qParams[1].c_str());
	m_pJobRunner->Speak(pThisNPC->GetModuleAI(), L"say", strText, fDelayTime);
}

void GColtRunner::OnBalloon(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	wstring strText = pPickedAction->qParams[0];
	float fDelayTime = (float)_wtof(pPickedAction->qParams[1].c_str());
	m_pJobRunner->Speak(pThisNPC->GetModuleAI(), L"balloon", strText, fDelayTime);
}

void GColtRunner::OnShout(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	wstring strText = pPickedAction->qParams[0];
	float fDelayTime = (float)_wtof(pPickedAction->qParams[1].c_str());
	m_pJobRunner->Speak(pThisNPC->GetModuleAI(), L"shout", strText, fDelayTime);
}

void GColtRunner::OnNarration(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	wstring strText = pPickedAction->qParams[0];
	m_pJobRunner->Speak(pThisNPC->GetModuleAI(), L"narration", strText, 0.0f);	// 나레이션은 항상 대기하지 않는다.
}

void GColtRunner::OnFaceTo(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	m_pJobRunner->FaceTo(pThisNPC->GetModuleAI(), pOpponent->GetUID());
}

//////////////////////////////////////////////////////////////////////////
//
//		GColtTaskRunner
//

GTask* GColtTaskRunner::RunAIAction(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent )
{
	switch(pPickedAction->nType)
	{
	case CAT_NOTHING:		return OnNothing(pPickedAction, pThisNPC, pOpponent);	
	case CAT_TALENT:		return OnTalent(pPickedAction, pThisNPC, pOpponent);	
	}

	return NULL;
}

GTask* GColtTaskRunner::OnNothing(const GColtAction* pPickedAction, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	float fDelayTime = (float)_wtof(pPickedAction->qParams[0].c_str());
	if (fDelayTime <= 0.0f)	return NULL;

	return m_TaskBroker.New_Idle(pThisNPC, fDelayTime);
}

GTask* GColtTaskRunner::OnTalent(const GColtAction* pPickedAction, GEntityNPC* pThisNPC, GEntityActor* pOpponent)
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(_wtoi(pPickedAction->qParams[0].c_str()));
	if (!pTalentInfo)	return NULL;

	MUID uidTarget;
	if (NULL != pOpponent)
	{
		uidTarget = pOpponent->GetUID();
	}

	return m_TaskBroker.New_UseTalent(pThisNPC, pTalentInfo, uidTarget, false, true);
}
