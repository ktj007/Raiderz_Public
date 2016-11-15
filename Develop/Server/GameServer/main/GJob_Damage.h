#pragma once

#include "GInstantJob.h"

class GJob_Damage : public GInstantJob
{
private:
	uint32  m_nDamageType;
	uint32  m_nDamage;
	MUID	m_uidOpponent;
protected:
	virtual bool OnStart();
public:
	DECLARE_JOB_ID(GJOB_DAMAGE);

	GJob_Damage(GModuleAI* pmAI, uint32 nDamageType, uint32 nDamage, MUID	uidOpponent);
	virtual ~GJob_Damage();
};