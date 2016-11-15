#include "StdAfx.h"
#include "GPlayerFieldGateRetryer.h"
#include "GPlayerField.h"

const float GPlayerFieldGateRetryer::TIME_TO_UPDATE_SEC = 3.0f;

GPlayerFieldGateRetryer::GPlayerFieldGateRetryer(GEntityPlayer* pOwner, GPlayerField* pPlayerField)
: m_pOwner(pOwner)
, m_pPlayerField(pPlayerField)
, m_bIsRunRetry(false)
, m_nDestFieldID(INVALID_ID)
{
	m_rgltRetry.SetTickTime(TIME_TO_UPDATE_SEC);
}

GPlayerFieldGateRetryer::~GPlayerFieldGateRetryer()
{
}

void GPlayerFieldGateRetryer::Update(float fDelta)
{
	if (!IsRunRetryer()) return;
	if (!m_rgltRetry.IsReady(fDelta)) return;

	StopRetryer();

	m_pPlayerField->Gate(m_nDestFieldID, m_vPos, m_vDir);
	
	ClearRetryInfo();
}

void GPlayerFieldGateRetryer::StartRetryer(int nFieldID, vec3 vPos, vec3 vDir)
{
	m_bIsRunRetry = true;

	m_nDestFieldID = nFieldID;
	m_vPos = vPos;
	m_vDir = vDir;

	m_rgltRetry.Start();
}

void GPlayerFieldGateRetryer::StopRetryer()
{
	m_bIsRunRetry = false;

	m_rgltRetry.Stop();
}

void GPlayerFieldGateRetryer::ClearRetryInfo()
{
	m_nDestFieldID = INVALID_ID;
	m_vPos = vec3(0,0,0);
	m_vDir = vec3(0,0,1);
}