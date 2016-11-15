#pragma once

#include "MTime.h"
#include "GGateInfo.h"

class GPlayerField;

class GPlayerFieldGateRetryer : public MTestMemPool<GPlayerFieldGateRetryer>
{
public:
	static const float TIME_TO_UPDATE_SEC;

public:
	GPlayerFieldGateRetryer(GEntityPlayer* pOwner, GPlayerField* pPlayerField);
	~GPlayerFieldGateRetryer();

	bool IsRunRetryer()	{ return m_bIsRunRetry; }

	void Update(float fDelta);
	void StartRetryer(int nFieldID, vec3 pos, vec3 dir);

private:
	GEntityPlayer*	m_pOwner;		///< ¼ÒÀ¯ÀÚ
	GPlayerField*	m_pPlayerField;

	MRegulator		m_rgltRetry;
	bool			m_bIsRunRetry;

	/// GateInfo
	int				m_nDestFieldID;
	vec3			m_vPos;
	vec3			m_vDir;

	void			StopRetryer();
	void			ClearRetryInfo();

};
