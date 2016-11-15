#pragma once

/// 선형 보간
class XLinearInterpolation
{
private:
	float m_fValue;
	float m_fStep;
	float m_fRemainingTime;
	float m_fTo;
public:
	bool Setup(float from, float to, float time);
	bool Interpolate(float deltaTime);	// 도착하면 TRUE 반환

	float GetValue() { return m_fValue; }
};


/// 방향 보간 (구면 보간)
class XDirectionInterpolation
{
private:
	vec3	m_vFrom;
	vec3	m_vTo;
	float	m_fIntervalTime;
	float	m_fElapsedTime;
	vec3	m_vValue;

	MQuat	m_qSrc;
	MQuat	m_qDest;
public:
	bool Setup(vec3 vFromDir, vec3 vToDir, float fTime);
	bool Interpolate(float fDeltaTime);	// 도착하면 TRUE 반환

	vec3 GetValue() { return m_vValue; }
	vec3 GetFrom() { return m_vFrom; }
	vec3 GetTo() { return m_vTo; }
};
