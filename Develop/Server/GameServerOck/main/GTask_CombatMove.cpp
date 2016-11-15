#include "stdafx.h"
#include "GTask_CombatMove.h"
#include "GConst.h"
#include "CCommandTable.h"
#include "GPathFinder.h"
#include "GEntityNPC.h"
#include "GField.h"

GTask_CombatMove::GTask_CombatMove(GEntityNPC* pEntity, vec3 vTargetPos) 
: GTask(pEntity)
, m_vTargetPos(vTargetPos)
, m_bOldMoveStopByContext(false)
{
	ASSERT(pEntity->GetNPCInfo());

	m_fRotateSpeed = pEntity->GetNPCInfo()->fRotateSpeed;

	m_nOldPathCounter = 0;
}

GTask_CombatMove::~GTask_CombatMove()
{

}

bool GTask_CombatMove::OnStart()
{
	__PROFILE_THIS_FUNC(508);

	_FindPath();

	return true;
}

GTaskResult GTask_CombatMove::OnRun(float fDelta)
{	
	__PROFILE_THIS_FUNC(501);

	if (!m_pOwner->GetField() ||
		!m_pOwner->GetField()->GetInfo())	
	{
		return TR_CANCELED;
	}

	bool bMoveStopByContext;
	bool bNewPathLine;

	GTaskResult ret = _AdvancedAlongPath(fDelta, bMoveStopByContext, bNewPathLine);
	if (ret != TR_NONE)	return ret;

	_RouteStatus(bMoveStopByContext, bNewPathLine);
	
	return TR_RUNNING;
}

void GTask_CombatMove::OnComplete()
{
 	_MoveStop();
}

bool GTask_CombatMove::OnCancel()
{
	_MoveStop();

	return true;
}

void GTask_CombatMove::Reset( vec3 vTarPos )
{
	m_vTargetPos = vTarPos;
	OnStart();
}

void GTask_CombatMove::_RouteStatus(bool bMoveStopByContext, bool bNewPathLine)
{
	bool bMoved = false;
	bool bStopped = false;

	// 이전 정지 상태와 비교해 바뀌었을때만 클라에게 알려준다.
	if (bMoveStopByContext != m_bOldMoveStopByContext)
	{
		if (bMoveStopByContext)
		{
			// 이동중 컨텍스트 충돌
			bStopped = true;
		}
		else
		{
			m_pOwner->RouteMove(m_vAdvancePos, m_fRotateSpeed);

			bMoved = true;
		}

		m_bOldMoveStopByContext = bMoveStopByContext;
	}

	m_bOldMoveStopByContext = bMoveStopByContext;

	if (bNewPathLine &&
		(!bMoved && !bStopped))
	{
		m_pOwner->RouteMove(m_vAdvancePos, m_fRotateSpeed);
	}

	if (bStopped)
		_MoveStop();
}

GTaskResult GTask_CombatMove::_AdvancedAlongPath(float fDelta, bool& boutMoveStopByContext, bool& boutNewPathLine)
{
	__PROFILE_THIS_FUNC(6003);

	MPath& m_path = m_pOwner->GetPath();
	int nZoneID = m_pOwner->GetField()->GetInfo()->m_nZoneID;
	float fDistance = m_pOwner->GetSpeed()*fDelta;

	boutMoveStopByContext = 
		(GPathFinder::GetInstance().AdvancedAlongPath(NULL, nZoneID, m_pOwner->GetPathAgent(), m_path, fDistance) == COLLISION);

	if (!m_path.IsValid())
	{	
		if (!m_bOldMoveStopByContext)
		{
			m_bOldMoveStopByContext = true;
			_MoveStop();
		}
		NotifyTaskEventToJob(TE_PATH_FIND_FAILED);
		return TR_RUNNING;
	}

	if (m_path.GetLength() == 0)
	{
		NotifyTaskEventToJob(TE_PATH_FIND_FAILED);
		return TR_CANCELED;
	}

	boutNewPathLine = false;
	if (m_path.GetSize() >= 2 &&
		m_path.GetSize() != m_nOldPathCounter)
	{	
		m_nOldPathCounter = m_path.GetSize();

		GPathFinder::GetInstance().PathTo3DPoint(m_vAdvancePos, nZoneID, m_path, 1);

		boutNewPathLine = true;
	}

	vec3 vAgentPos;
	m_pOwner->GetPathAgent().GetPosition(&vAgentPos);

	if (vAgentPos.IsEqual(m_pOwner->GetPos()))
	{
		// 전진해도 같은 자리라면
		NotifyTaskEventToJob(TE_PATH_MOVE_BLOCKED);
		return TR_RUNNING;
	}

	m_pOwner->SetPos(vAgentPos);

	return TR_NONE;
};

void GTask_CombatMove::_MoveStop()
{
	m_pOwner->RouteMoveStop();
}

bool GTask_CombatMove::_FindPath()
{
	__PROFILE_THIS_FUNC(6000);
	
	if (m_pOwner->GetField() == NULL ||
		m_pOwner->GetField()->GetInfo() == NULL)
	{
		return false;
	}

	MContext* pContext = m_pOwner->GetPathAgent().GetContext();

	m_pOwner->GetPath().Release();

	vec3 vPos = m_pOwner->GetPos();
	m_pOwner->GetField()->GetGroundVerticalPos(vPos);

	int nZoneID = m_pOwner->GetField()->GetInfo()->m_nZoneID;


	PATHENGINE_RESULT_TYPE nPathResultType = GPathFinder::GetInstance().IsValidPosition(NULL, nZoneID, pContext, m_vTargetPos);
	if ( nPathResultType==COLLISION)
	{
		PATHENGINE_RESULT_TYPE ret =
			GPathFinder::GetInstance().FindNearPosition(&m_vTargetPos, nZoneID, pContext, m_vTargetPos, GConst::PATH_NEAR_HORIZONE_RANGE, GConst::PATH_NEAR_VERTICAL_RANGE, m_pOwner->GetShapeSize());

		if (ret != PATH_SUCCESS)
		{
			NotifyTaskEventToJob(TE_PATH_FIND_FAILED);
			return false;
		}
	}

	if (!GPathFinder::GetInstance().FindPath(m_pOwner->GetPath(), 
		nZoneID, 
		m_pOwner->GetPathAgent(), 
		m_vTargetPos,
		m_pOwner->GetShapeSize()))
	{
		NotifyTaskEventToJob(TE_PATH_FIND_FAILED);
		return false;
	}

#if 0
	_RouteDebugInfo();
#endif

	m_nOldPathCounter = 0;
	m_vAdvancePos.Set(0.0f, 0.0f, 0.0f);

	return true;
}

void GTask_CombatMove::_RouteDebugInfo()
{
	GEntityNPC* pOwner = m_pOwner;
	int nSize = m_pOwner->GetPath().GetSize();
	vec3 vFirstTarPos;

	vec3* pPathList = new vec3[nSize+1];

	pPathList[0] = m_pOwner->GetPos();

	for (int i=0; i<nSize; i++)
	{
		MVector3 out;
		GPathFinder::GetInstance().PathTo3DPoint(out, m_pOwner->GetField()->GetInfo()->m_nZoneID, m_pOwner->GetPath(), i);
		pPathList[i+1] = out;

		if (i==1)
			vFirstTarPos = out;
	}

	if (GConst::TEST_SHOW_CLIENT_NPC_MOVE_PATH &&
		m_pOwner &&
		m_pOwner->GetField())
	{
		MUID uidTarget = m_pOwner->GetUID();

		char buff[128];
		sprintf_s(buff, 128, "%u", uidTarget.High);

		MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, 4, 
			NEW_STR("MovePathTrace"), 
			NEW_INT(uidTarget.Low), 
			NEW_STR(buff), 
			NEW_BLOB(pPathList, sizeof(vec3), static_cast<unsigned short>(nSize+1)));
		m_pOwner->GetField()->RouteToField(pNewCommand);
	}

	delete [] pPathList;
}