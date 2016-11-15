#pragma once

#include "XFieldInfo.h"
#include "XTimer.h"

#define PVP_AREA_CHECKER_CHECK_TICK			1.0f

class XPvPAreaChecker
{
protected:
	bool					m_bCurrentPvPArea;
	PVP_AREA_INFO			m_CurrentPvPAreaInfo;
	XRegulator				m_CheckRegulator;
	bool CheckPvPArea(XFieldInfo* pFieldInfo, vec3 vPos);
public:
	XPvPAreaChecker();
	~XPvPAreaChecker() {}
	bool CheckPvPStateChange(float fDeltaTime, XFieldInfo* pFieldInfo, vec3 vPos);
	bool IsCurrentPvPArea() { return m_bCurrentPvPArea; }
	PVP_AREA_INFO& GetCurrentPvPArea()	{ return m_CurrentPvPAreaInfo; }
};