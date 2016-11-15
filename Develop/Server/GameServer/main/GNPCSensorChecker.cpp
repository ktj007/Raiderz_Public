#include "StdAfx.h"
#include "GNPCSensorChecker.h"
#include "CSTriggerInfo.h"
#include "GEntityNPC.h"

GNPCSensorChecker::GNPCSensorChecker(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
	m_rgrTick.SetTickTime(1.0f);
	m_rgrTick.Start();
}

GNPCSensorChecker::~GNPCSensorChecker(void)
{

}

void GNPCSensorChecker::Update(float fDelta)
{
	if (m_mapSersorInfo.empty()) return;

	if (!m_rgrTick.IsReady(fDelta))
		return; // 주기가 되지 않음

	vector<const SENSOR_INFO*> vecDeleteSensorInfo;

	for each (const MAP_SENSORINFO::value_type& val in m_mapSersorInfo)
	{
		const SENSOR_INFO* pSensorInfo = val.first;
		bool bAutoDelete = val.second;		

		if (pSensorInfo->nID == m_pOwner->GetForcusedSensorID())
		{
			if (pSensorInfo->fRadius < pSensorInfo->vPoint.DistanceTo(m_pOwner->GetPos()))
			{
				m_pOwner->LeaveSensor(pSensorInfo->nID);				
			}			
		}
		else
		{
			if (pSensorInfo->fRadius >= pSensorInfo->vPoint.DistanceTo(m_pOwner->GetPos()))
			{
				m_pOwner->EnterSensor(pSensorInfo->nID);

				if (true == bAutoDelete)
				{
					vecDeleteSensorInfo.push_back(pSensorInfo);					
				}
			}
		}
	}

	for each (const SENSOR_INFO* pDeleteSensorInfo in vecDeleteSensorInfo)
	{
		Disable(pDeleteSensorInfo);
	}
}

void GNPCSensorChecker::Enable(const SENSOR_INFO* pSensorInfo, bool bAutoDisable)
{
	MAP_SENSORINFO::iterator itor = m_mapSersorInfo.find(pSensorInfo);
	if (m_mapSersorInfo.end() != itor)
	{
		bool bBeforeAutoDelete = itor->second;

		if (true == bBeforeAutoDelete && false == bAutoDisable)
		{
			itor->second = bAutoDisable;
		}

		return;
	}	

	m_mapSersorInfo.insert(MAP_SENSORINFO::value_type(pSensorInfo, bAutoDisable));
}

void GNPCSensorChecker::Disable(const SENSOR_INFO* pSensorInfo)
{
	m_mapSersorInfo.erase(pSensorInfo);
}

bool GNPCSensorChecker::IsEnable(const  SENSOR_INFO* pSensorInfo )
{
	 if (m_mapSersorInfo.end() == m_mapSersorInfo.find(pSensorInfo)) return false;

	 return true;
}