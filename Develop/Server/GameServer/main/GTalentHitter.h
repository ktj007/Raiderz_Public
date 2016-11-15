#pragma once

#include "GTalentInfo.h"

class GTalentEffector;
class GEntityActor;
class GField;

enum TARGEHITTER_TYPE
{
	THT_INVALID = 0,
	THT_PROJECTILE,
	THT_TIMEDELAY,
	THT_INSTANT,
	THT_MAX_COUNTER,
};

//////////////////////////////////////////////////////////////////////////
//
// GTalentHitter
//
//////////////////////////////////////////////////////////////////////////

class GTalentHitter
{
public:
	static const int TALENT_HITTER_TIMEOUT = 30;
protected:
	GTalentInfo*			m_pTalentInfo;
	float					m_fElapsedTime;

	// ret true: Ãæµ¹µÊ false: Ãæµ¹¾ÈµÊ
	bool						CheckTimeout();
public:
	GTalentHitter() : m_fElapsedTime(0.0f), m_pTalentInfo(NULL) {}
	virtual ~GTalentHitter() {}
	virtual bool Update(float fDelta, GField* pField);
	template< typename T_TalentHitter >
	inline static T_TalentHitter* NewTalentHitter()
	{
		return new T_TalentHitter();
	}

	virtual TARGEHITTER_TYPE GetType() { return THT_INVALID; }
};

//////////////////////////////////////////////////////////////////////////
//
// GTalentHitter_Projectile
//
//////////////////////////////////////////////////////////////////////////

class GTalentHitter_Projectile : public GTalentHitter, public MTestMemPool<GTalentHitter_Projectile>
{
private:
	MUID						m_uidOwner;
	CSProjectileInfo*			m_pProjectileInfo;
	vec3						m_vPos;
	vec3						m_vDir;
	vec3						m_vStartPos;
	vec3						m_vVelocity;
	vec3						m_vOldPos;
	vector<MUID>				m_vecEnemyList;
	GField*						m_pField;
	bool						m_bObjectHit;
private:
	void MakeEnemyList(GField* pField);
	void AddStress_AvoidRangeAttack(GField* pField);
	bool CheckObjects( GField* pField, GEntityActor* pOwner );
	bool CheckTerrain( GField* pField) /* ret true: Ãæµ¹µÊ false: Ãæµ¹¾ÈµÊ */;
	bool CheckRange( float fTalentProjectileRange );
	void SetMoveData( vec3 vPos );
	void Move( float fElapsedTime );
	void DebugTraceProjectileHitCapsule( MCapsule &capProjectile, GEntityActor* pTar, MUID &uidOwner );
public:
	GTalentHitter_Projectile();
	virtual ~GTalentHitter_Projectile();

	void Setup( GField* pField, MUID uidOwner, vec3& vStartPos, vec3& vVelocity, GTalentInfo* pTalentInfo, CSProjectileInfo* pProjectileInfo );
	bool CheckProjectileMissile(GField* pField);
	const vector<MUID> GetEnemtyList()		{ return m_vecEnemyList; }
	
	virtual bool Update(float fDelta, GField* pField) override;
	virtual TARGEHITTER_TYPE GetType() { return THT_PROJECTILE; }
};

//////////////////////////////////////////////////////////////////////////
//
// GTimeDelayHitChecker
//
//////////////////////////////////////////////////////////////////////////

class GTimeDelayHitChecker
{
private:
	MUID						m_uidTarget;
	float						m_fEstimateTime;
	int							m_nCapsuleGroup;
	int							m_nCapsuleIndex;
public:
	GTimeDelayHitChecker() : m_uidTarget(0), m_fEstimateTime(0.0f), m_nCapsuleGroup(INVALID_ID), m_nCapsuleIndex(INVALID_ID) {}
	void Setup(float fEstimateTime, MUID uidTarget, int nCapsuleGroup=INVALID_ID, int nCapsuleIndex=INVALID_ID);
	bool CheckTime(float fElapsedTime, GEntityActor* pOwner, GTalentInfo* pTalentInfo);
	float GetEstimateTime()	{ return m_fEstimateTime; }
};

//////////////////////////////////////////////////////////////////////////
//
// GTalentHitter_TimeDelay
//
//////////////////////////////////////////////////////////////////////////

class GTalentHitter_TimeDelay : public GTalentHitter, public MTestMemPool<GTalentHitter_TimeDelay>
{
private:
	GTimeDelayHitChecker		m_TimeDelayHitChecker;
	MUID						m_uidOwner;
	MUID						m_uidTarget;
	const GTalentEffector*		m_pTalentEffector;
public:
	GTalentHitter_TimeDelay() : GTalentHitter(), m_pTalentEffector(NULL)	{}
	void Setup( MUID uidOwner, MUID uidTarget, int nCapsuleGroup, int nCapsuleIndex, GTalentInfo* pTalentInfo, float fTimeDelay, const GTalentEffector& TalentEffector );
	virtual bool Update(float fDelta, GField* pField) override;
	virtual TARGEHITTER_TYPE GetType() { return THT_TIMEDELAY; }
	GTimeDelayHitChecker		GetDelayHitChecker() {return m_TimeDelayHitChecker;}
};

//////////////////////////////////////////////////////////////////////////
//
// GTalentHitter_Instant
//
//////////////////////////////////////////////////////////////////////////

class GTalentHitter_Instant : public GTalentHitter, public MTestMemPool<GTalentHitter_Instant>
{
private:
	MUID						m_uidOwner;
	MUID						m_uidTarget;
	uint16						m_nCheckTime;
	int							m_nCapsuleGroup;
	int							m_nCapsuleIndex;
public:
	GTalentHitter_Instant() : GTalentHitter(), m_nCheckTime( 0), m_nCapsuleGroup(INVALID_ID), m_nCapsuleIndex(INVALID_ID)	{}
	void Setup( MUID uidOwner, GTalentInfo* pTalentInfo, GEntityActor* pTarget, uint16 nCheckTime, int nCapsuleGroup, int nCapsuleIndex );
	virtual bool Update(float fDelta, GField* pField) override;
	virtual TARGEHITTER_TYPE GetType() { return THT_INSTANT; }
};

//////////////////////////////////////////////////////////////////////////
//
// GTalentHitterList
//
//////////////////////////////////////////////////////////////////////////

class GTalentHitterList
{
private:
	list<GTalentHitter*>			m_HitterList;
public:
	GTalentHitterList() {}
	~GTalentHitterList();

	void Update(float fDelta, GField* pField);
	void Add(GTalentHitter* pNewHitter);
	void Clear();
	vector<GTalentHitter*>	 Get(TARGEHITTER_TYPE nType);
};

