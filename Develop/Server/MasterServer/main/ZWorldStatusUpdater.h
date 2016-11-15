#pragma once

class ZDBManager;

class ZWorldStatusUpdater
{
public:
	ZWorldStatusUpdater(ZDBManager* pDBManager);
	virtual ~ZWorldStatusUpdater();

	void Start(void);
	void Update(float fDeltaSecond);
	
	void OnStart(void);
	void OnUpdate(void);

private:
	ZDBManager* m_pDBManager;
	MRegulator m_updateRegulator;
};
