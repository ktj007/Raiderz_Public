#pragma once

#include "CTransData.h"

class GField;
class GEntityPlayer;

class GPlayerSensorCacheUpdater : public MTestMemPool<GPlayerSensorCacheUpdater>
{
private:
	unsigned long m_nUsableSensorChecksum;
	bool IsSensorChanged( unsigned int nSensorChecksum );
	void Route( GEntityPlayer* pEntityPlayer, vector<TD_TRIGGER_SENSOR_INFO> &vecSensors );
	unsigned int CalcChecksum( vector<TD_TRIGGER_SENSOR_INFO> &vecSensors );
public:
	GPlayerSensorCacheUpdater();
	~GPlayerSensorCacheUpdater();
	void Update(GEntityPlayer* pEntityPlayer, GField* pField, bool bRoute);
	void Invalidate();

};
