#include "stdafx.h"
#include "XPvPAreaChecker.h"
#include "XMyPlayer.h"

XPvPAreaChecker::XPvPAreaChecker() : m_bCurrentPvPArea(false)
{
	m_CheckRegulator.SetTickTime(PVP_AREA_CHECKER_CHECK_TICK);
}

bool XPvPAreaChecker::CheckPvPStateChange( float fDeltaTime, XFieldInfo* pFieldInfo, vec3 vPos )
{
	if (!m_CheckRegulator.IsReady(fDeltaTime))		return false ;
	if (pFieldInfo == NULL) return false;

	bool bPvPArea = CheckPvPArea(pFieldInfo, vPos);

	bool ret = (m_bCurrentPvPArea != bPvPArea);
	m_bCurrentPvPArea = bPvPArea;

	return ret;
}

bool XPvPAreaChecker::CheckPvPArea( XFieldInfo* pFieldInfo, vec3 vPos )
{
	int x = (int)vPos.x;
	int y = (int)vPos.y;

	for (size_t i = 0; i < pFieldInfo->m_PvPAreas.size(); ++i)
	{
		PVP_AREA_INFO& area = pFieldInfo->m_PvPAreas[i];

		if (area.rcArea.left <= x && 
			area.rcArea.right >= x &&
			area.rcArea.top <= y &&
			area.rcArea.bottom >= y)
		{
			m_CurrentPvPAreaInfo = area;
			return true;
		}
	}

	return false;
}
