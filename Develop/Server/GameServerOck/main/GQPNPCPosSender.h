#pragma once

class MRegulator;
class GEntityNPC;

class GQPNPCPosSender : public MTestMemPool<GQPNPCPosSender>
{
public:
	GQPNPCPosSender(GEntityNPC* pNPC);
	~GQPNPCPosSender(void);

	void OnDead();
	void OnDestroy();

	void Enable();
	void Disable();
	void Update(float fDelta);

private:
	void SendMarkPos();
	void SendUnmarkPos();

private:
	bool		m_bEnable;
	GEntityNPC*	m_pOwner;
	MRegulator*	m_pRegular;	
};
