#ifndef _XACTION_DIE_H
#define _XACTION_DIE_H

enum ACTION_DIE_STATE
{
	ADS_NORMAL,
	ADS_GAMESTART,

	// 모션 펙터 관련
	ADS_KNOCKBACK,
	ADS_KNOCKDOWN,
	ADS_THROWUP,
	ADS_GRAPPLED,
	ADS_UPPERED,
	ADS_MF_NO_ANIMATION,

	ADS_MAX
};

class XActionDie
{
private:
	XObject *			m_pOwner;

	bool				m_bStartDie;					// 죽는 애니메이션 시작
	bool				m_bMFDie;
	bool				m_bDieAniEnd;

	vec3				m_vDieStartUp;					// 시작 djq
	vec3				m_vDieStartDir;					// 시작 방향
	vec3				m_vDieTargetUp;					// 최종 업
	vec3				m_vDieTargetDir;				// 최종 방향

private:
	bool				CalTerrainUpDir(vec3& pos, vec3& dir, vec3& outUp, vec3& outDir);

	void				StartDie(bool bAni);
	void				DieAniEnd();

	void				StartTerrainUpDir();
	void				UpdateTerrainUpDir(float fDelat);

public:
	XActionDie();
	virtual ~XActionDie();

	void				Init(XObject* pObject);
	void				OnStartDie(ACTION_DIE_STATE eState);
	void				OnStartDie(MF_STATE eMFState, bool bAniSet);
	void				OnEndDie();
	void				Update(float fDelta);

	void				DoneDie();

	bool				IsDieAniEnd() { return m_bDieAniEnd; }
	bool				IsDoingActionDie();
};

#endif // _XACTION_DIE_H