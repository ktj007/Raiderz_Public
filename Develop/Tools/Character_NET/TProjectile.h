#ifndef _TPROJECTILE_H
#define _TPROJECTILE_H

#pragma once

struct EFFECT_RESULT_DATA;

class TProjectile
{
private:
	TCharacter*		m_pActor;
	CSProjectileInfo*	m_pProjectileInfo;
	CSTalentInfo*	m_pTalentInfo;

	vec3			m_vPos;
	vec3			m_vDir;
	vec3			m_vVelocity;
	vec3			m_vOldPos;

	vec3			m_vStartPos;
	vec3			m_vTargetPos;
	vec3			m_vCalParabola;						// 포물선 계산용 좌표

	vector<EFFECT_RESULT_DATA>	m_EffectDelStorage;

	float			m_fElapsedTime;						// 시간
	float			m_fAutoDelTime;						// 자동 소멸 시간
	float			m_fEstimateTime;					// 명중까지 남은 시간 예측

	bool			m_bCol;

private:
	vec3			GetProjectileStartPos();

	void			SetTargetByProjectileType();

	void			UpdateMoving(float fDelta);
	bool			UpdateCol(vec3& old_pos, vec3& pos);
	void			UpdateEffect();
	
	// 이동 패턴
	void			Move_Straight(float fDelta, vec3 * vTargetPos = NULL);		// 직선(중력 영향 없음)
	void			Move_Curve(float fDelta, vec3 * vTargetPos = NULL);			// 유도(곡선률 없음)
	void			Move_Parabola(float fDelat, vec3 * vTargetPos = NULL);		// 포물선

	bool			CheckGPS();

	void			HitEffect();

public:
	TProjectile();
	virtual ~TProjectile();

	bool			LaunchProjectile(CSProjectileInfo& csProjectilInfo, TCharacter* pActor, CSTalentInfo* pInfo);
	void			Update(float fDelta);
	void			DelProjectile();

	bool			IsCol() { return m_bCol; }
};


#endif