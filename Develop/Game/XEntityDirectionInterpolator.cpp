#include "stdafx.h"
#include "XEntityDirectionInterpolator.h"
#include "XActor.h"
#include "XModuleEntity.h"

XEntityDirectionInterpolator::XEntityDirectionInterpolator( XModuleEntity* pModuleEntity ) : m_pModuleEntity(pModuleEntity), m_bInterpolation(false)
{

}

XEntityDirectionInterpolator::~XEntityDirectionInterpolator()
{

}

bool XEntityDirectionInterpolator::Update( float fDelta )
{
	if (m_bInterpolation == false) return false;

	bool bInterpoRet = m_DirInterpo.Interpolate(fDelta);

	vec3 vInterpoDir = m_DirInterpo.GetValue();
	SetDirection(vInterpoDir);

	if (bInterpoRet)
	{
		m_bInterpolation = false;
		ResetRenderDirection();
	}


	return true;
}

void XEntityDirectionInterpolator::Start( vec3 vCurrDir, vec3 vToDir, float fRotateSpeed )
{
	float fAngle = vCurrDir.AngleBetween(vToDir);

	if (fAngle <= 0.001f) return;

	if (fAngle < 0.01f)
	{
		SetDirection(vToDir);
		ResetRenderDirection();
		return;
	}

	// 한바퀴 도는 시간
	float fTime = (fAngle * fRotateSpeed) / MMath::TWOPI;
	fTime = max(0.001f, fTime);

	vCurrDir.Normalize();
	vToDir.Normalize();

	m_DirInterpo.Setup(vCurrDir, vToDir, fTime);
	m_bInterpolation = true;

}

void XEntityDirectionInterpolator::Stop()
{
	m_bInterpolation = false;
}

void XEntityDirectionInterpolator::SetDirection( vec3 vDir )
{
	const XObject* pCaller = NULL;
	m_pModuleEntity->SetDirection(vDir, pCaller);
	m_pModuleEntity->SetRenderDirection(vDir);
}

void XEntityDirectionInterpolator::ResetRenderDirection()
{
	vec3 vRenderDirection = m_pModuleEntity->GetRenderDirection();

	// NPCAligner에서 z축을 바꿨으면 RenderDirection은 계속 유지시킨다.
	if (XMath::IsZero(vRenderDirection.z, 0.01f) == true)
	{
		m_pModuleEntity->SetRenderDirection(vec3::ZERO);
	}
}
