#pragma once

#include "XModuleActorControl.h"

#define GRAPPLED_COLLISION_PLAYER_ANIMATION		L"mf_grappled_collision"
#define GRAPPLED_DOWN_PLAYER_ANIMATION			L"mf_stand_down"
#define GRAPPLED_UP_PLAYER_ANIMATION			L"mf_stand_up"

#define GRAPPLED_COLLISION_BASE_EFFECT_NAME		L"dirt_shock_nc_s"
#define GRAPPLED_COLLISION_BASE_EFFECT_BONENAME	L"Bip01"

#define GRAPPLED_PLAYER_BONENAME				L"Bip01 Spine1"

class XGrappledController
{
protected:
	XPlayer*				m_pPlayer;
	_DamageRepeatInfo*		m_pHitInfo;
	bool					m_bStart;
	bool					m_bAttach;
	bool					m_bBeThrown;
	bool					m_bCollision;
	bool					m_bFinished;

	float					m_fRemainStartTime;

	wstring					m_strGrappledPlayerAnimation;
	wstring					m_strGrappledPlayerBoneName;

private:
	void					Die();

	float					GetGrappledHitTime(XObject* pAttacker);
	bool					IsValidAttacker(XPlayer* pAttacker);

public:
	XGrappledController();
	virtual ~XGrappledController();


	void					SetOwnerPlayer( XPlayer* pPlayer);

	virtual bool 			OnStart( _DamageRepeatInfo * pHitInfo);
	virtual void 			OnFinish();
	virtual void 			OnUpdate( float fDelta);
	virtual void 			OnPostUpdate( float fDelta);
	virtual void 			OnBeThrown();

	bool					IsFinish();



protected:
	vec3 					_GetSpineDummyPos(wstring strBoneName);

	void 					_UpdateDamage( float fDelta);
	void 					_ShowDamage( int nDamage);
	void 					_ShowEffect( wstring strEffectName, wstring strBoneName);

	void					SetGrappledAnimation();
};

