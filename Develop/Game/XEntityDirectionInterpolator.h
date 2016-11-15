#pragma once

#include "XInterpolation.h"

class XModuleEntity;

class XEntityDirectionInterpolator
{
protected:
	XModuleEntity*				m_pModuleEntity;
	XDirectionInterpolation		m_DirInterpo;
	bool						m_bInterpolation;

	void						SetDirection( vec3 vDir );
	void						ResetRenderDirection();
public:
	XEntityDirectionInterpolator(XModuleEntity* pModuleEntity);
	virtual ~XEntityDirectionInterpolator();
	bool Update(float fDelta);
	void Start(vec3 vCurrDir, vec3 vToDir, float fRotateSpeed);		// fRotateSpeed는 한바퀴 도는데 드는 시간
	void Stop();
	bool IsDoing() { return m_bInterpolation; }
};