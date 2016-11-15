#include "stdafx.h"
#include "XSensor.h"
#include "XEffectInvoker.h"
#include "XEffectManager.h"
#include "XMyPlayer.h"
#include "XPost_Comm.h"

void XSensor::CreateSensorEffect(SENSOR_INFO* pInfo, GATE_TYPE nGateType)
{
	if (m_pSensorInfo == NULL) return;
	if (m_pSensorInfo->strEffect.empty()) return;
	if (nGateType == TGT_NONE) return;

	wstring& strEffectName = m_pSensorInfo->strEffect;

	XEffectSinglyInvoker invoker;

	vector<EFFECT_RESULT_DATA> vecResult;
	vec3 vPos = m_pSensorInfo->vPoint + m_pSensorInfo->vEffectLocalPos;
	vec3 vDir = m_pSensorInfo->vEffectDir;
	vec3 vUp = m_pSensorInfo->vEffectUp;

	EFFECT_POST_TRANSFORM postData;
	postData.vSize = m_pSensorInfo->vEffectScale;

	invoker.Invoke(strEffectName, vPos, vDir, vUp, &vecResult, &postData);
	if (!vecResult.empty())
	{
		// 이펙트 지울때 필요합니다.
		m_uidEffect = vecResult[0].uidEffect;
	}
}

XSensor::XSensor( unsigned long nSensorID, XSensorInfo* pInfo, GATE_TYPE nGateType ) 
: m_pSensorInfo(pInfo)
, m_nID(nSensorID)
, m_nGateType(nGateType)
, m_uidEffect(MUID::ZERO)
{
	CreateSensorEffect(m_pSensorInfo, m_nGateType);
}

XSensor::~XSensor()
{
	DeleteSensorEffect();
}

bool XSensor::IsEnteredSensor(vec3 vPos)
{
	if (m_pSensorInfo == NULL)					return false;
	if (m_pSensorInfo->strGroupName != L"none")	return false;

	float fDistance = m_pSensorInfo->vPoint.DistanceTo(vPos);
	if (fDistance <= m_pSensorInfo->fRadius)
		return true;

	return false;
}

SENSOR_RESULT XSensor::OnUpdate( bool bEnter, vec3 vPos )
{
	if(IsEnteredSensor(vPos))
	{
		if (!bEnter)
		{
			if (OnEnter(vPos))
			{
				return SENSOR_RESULT_ENTER;
			}
		}
	}
	else
	{
		if (bEnter)
		{
			if (OnLeave(vPos))
			{
				return SENSOR_RESULT_LEAVE;
			}
		}
	}

	return SENSOR_RESULT_NONE;
}

void XSensor::Refresh( GATE_TYPE nGateType )
{
	if (m_nGateType == nGateType) return;

	if (nGateType != TGT_NONE)
	{
		CreateSensorEffect(m_pSensorInfo, nGateType);
	}
	else
	{
		DeleteSensorEffect();
	}

	m_nGateType = nGateType;
}

void XSensor::DeleteSensorEffect()
{
	if (m_uidEffect != MUID::ZERO && global.emgr)
	{
		global.emgr->DeleteXEffect(m_uidEffect, true);
	}

	m_uidEffect = MUID::ZERO;
}

bool XSensor::OnEnter( vec3 vPos )
{
	XPostTriggerTouchSensorEnter(m_nID, vPos);

	return true;
}

bool XSensor::OnLeave(vec3 vPos)
{
	XPostTriggerTouchSensorLeave(m_nID, vPos);

	return true;
}
