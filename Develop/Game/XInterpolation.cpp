#include "stdafx.h"
#include "XInterpolation.h"

bool XLinearInterpolation::Setup( float from, float to, float time )
{
	if(time < 0.0f)
	{
		return false;
	}
	m_fTo = to;
	m_fRemainingTime = time;
	m_fValue = from;
	m_fStep = (to-from)/time; // Calculate distance per second.
	return true;
}

bool XLinearInterpolation::Interpolate( float deltaTime )
{
	m_fRemainingTime -= deltaTime;
	m_fValue += m_fStep*deltaTime;

	if (m_fRemainingTime <= 0.0f)
	{
		m_fValue = m_fTo;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool XDirectionInterpolation::Setup( vec3 vFromDir, vec3 vToDir, float fTime )
{
	if (fTime < 0.0f)
	{
		return false;
	}

	m_vFrom = vFromDir;
	m_vTo = vToDir;

	m_fIntervalTime = fTime;
	m_fElapsedTime = 0.0f;
	m_vValue = vFromDir;

	MMatrix mat;

	mat.SetLocalMatrix(vec3::ZERO, m_vFrom, vec3::AXISZ);
	m_qSrc = MQuat(mat);

	mat.SetLocalMatrix(vec3::ZERO, m_vTo, vec3::AXISZ);
	m_qDest = MQuat(mat);

	return true;
}

bool XDirectionInterpolation::Interpolate( float fDeltaTime )
{
	m_fElapsedTime += fDeltaTime;
	if (m_fElapsedTime >= m_fIntervalTime)
	{
		m_vValue = m_vTo;
		return true;
	}

	MQuat qResult;
	qResult.Slerp(m_qSrc, m_qDest, m_fElapsedTime / m_fIntervalTime);

	MMatrix matResult = qResult.Conv2Matrix();
	m_vValue = vec3::AXISY * matResult;

	return false;
}

