#include "stdafx.h"
#include "GPlayerSensorCacheUpdater.h"
#include "GField.h"
#include "GEntityPlayer.h"
#include "GCommand.h"
#include "GFieldSensor.h"
#include "CCommandTable.h"

bool GPlayerSensorCacheUpdater::IsSensorChanged( unsigned int nSensorChecksum )
{
	return (m_nUsableSensorChecksum != nSensorChecksum);
}

GPlayerSensorCacheUpdater::GPlayerSensorCacheUpdater() : m_nUsableSensorChecksum(UINT_MAX)
{

}

GPlayerSensorCacheUpdater::~GPlayerSensorCacheUpdater()
{

}

void GPlayerSensorCacheUpdater::Update( GEntityPlayer* pEntityPlayer, GField* pField, bool bRoute )
{
	//	assert(m_pField);
	if (!pField)					
		return;
	VALID(pField->GetInfo());

	vector<TD_TRIGGER_SENSOR_INFO> vecSensors;
	pField->GetFieldSensor().MakeUsableSensorInfo(pField, pEntityPlayer, vecSensors);

	unsigned int nChecksum = CalcChecksum(vecSensors);

	if (!IsSensorChanged(nChecksum))		
		return;

	m_nUsableSensorChecksum = nChecksum;

	if (bRoute)
	{
		Route(pEntityPlayer, vecSensors);
	}
}

void GPlayerSensorCacheUpdater::Invalidate()
{
	m_nUsableSensorChecksum = UINT_MAX;
}

void GPlayerSensorCacheUpdater::Route( GEntityPlayer* pEntityPlayer, vector<TD_TRIGGER_SENSOR_INFO> &vecSensors )
{
	void* pMarkerPtr=NULL;
	if (!vecSensors.empty())		pMarkerPtr = &vecSensors[0];

	MCommand* pNewCommand = MakeNewCommand(MC_TRIGGER_UPDATE_USABLE_SENSOR, 1, 
		NEW_BLOB(pMarkerPtr, static_cast<unsigned short>(sizeof(TD_TRIGGER_SENSOR_INFO)), static_cast<unsigned short>(vecSensors.size())));

	pEntityPlayer->RouteToMe(pNewCommand);
}

unsigned int GPlayerSensorCacheUpdater::CalcChecksum( vector<TD_TRIGGER_SENSOR_INFO> &vecSensors )
{
	unsigned int nChecksum = 0;

	for (size_t i = 0; i < vecSensors.size(); ++i)
	{
		TD_TRIGGER_SENSOR_INFO& td = vecSensors[i];
		nChecksum += (td.nSensorID + (td.nSensorID * (int)td.nGateType));
	}
	return nChecksum;
}