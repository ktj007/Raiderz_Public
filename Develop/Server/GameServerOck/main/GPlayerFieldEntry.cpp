#include "stdafx.h"
#include "GPlayerFieldEntry.h"
#include "GFieldMgr.h"
#include "GSharedField.h"
#include "GGlobal.h"
#include "GPlayerField.h"
#include "GEntityPlayer.h"
#include "GTestSystem.h"

//////////////////////////////////////////////////////////////////////////

void GPlayerFieldEntry::ReserveWarp( MUID uidField, vec3 vPos, vec3 vDir )
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vPos);
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vDir);

	m_WarpReservedInfo.uidField = uidField;
	m_WarpReservedInfo.vPos = vPos;
	m_WarpReservedInfo.vDir = vDir;
}

void GPlayerFieldEntry::ReserveWarp(int nField, vec3 vPos)
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vPos);

	m_WarpReservedInfo.nField = nField;
	m_WarpReservedInfo.vPos = vPos;
}

void GPlayerFieldEntry::ClearReserveWarp()
{
	m_WarpReservedInfo.Reset();
}

bool GPlayerFieldEntry::OnEnterFieldComplete()
{
	GField* pField = gmgr.pFieldMgr->GetField(m_WarpReservedInfo.uidField);
	if (!pField) 
		return false;

	if (m_pLisener)
	{
		gsys.pTestSystem->CheckNaN(__FUNCTIONW__, m_WarpReservedInfo.vPos);
		gsys.pTestSystem->CheckNaN(__FUNCTIONW__, m_WarpReservedInfo.vDir);
		m_pLisener->OnGateComplete(pField, m_WarpReservedInfo.vPos, m_WarpReservedInfo.vDir);

		if (pField->IsSharedField())
			m_pLisener->OnEnterSharedField(ToSharedField(pField), m_WarpReservedInfo.vPos, m_WarpReservedInfo.vDir);
		else if (pField->IsDynamicField())
			m_pLisener->OnEnterDynamicField(ToDynamicField(pField), m_WarpReservedInfo.vPos, m_WarpReservedInfo.vDir);
		else
			VALID_RET(false, false);
	}

	return true;
}

bool GPlayerFieldEntry::IsReserved()
{
	if (MUID::ZERO != m_WarpReservedInfo.uidField ||
		0 != m_WarpReservedInfo.nField) return true;

	return false;
}
