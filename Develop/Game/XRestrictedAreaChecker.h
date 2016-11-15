#pragma once

#include "XFieldInfo.h"

class XRestrictedAreaChecker
{
protected:
	float					m_fPreRestrictedAreaOutTime;
	float					m_fRestrictedAreaOutTime;

	bool					m_bStart;
	bool					m_bEnd;

private:
	void					Init();

public:
	XRestrictedAreaChecker();
	virtual ~XRestrictedAreaChecker() {};

	void					_UpdateRestrictedArea(float fDelta, vec3& vPos, XFieldInfo* pFieldInfo, CSFieldFileInfo* pFieldFileInfo);
};