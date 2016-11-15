#ifndef _TMAGIC_AREA_H
#define _TMAGIC_AREA_H

#pragma once

class TMagicArea
{
protected:

	TCharacter*					m_pActor;
	TCharacter*					m_pNPCActor;
	TNPCInfo*					m_pNPCInfo;

	vector<EFFECT_RESULT_DATA>	m_vecEffectData;

	bool						m_bModelLoadingComplete;
	bool						m_bDespawn;

	float						m_fExistTime;

	bool						m_bStart;

	RMatrix						m_matWorld;

private:
	void						AniSpawn();
	void						AniIdle();
	void						AniAttack();
	void						AniDespawn();

	void						DoModelLoadingComplete();

	bool						CheckModelLoadingComplete();
	void						CheckAnimationState(float fDelta);

	void						EffectCreate(XBuffInfo* pBuffInfo, vec3 vCenter);

protected:
	virtual bool				OnCreate();

public:
	TMagicArea();
	virtual ~TMagicArea();

	void						Setup(TCharacter* pActor, CSTalentInfo* pInfo, CSTalentEventInfo * pEvent = NULL);

	void						SetupModel( TTalentInfo* pTalentInfo );

	virtual void				OnUpdate(float fDelta);
	virtual void				OnRender();
	virtual void				OnDestroy();

	void						Despawn();
	void						TrapAction();
};


#endif