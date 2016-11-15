#include "stdafx.h"
#include "GTalentHitter.h"
#include "GEntityActor.h"
#include "GModuleAI.h"
#include "GEntityNPC.h"
#include "GConst.h"
#include "GEffectTargetSelector.h"
#include "GField.h"
#include "GCollision.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GRelationChecker.h"
#include "GNPCStress.h"

//////////////////////////////////////////////////////////////////////////
//
// GTalentHitter
//
//////////////////////////////////////////////////////////////////////////

bool GTalentHitter::CheckTimeout()
{
	return (m_fElapsedTime >= (float)TALENT_HITTER_TIMEOUT);
}

bool GTalentHitter::Update( float fDelta, GField* pField )
{
	m_fElapsedTime += fDelta;

	return (CheckTimeout());
}

//////////////////////////////////////////////////////////////////////////
//
// GTimeDelayHitChecker
//
//////////////////////////////////////////////////////////////////////////

void GTimeDelayHitChecker::Setup( float fEstimateTime, MUID uidTarget, int nCapsuleGroup/*=INVALID_ID*/, int nCapsuleIndex/*=INVALID_ID*/ )
{
	m_uidTarget = uidTarget;
	m_fEstimateTime = fEstimateTime;
	m_nCapsuleGroup = nCapsuleGroup;
	m_nCapsuleIndex = nCapsuleIndex;
}

bool GTimeDelayHitChecker::CheckTime( float fElapsedTime, GEntityActor* pOwner, GTalentInfo* pTalentInfo )
{
	if (m_uidTarget == 0) return false;
	GField* pField = pOwner->GetField();

	GEntityActor* pTarget = pField->FindActor(m_uidTarget);
	if (pTarget == NULL) return false;
	if (fElapsedTime < m_fEstimateTime) return false;

	GEffectTargetSelector effecteeTargetSelector;
	GEffectTargetSelector::Desc desc(pTalentInfo->m_EffectInfo);
	desc.pSelf = pOwner;
	desc.pTarget = pTarget;
	set<GEntityActor*> qEffecteeList;
	effecteeTargetSelector.Select(desc, qEffecteeList);

	for each (GEntityActor* pActor in qEffecteeList)
		pActor->doTryHit(pOwner, pTalentInfo, 0, 0, m_nCapsuleGroup, m_nCapsuleIndex);

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
// GTalentHitter_Projectile
//
//////////////////////////////////////////////////////////////////////////

GTalentHitter_Projectile::GTalentHitter_Projectile() 
: GTalentHitter()
, m_pProjectileInfo(NULL)
, m_pField(NULL)
, m_bObjectHit(false)
{
}

GTalentHitter_Projectile::~GTalentHitter_Projectile()
{
	if (m_bObjectHit)
	{
		AddStress_AvoidRangeAttack(m_pField);
	}
}

void GTalentHitter_Projectile::Setup( GField* pField, MUID uidOwner, vec3& vStartPos, vec3& vVelocity, GTalentInfo* pTalentInfo, CSProjectileInfo* pProjectileInfo )
{
	DCHECK(pField);
	DCHECK(pTalentInfo);
	DCHECK(pProjectileInfo);
	
	m_pTalentInfo = pTalentInfo;
	m_uidOwner = uidOwner;
	m_vStartPos = vStartPos;
	m_vPos = vStartPos;
	m_vOldPos = m_vPos;
	m_vVelocity = vVelocity;
	m_vDir = vVelocity;
	m_vDir.Normalize();
	m_pProjectileInfo = pProjectileInfo;
	m_pField = pField;
	m_bObjectHit = false;

	MakeEnemyList(pField);
}

void GTalentHitter_Projectile::MakeEnemyList(GField* pField)
{
	VALID(pField);
	m_vecEnemyList.clear();

	GEntityActor* const pOwner = pField->FindActor(m_uidOwner);
	if (!pOwner)
		return;	// 발사한 액터가 무효함

	GFieldGrid::EntitySelector* grid_selector = 
		pField->GetGrid().GetEntitySelector();

	class EnemyMaker : public GFieldGrid::Sector::IVisitor
	{
	public:
		EnemyMaker(GField& field,GEntityActor& owner,vector<MUID>& enemies)
			: m_field(field)
			, m_owner(owner)
			, m_enemies(enemies) {}

	private:
		void OnVisit(const MUID& uid)
		{
			GEntityActor* const pTarget = m_field.FindActor(uid);
			if (NULL == pTarget) 
				return;	// 유효하지 않은 목표

			static GRelationChecker checker;
			if (false == checker.IsEnemy(&m_owner, pTarget))
				return;	// 적이 아님

			m_enemies.push_back(uid);
		}

	private:
		GField&			m_field;
		GEntityActor&	m_owner;
		vector<MUID>&	m_enemies;
	};

	grid_selector->VisitByPos(EnemyMaker(*pField, *pOwner, m_vecEnemyList), ETID_ACTOR, pOwner->GetPos(), 1);
}

bool GTalentHitter_Projectile::CheckProjectileMissile( GField* pField )
{
	Move(m_fElapsedTime);

	GEntityActor* pOwner = pField->FindActor(m_uidOwner);
	if (!pOwner)
		return false; // 발사한 액터가 무효함

	if (CheckObjects(pField, pOwner))
		return true;

	if (CheckTerrain(pField)) 
		return true;

	if (CheckRange(m_pTalentInfo->m_fRange)) 
		return true;

	return false;
}

bool GTalentHitter_Projectile::Update( float fDelta, GField* pField )
{
	PFI_BLOCK_THISFUNC(9025);

	// 만료 검사
	if (__super::Update(fDelta, pField))
		return true;

	// 충돌 검사
	if (CheckProjectileMissile(pField))
		return true;

	return false;
}

void GTalentHitter_Projectile::AddStress_AvoidRangeAttack(GField* pField)
{
	GEntityNPC* pOwnerNPC = pField->FindNPC(m_uidOwner);
	if (!pOwnerNPC)
		return;

	pOwnerNPC->GetNPCStress().AddStress(GConst::STRESS_ADD_AVOID_RANGE_ATTACK);
}

bool GTalentHitter_Projectile::CheckObjects( GField* pField, GEntityActor* pOwner )
{
	float	foutDistance = 0.0f;
	float	fGapToNewPos = m_vOldPos.DistanceTo(m_vPos);
	MCapsule capProjectile = MCapsule(m_vPos, m_vOldPos, m_pProjectileInfo->m_fHitRadius);

	for each (const MUID& uidEnemy in m_vecEnemyList)
	{
		GEntityActor* pEnemy = pField->FindActor(uidEnemy);
		if (!pEnemy)		
			continue;

		int8 nHitCapsuleGroup;
		int8 nHitCapsuleindex;
		if (!GCollision::HitTest2(pEnemy, capProjectile, nHitCapsuleGroup, nHitCapsuleindex))
			continue;

		GEffectTargetSelector effecteeTargetSelector;
		GEffectTargetSelector::Desc desc(m_pTalentInfo->m_EffectInfo);
		desc.pSelf = pOwner;
		desc.pTarget = pEnemy;
		set<GEntityActor*> qEffecteeList;
		effecteeTargetSelector.Select(desc, qEffecteeList);

		for each (GEntityActor* pActor in qEffecteeList)
		{
			pActor->doTryHit(pOwner, m_pTalentInfo, 0, 0, nHitCapsuleGroup, nHitCapsuleindex);
		}

		m_bObjectHit = true;
		return true;
	}

	return false;
}


bool GTalentHitter_Projectile::CheckTerrain( GField* pField) 
{
	return pField->Pick(m_vOldPos, m_vPos);
}

bool GTalentHitter_Projectile::CheckRange( float fTalentProjectileRange )
{
	// 사거리 체크
	vec3 vMove = m_vPos - m_vStartPos;
	float fRange = vMove.Length();

	return (fRange >= fTalentProjectileRange);
}

void GTalentHitter_Projectile::SetMoveData( vec3 vPos )
{
	vec3 vOldPos		= m_vPos;
	vec3 vDir			= vPos - vOldPos;
	vDir.Normalize();

	// 결과 좌표 입력
	m_vPos				= vPos;
	m_vOldPos			= vOldPos;
	m_vDir				= vDir;
}

void GTalentHitter_Projectile::Move( float fElapsedTime )
{
	// 직선 운동(중력 없음)
	vec3 vPos = CSTalentInfoHelper::MoveStraight(m_vStartPos, m_vVelocity, fElapsedTime);
	SetMoveData(vPos);
}


void GTalentHitter_Projectile::DebugTraceProjectileHitCapsule( MCapsule &capProjectile, GEntityActor* pTar, MUID &uidOwner )
{
	if (IsRunForTest())
	{
		std::vector<TD_CAPSULE> vecCapsules;
		TD_CAPSULE capsule;
		capsule.bottom = capProjectile.bottom;
		capsule.top = capProjectile.top;
		capsule.height = capProjectile.height;
		capsule.radius = capProjectile.radius;
		vecCapsules.push_back(capsule);

		GActorMeshInfo* pMeshInfo = pTar->GetMeshInfo();
		if (pMeshInfo)
		{
			MCapsule thisCapsule = pMeshInfo->GetSimpleCapsule();
			thisCapsule.bottom += pTar->GetPos();
			thisCapsule.top += pTar->GetPos();

			capsule.bottom = thisCapsule.bottom;
			capsule.top = thisCapsule.top;
			capsule.height = thisCapsule.height;
			capsule.radius = thisCapsule.radius;
			vecCapsules.push_back(capsule);
		}


		wchar_t buff[128];
		swprintf_s(buff, 128, L"%u", uidOwner.High);
		MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, 4, 
			NEW_WSTR(L"TraceProjectileHitCapsule"), 
			NEW_INT(uidOwner.Low), 
			NEW_WSTR(buff), 
			NEW_BLOB(&vecCapsules[0], sizeof(TD_CAPSULE), static_cast<int>(vecCapsules.size())));
		pTar->RouteToField(pNewCommand);
	}
}



//////////////////////////////////////////////////////////////////////////
//
// GTalentHitter_TimeDelay
//
//////////////////////////////////////////////////////////////////////////


void GTalentHitter_TimeDelay::Setup( MUID uidOwner, MUID uidTarget, int nCapsuleGroup, int nCapsuleIndex, GTalentInfo* pTalentInfo, float fTimeDelay, const GTalentEffector& TalentEffector )
{
	m_uidOwner = uidOwner;
	m_uidTarget = uidTarget;
	m_pTalentInfo = pTalentInfo;
	m_pTalentEffector = &TalentEffector;
	m_TimeDelayHitChecker.Setup(fTimeDelay, uidTarget, nCapsuleGroup, nCapsuleIndex);
}

bool GTalentHitter_TimeDelay::Update( float fDelta, GField* pField )
{
	PFI_BLOCK_THISFUNC(9026);

	if (GTalentHitter::Update(fDelta, pField) == true) return true;

	GEntityActor* pOwner = pField->FindActor(m_uidOwner);
	if (pOwner == NULL) return true;

	if (m_TimeDelayHitChecker.CheckTime(m_fElapsedTime, pOwner, m_pTalentInfo) == true)
	{
		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
//
// GTalentHitter_Instant
//
//////////////////////////////////////////////////////////////////////////

void GTalentHitter_Instant::Setup( MUID uidOwner, GTalentInfo* pTalentInfo, GEntityActor* pTarget, uint16 nCheckTime, int nCapsuleGroup, int nCapsuleIndex )
{
	_ASSERT(pTalentInfo);

	m_uidOwner = uidOwner;
	m_uidTarget = pTarget->GetUID();
	m_pTalentInfo = pTalentInfo;
	m_nCheckTime = nCheckTime;
	m_nCapsuleGroup = nCapsuleGroup;
	m_nCapsuleIndex = nCapsuleIndex;
}

bool GTalentHitter_Instant::Update( float fDelta, GField* pField )
{
	PFI_BLOCK_THISFUNC(9027);

	_ASSERT(m_pTalentInfo);
	if (!m_pTalentInfo)	return true;

	GEntityActor* pOwner = pField->FindActor(m_uidOwner);
	if (!pOwner) return true;

	GEntityActor* pTarget = pField->FindActor(m_uidTarget);
	if (!pTarget) return true;

	pTarget->doTryHit(pOwner, m_pTalentInfo, 0, m_nCheckTime, m_nCapsuleGroup, m_nCapsuleIndex);

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
// GTalentHitterList
//
//////////////////////////////////////////////////////////////////////////

GTalentHitterList::~GTalentHitterList()
{
	Clear();
}

void GTalentHitterList::Update( float fDelta, GField* pField )
{
	PFI_BLOCK_THISFUNC(9007);

	for (list<GTalentHitter*>::iterator itor = m_HitterList.begin(); itor != m_HitterList.end(); )
	{
		GTalentHitter* pHitter = (*itor);
		if (pHitter->Update(fDelta, pField))
		{
			PFI_B(9019, "GTalentHitterList::SAFE_DELETE");
			itor = m_HitterList.erase(itor);
			SAFE_DELETE(pHitter);
			PFI_E(9019);
		}
		else
		{
			++itor;
		}
	}
}

void GTalentHitterList::Add( GTalentHitter* pNewHitter )
{
	m_HitterList.push_back(pNewHitter);
}

void GTalentHitterList::Clear()
{
	for (list<GTalentHitter*>::iterator itor = m_HitterList.begin(); itor != m_HitterList.end(); ++itor)
	{
		GTalentHitter* pHitter = (*itor);
		SAFE_DELETE(pHitter);
	}
	m_HitterList.clear();
}

vector<GTalentHitter*> GTalentHitterList::Get( TARGEHITTER_TYPE nType )
{
	vector<GTalentHitter*> vecRet;
	for each (GTalentHitter* each in m_HitterList)
	{
		if (each->GetType() == nType)
			vecRet.push_back(each);
	}
	return vecRet;
}