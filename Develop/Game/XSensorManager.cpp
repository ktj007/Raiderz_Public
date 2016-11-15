#include "stdafx.h"
#include "XSensorManager.h"
#include "XSensor.h"
#include "XLocalSensor.h"
#include "XQuestSensor.h"
#include "XSensorController.h"
#include "XFieldInfo.h"

void XSensorManager::Insert( XFieldInfo* pFieldInfo, TD_TRIGGER_SENSOR_INFO tdSensorInfo )
{
	if (pFieldInfo == NULL)	return;
	if (m_SensorMap.find(tdSensorInfo.nSensorID) != m_SensorMap.end())	return;

	int nSensorID = tdSensorInfo.nSensorID;
	XSensorInfo& SensorInfo = pFieldInfo->GetSensorInfo(nSensorID);
	if (SensorInfo.bLocal == true) return;

	GATE_TYPE nGateType = XSensorManager::ConvertGateType(tdSensorInfo.nGateType);
	XSensor* pNewSensor = new XSensor(nSensorID, &SensorInfo, nGateType);
	m_SensorMap.insert(XSensorMap::value_type(tdSensorInfo.nSensorID, pNewSensor));
}

void XSensorManager::InsertSensor(XFieldInfo* pFieldInfo)
{
	SENSOR_INFO_MAP_BY_ID& mapSensorByID = pFieldInfo->m_mapSensorByID;
	for (	SENSOR_INFO_MAP_BY_ID::iterator iter = mapSensorByID.begin(); iter != mapSensorByID.end(); ++iter)
	{
		const unsigned long& nSensorID = iter->first;
		XSensorInfo& SensorInfo = iter->second;
		
		XSensorMap::iterator itor = m_SensorMap.find(nSensorID);
		if (itor != m_SensorMap.end()) continue;

		if (SensorInfo.bAutoParty)
		{
			XQuestInfo* pQuestInfo = info.quest->Get(SensorInfo.nQuestID);
			if (pQuestInfo && pQuestInfo->nPartyFieldID == pFieldInfo->m_nFieldID)
			{
				XQuestSensor* pNewSensor = new XQuestSensor(nSensorID, &SensorInfo, pQuestInfo);
				m_SensorMap.insert(XSensorMap::value_type(nSensorID, pNewSensor));
			}
		}
		else if (SensorInfo.bLocal)
		{
			XLocalSensor* pNewSensor = new XLocalSensor(nSensorID, &SensorInfo, TGT_NONE);
			m_SensorMap.insert(XSensorMap::value_type(nSensorID, pNewSensor));
		}
	}
}

XSensor* XSensorManager::Find( int nSensorID )
{
	XSensorMap::iterator itor = m_SensorMap.find(nSensorID);
	if (itor != m_SensorMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

void XSensorManager::Init( XFieldInfo* pFieldInfo, vector<TD_TRIGGER_SENSOR_INFO>& vecID )
{
	if (pFieldInfo == NULL)	return;

	vector<XSensor*>	vecExistSensors;

	// 이미 있으면 해당 센서는 잠시 빼놓는다.
	if (pFieldInfo && 
		m_pFieldInfo &&
		pFieldInfo->m_nFieldID == m_pFieldInfo->m_nFieldID)
	{
		for (size_t i = 0; i < vecID.size(); i++)
		{
			int nID = vecID[i].nSensorID;
			XSensorMap::iterator itor = m_SensorMap.find(nID);
			if (itor != m_SensorMap.end())
			{
				vecExistSensors.push_back((*itor).second);
				m_SensorMap.erase(itor);
			}
		}
	}
	
	Clear();
	m_pFieldInfo = pFieldInfo;

	// 다시 넣음
	for (size_t i = 0; i < vecExistSensors.size(); i++)
	{
		XSensor* pSensor = vecExistSensors[i];
		m_SensorMap.insert(XSensorMap::value_type(pSensor->GetID(), pSensor));
	}

	// 나머지 추가
	for (vector<TD_TRIGGER_SENSOR_INFO>::iterator itSensor = vecID.begin(); itSensor != vecID.end(); itSensor++)
	{
		TD_TRIGGER_SENSOR_INFO& tdInfo = (*itSensor);

		int nSensorID = tdInfo.nSensorID;

		XSensorMap::iterator itor = m_SensorMap.find(nSensorID);
		if (itor == m_SensorMap.end())
		{
			Insert(pFieldInfo, (*itSensor));
		}
		else
		{
			GATE_TYPE nGateType = ConvertGateType(tdInfo.nGateType);
			XSensor* pSensor = (*itor).second;
			pSensor->Refresh(nGateType);
		}
	}

	// 센서 등록
	InsertSensor(m_pFieldInfo);
}


void XSensorManager::Clear()
{
	for (XSensorMap::iterator itor = m_SensorMap.begin(); itor != m_SensorMap.end(); ++itor)
	{
		XSensor* pSensor = (*itor).second;
		delete pSensor;
	}
	m_SensorMap.clear();

	m_pFieldInfo = NULL;
}

XSensorManager::XSensorManager() : m_pFieldInfo(NULL)
{
	m_pController = new XSensorController(this);
}

XSensorManager::~XSensorManager()
{
	Clear();
	delete m_pController;
}

GATE_TYPE XSensorManager::ConvertGateType( TD_SENSOR_GATE_TYPE tdGateType )
{
	GATE_TYPE nGateType = TGT_NONE;
	if (tdGateType == SENSOR_GATE_TYPE_AUTO) nGateType = TGT_AUTO;
	else if (tdGateType == SENSOR_GATE_TYPE_MANUAL) nGateType = TGT_MANUAL;

	return nGateType;
}

