#include "StdAfx.h"
#include "GPlayerFieldLazyGateway.h"
#include "GEntityPlayer.h"
#include "GPlayerField.h"
#include "GPlayerFieldDynamic.h"

#include "GGlobal.h"
#include "GFieldInfoMgr.h"
#include "GFieldInfo.h"
#include "GInnRoomInfo.h"
#include "GInnRoomInfoMgr.h"


GPlayerFieldLazyGateway::GPlayerFieldLazyGateway(GEntityPlayer* pOwner, GPlayerField* pPlayerField)
: m_pOwner(pOwner)
, m_pPlayerField(pPlayerField)
, m_bScheduled(false)
{
}

GPlayerFieldLazyGateway::~GPlayerFieldLazyGateway()
{
}

void GPlayerFieldLazyGateway::ProcessScheduledGate()
{
	if (IsScheduled())
	{
		if (INVALID_ID != m_tdLazyGateInfo.m_nFieldGroupID)
		{
			m_pPlayerField->GetFieldDynamic().GateRequest(
				m_tdLazyGateInfo.m_eType
				, m_tdLazyGateInfo.m_nFieldGroupID
				, m_tdLazyGateInfo.m_nFieldID
				, m_tdLazyGateInfo.m_vPos
				, m_tdLazyGateInfo.m_vDir
				, m_tdLazyGateInfo.m_bSingleEnter);
		}
		else
		{
			m_pPlayerField->Gate(m_tdLazyGateInfo.m_nFieldID, m_tdLazyGateInfo.m_vPos, m_tdLazyGateInfo.m_vDir);
		}

		m_tdLazyGateInfo.Reset();
		SetSchedule(false);
	}
}

bool GPlayerFieldLazyGateway::GateToMarker(int nFieldID, int nMarkerID)
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	if (!pFieldInfo)
		return false;

	const MARKER_INFO* pMarker = pFieldInfo->FindMarker(nMarkerID);
	if (!pMarker)
		return false;

	GateToMarker(pMarker);
	return true;
}
void GPlayerFieldLazyGateway::GateToMarker(const MARKER_INFO* pMarker)
{
	VALID(pMarker);

	m_tdLazyGateInfo.Set(pMarker->nFieldID, pMarker->vPoint, pMarker->vDir);
	SetSchedule(true);
}

bool GPlayerFieldLazyGateway::Gate(int nDestFieldID, vec3 vecDestPos, vec3 vecDestDir)
{
	m_tdLazyGateInfo.Set(nDestFieldID, vecDestPos, vecDestDir);
	SetSchedule(true);
	return true;
}
bool GPlayerFieldLazyGateway::GateToActor(GEntityActor* pActor)
{
	RVALID_RET(NULL != pActor, false);

	m_tdLazyGateInfo.Set(pActor->GetFieldID(), pActor->GetPos(), pActor->GetDir());
	SetSchedule(true);
	return true;
}
bool GPlayerFieldLazyGateway::GateToTrial(int nDynamicFieldGroupID, bool bSingleEnter)
{
	FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoMgr->FindFieldGroup(nDynamicFieldGroupID);
	if (!pFieldGroupInfo)
		return false;

	if (NULL == pFieldGroupInfo->pStartFieldInfo ||
		NULL == pFieldGroupInfo->pStartMarkerInfo) return false;

	m_tdLazyGateInfo.Set(DFT_TRIAL
		, nDynamicFieldGroupID
		, pFieldGroupInfo->pStartMarkerInfo->nFieldID
		, pFieldGroupInfo->pStartMarkerInfo->vPoint
		, pFieldGroupInfo->pStartMarkerInfo->vDir
		, bSingleEnter);
	
	SetSchedule(true);
	return true;
}
bool GPlayerFieldLazyGateway::GateToInnRoom()
{
	GInnRoomInfo* pInnRoomInfo = gmgr.pInnRoomInfoMgr->Get(m_pOwner->GetPlayerInfo()->nInnRoomID);
	if (!pInnRoomInfo)
		return false;

	FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoMgr->FindFieldGroup(pInnRoomInfo->m_nFieldGroupID);
	if (!pFieldGroupInfo)
		return false;

	m_tdLazyGateInfo.Set(DFT_INNROOM
		, pInnRoomInfo->m_nFieldGroupID
		, pFieldGroupInfo->pStartMarkerInfo->nFieldID
		, pFieldGroupInfo->pStartMarkerInfo->vPoint
		, pFieldGroupInfo->pStartMarkerInfo->vDir
		, true);

	SetSchedule(true);
	return true;
}
