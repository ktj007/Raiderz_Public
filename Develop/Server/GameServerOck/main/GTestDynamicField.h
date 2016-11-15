#pragma once


class GEntityPlayer;

class GTestDynamicField
{
public:
	GTestDynamicField(void);
	~GTestDynamicField(void);

	static GTestDynamicField& Instance();
		
	void Update(float fDelta);

private:
	void Init();
	void Fini();
	void CreateDF();

private:
	bool m_bInit;
	MRegulator m_rgrSensorTick;
	MRegulator m_rgrFieldTick;
	vector<GEntityPlayer*> m_vecPlayers;
	int m_nCurSensorID;
	int m_nCurFieldQty;
};
