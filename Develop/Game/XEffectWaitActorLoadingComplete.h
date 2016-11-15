#pragma once

#include "XCommonData.h"

class XEffectManager;

enum WAIT_EFFECT_TYPE 
{
	WET_MODEL,
	WET_SELFILLUMINATION,
	WET_WEAPON,
};

class XEffectWaitActorLoadingComplete
{
private:
	struct stEffectWaitingData
	{
		WAIT_EFFECT_TYPE	eType;
		MUID				uidEffect;

		EFFECT_CREATE_DATA	effectData;

		stEffectWaitingData()
		{
			eType				= WET_MODEL;
			uidEffect			= MUID::ZERO;
		}
	};

private:
	bool						m_bInit;
	XEffectManager *			m_pEffectManager;

	vector<stEffectWaitingData>	m_vecEffectWaiting;

public:
	XEffectWaitActorLoadingComplete(void);
	~XEffectWaitActorLoadingComplete(void);

	void						Init(XEffectManager * pEffectManager);
	void						Destroy();

	void						AddEffectWaiting(MUID uidEffect, EFFECT_CREATE_DATA effectData, WAIT_EFFECT_TYPE eType);
	void						DelEffectWaiting(MUID uidEffect);

	void						ActorLoadingComplete(MUID uidActor);

	bool						CheckWaiting(const MUID& uidEffect);
};
