#ifndef _XPROJECTILE_H
#define _XPROJECTILE_H

#include "XObject.h"
#include "XDef.h"
#include "XEffect.h"
#include "XProjectileEffect.h"

#define AUTO_DELETE_TIME			30.0f

class XProjectile;
class XTalentInfo;
class CSProjectileInfo;
class XTalentEffectDataMgr;
class XTalentEffectInfo;
enum TALENT_EFFECT_TYPE;

using namespace rs3;

enum ProjectileColJudgmentType
{
	PCJT_NONE,
	PCJT_HIT,
	PCJT_DEFENCE
};

class XProjectile : public XObject, public MMemPool<XProjectile>
{
	MDeclareRTTI;
private:
	struct stColJedgment
	{
		ProjectileColJudgmentType	eType;
		XObject*					pTarget;

		stColJedgment(ProjectileColJudgmentType e, XObject* p)
		{
			eType				= e;
			pTarget				= p;
		}
	};

private:
	XModuleEntity*				m_pModuleEntity;
	XProjectileEffect			m_Effect;
	XTalentInfo*				m_pTalentInfo;

	CSProjectileInfo*			m_pProjectileInfo;

	// 타겟팅 방식
	MUID						m_uidAttaker;						// 발사자
	MUID						m_uidTarget;						// 타겟
	vec3						m_vTargetPos;						// 타겟 좌표
	
	vec3						m_vPos;								// 현 좌표
	vec3						m_vOldPos;							// 이전 좌표
	vec3						m_vDir;								// 현 방향
	vec3						m_vVelocity;						// 속력

	// 계산에 필요한 좌표
	vec3						m_vStartPos;						// 시작 좌표
	vec3						m_vCalParabola;						// 포물선 계산용 좌표

	float						m_fElapsedTime;						// 시간
	float						m_fAutoDelTime;						// 자동 소멸 시간
	float						m_fEstimateTime;					// 명중까지 남은 시간 예측

	bool						m_bColCheck;						// 충돌 체크?
	bool						m_bDelMe;							// 삭제?
	bool						m_bWaitColJudgment;					// 충돌에 대한 판단
	bool						m_bColJudgment;

	int							m_nTargetGroupID;					// 타겟 그룹 ID
	int							m_nTargetCapsuleID;					// 타겟 더미 ID
	ENTITY_TYPEID				m_eAttackerTypeID;					// 공격자 타입 ID

	// 충돌에 대한 정보
	XObject*					m_pColObject;						// 충돌한 오브젝트
	vec3						m_vCoPos;							// 충돌 위치...
	vec3						m_vColDir;
	ProjectileColJudgmentType	m_ColJudgmentType;

	vector<stColJedgment>		m_vecColJedgment;

protected:
	void						Destroy();

	void						SetAtt(bool bCol, float fAutoDelTime = AUTO_DELETE_TIME);
	void						SetTargetByProjectileType( MMatrix matAttacker );

	bool						CheckTimeOut();
	bool						CheckGPS();
	bool						CheckRange();
	bool						CheckColTerrain(vec3& old_pos, vec3& pos);	///< 지형 충돌 체크
	bool						CheckColObj(vec3& old_pos, vec3& pos);		///< 오브젝트 충돌 체크
	bool						CheckColBase(vec3& old_pos, vec3& pos, XObject * pObject, float & out_Distance, vec3& out_colPos, bool bDirCheck = false);

	void						UpdateMoving( float fDelta );
	bool						UpdateCol(float fDelta);
	void						UpdateColJedgment();

	vec3						GetTargetPos(MUID uidTarget, int nGroupID = -1, int nDummyID = -1);

	// 발사체 타입
	void						Projectile_Missile(float fDelta);
	void						Projectile_Guided(float fDelta);
	void						Projectile_HitCapsule(float fDelta);

	// 이동 패턴
	void						Move_Straight(float fDelta, vec3 * vTargetPos = NULL);		// 직선(중력 영향 없음)
	void						Move_Curve(float fDelta, vec3 * vTargetPos = NULL);			// 유도(곡선률 없음)
	void						Move_Parabola(float fDelat, vec3 * vTargetPos = NULL);		// 포물선

	void						SetMoveData(vec3 vPos);				// 이동 데이터 입력

	void						ShowColEffect(XObject* pColObject, ProjectileColJudgmentType eType, vec3 vColPos, vec3 vColDir);
	void						CheckColJedgment();
	void						ProjectileColJedgment_Missile(ProjectileColJudgmentType eType, XObject* pTarget);

	virtual void				RegisterModules();
	virtual void				UnregisterModules();
	virtual void				OnUpdate(float fDelta);
	virtual bool				OnCreate();
	virtual void				OnDestroy();
	virtual void				OnRender();
public:
	XProjectile(MUID uid);
	virtual ~XProjectile();

	virtual XObjectID				GetType() { return XOBJ_PROJECTILE; }
	virtual bool		Reload();

	// 데이터 입력
	void						Setup(MUID& uidPlayer
										, XTalentInfo* pInfo
										, CSProjectileInfo* pProjectileInfo
										, vec3& vStartPos
										, vec3& velocity
										, vec3 vAttackerStartPos = vec3::ZERO
										, MUID uidTarget = MUID::Invalid()
										, int nGroupID = -1
										, int nCapsuleID = -1);

	void						SetIndex(int nIdx, int nMaxIdx);	// 동시에 여러개 쐈을때 몇번째 놈이냐 - 초기방향 설정

	bool						IsEqualProjectile(MUID uidAttacker, int nTalentID);
	bool						IsWaitColJudgment()		{ return m_bWaitColJudgment; }

	XObject *					GetColObject()			{ return m_pColObject; }
	XTalentInfo*				GetTalentInfo()			{ return m_pTalentInfo; }

	void						ProjectileColResult(ProjectileColJudgmentType eType, XObject* pTarget);
};

#endif