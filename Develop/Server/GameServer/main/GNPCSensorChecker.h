#pragma once

struct SENSOR_INFO;
class GEntityNPC;

typedef map<const SENSOR_INFO*, bool> MAP_SENSORINFO;

class GNPCSensorChecker : public MTestMemPool<GNPCSensorChecker>
{
private:
	GEntityNPC*			m_pOwner;
	MAP_SENSORINFO		m_mapSersorInfo;
	MRegulator			m_rgrTick;

public:
	GNPCSensorChecker(GEntityNPC* pOwner);
	~GNPCSensorChecker(void);

	void Update(float fDelta);

	void Enable(const SENSOR_INFO* pSensorInfo, bool bAutoDisable = true);
	void Disable(const SENSOR_INFO* pSensorInfo);

	bool IsEnable(const SENSOR_INFO* pSensorInfo);
};
