#include "stdafx.h"
#include "XProjectile.h"
#include "XGame.h"
#include "XModuleCollision.h"
#include "XModuleEntity.h"
#include "XModuleTalent.h"
#include "XModuleActorControl.h"
#include "XModuleEffect.h"
#include "XWorld.h"
#include "XTalentInfo.h"

MImplementRTTI(XProjectile, XObject);

vec3 MoveSmoothInTime(vec3 vCurrDir, vec3& vOutDir, vec3 vCurrentPos, vec3 vTargetPos, float fTickTime, float fStartDist, float& fRemainTime)
{
	// 방향 회전
	vec3 vDir = vTargetPos - vCurrentPos;
	float dist = vDir.Length();

	float r = (1 - dist / fStartDist);
	if (r < 0.1) r = 0.1f;
	if (r > 1) r = 1.0f;

	vCurrDir.Normalize();
	vDir.Normalize();
	vOutDir = vCurrDir * (1 - r) + vDir * r;
	vOutDir.Normalize();

	if (fRemainTime == 0.0f) fRemainTime = 0.1f;
	float fSpeed = dist * fTickTime / fRemainTime;

	// 이동
	vec3 vPos = vCurrentPos + (vOutDir * fSpeed);

	fRemainTime -= fTickTime;

	return vPos;
}

vec3 MoveParabolaInTime(vec3 vStartPos, vec3 vTargetPos, vec3 vCurrentPos, vec3& vCalParabola, float fMaxHeight, float fTickTime, float& fRemainTime)
{
	// 직선 위치 구하기
	vec3 vDir = vTargetPos - vCalParabola;
	float dist = vDir.Length();
	vDir.Normalize();

	if (fRemainTime <= 0.0f) 
	{
		return vTargetPos;
	}

	float fSpeed = dist * fTickTime / fRemainTime;
	vec3 vPos = vCalParabola + (vDir * fSpeed);
	vCalParabola = vPos;

	// 포물선 위치 구하기
	float fFullLen = (vTargetPos - vStartPos).Length();
	float fCurLen = (vCalParabola - vStartPos).Length();
	float fAngle = (180.0f * fCurLen) / fFullLen;

	float fDegree = DegreeToRadian(fAngle);
	float fTestSin = sin(DegreeToRadian(fAngle));
	float fTestZ = vPos.z;
	vPos.z += sin(DegreeToRadian(fAngle)) * fMaxHeight;
	//mlog("테스트 %f - %f(%f) --> %f - %f\n", fDegree, fTestSin, fTestSin * fMaxHeight, fTestZ, vPos.z);

	fRemainTime -= fTickTime;

	return vPos;
}

//////////////////////////////////////////////////////////////////////////
// class XProjectile
XProjectile::XProjectile(MUID uid)
: XObject(uid)
, m_pProjectileInfo(NULL)
{
	m_vVelocity			= vec3::ZERO;
	m_vPos				= vec3::ZERO;
	m_vOldPos			= vec3::ZERO;
	m_vDir				= vec3::ZERO;
	m_vTargetPos		= vec3::ZERO;

	m_vStartPos			= vec3::ZERO;
	m_vCalParabola		= vec3::ZERO;

	m_fElapsedTime		= 0.0f;
	m_fAutoDelTime		= AUTO_DELETE_TIME;

	m_bColCheck			= true;				// 충돌 처리는 기본
	m_bDelMe			= false;
	m_bWaitColJudgment	= false;

	m_pModuleEntity		= new XModuleEntity(this);

	m_nTypeID			= ETID_PROJECTILE;
	m_eAttackerTypeID	= ETID_ENTITY;

	m_uidTarget			= MUID::ZERO;
	m_nTargetGroupID	= -1;
	m_nTargetCapsuleID	= -1;

	m_pColObject		= NULL;
	m_vCoPos			= vec3::ZERO;
	m_vColDir			= vec3::ZERO;
	m_bColJudgment		= false;
	m_ColJudgmentType	= PCJT_NONE;
}

XProjectile::~XProjectile()
{
	Destroy();

	SAFE_DELETE(m_pModuleEntity);

	m_vecColJedgment.clear();
}

void XProjectile::RegisterModules()
{
	XObject::RegisterModules();

	// Initialize 먼저
	if(m_pModuleEntity)
	{
		m_pModuleEntity->Initialize();
		m_Modules.AddModule(m_pModuleEntity, true);
	}
}

void XProjectile::UnregisterModules()
{
	// Remove 먼저..
	if(m_pModuleEntity)
	{
		m_Modules.RemoveModule(m_pModuleEntity);
		m_pModuleEntity->Finalize();
	}

	XObject::UnregisterModules();
}

void XProjectile::Destroy()
{
	m_Effect.Destroy();
}

void XProjectile::OnUpdate(float fDelta)
{ 
	UpdateColJedgment();

	m_fElapsedTime += fDelta;

	UpdateMoving(fDelta);

	// 충돌 체크
	bool bCol = UpdateCol(fDelta);
	if(bCol)
	{
		// 충돌에 대한 판단을 기다린다.
		m_bWaitColJudgment = true;

		CheckColJedgment();

		// 판단을 기다리지만... 시간 오바는 그냥 지운다.
		if(CheckTimeOut())
			m_bDelMe = true;
	}

	// 이건 삭제하는 것이다.
	if(m_bDelMe)
		DeleteMe();

	// 충돌하거나 삭제하는 것이면 이펙트를 없앤다.
	if(bCol || m_bDelMe)
		m_Effect.Destroy();

	m_Effect.Update(m_vPos, m_vDir);
}


void XProjectile::SetMoveData(vec3 vPos)
{
	vec3 vOldPos		= m_vPos;
	vec3 dir			= vPos - vOldPos;
	dir.Normalize();

	// 결과 좌표 입력
	m_vPos				= vPos;
	m_vOldPos			= vOldPos;
	m_vDir				= dir;

	SetPosition(m_vPos);
	SetDirection(m_vDir);
}

//----------------------------------------------------------------------------//
// 이동 패턴
void XProjectile::UpdateMoving( float fDelta )
{
	switch(m_pProjectileInfo->m_nType)
	{
	case TPT_MISSILE:
		{
			// 직선
			Projectile_Missile(fDelta);
			break;
		}
	case TPT_MISSILE_GUIDED:
		{
			// 유도
			Projectile_Guided(fDelta);
			break;
		}
	case TPT_HITCAPSULE_GUIDED:
		{
			// 판정캡슐
			Projectile_HitCapsule(fDelta);
			break;
		}
	}
}

void XProjectile::Projectile_Missile( float fDelta )
{
	// 타겟 위치및 방향으로 발사
	switch(m_pProjectileInfo->m_nVisualType)
	{
	case TPVT_STRAIGHT:
	case TPVT_CURVE:
		Move_Straight(fDelta);
		break;
	}
}

void XProjectile::Projectile_Guided( float fDelta )
{
	// 타겟에게 무조건 따라가기(중간에 충돌 처리 없다.)
	// 타겟이 있어야 한다.
	// 타겟 체크
	XObject* pTarget = gg.omgr->Find(m_uidTarget);
	if(pTarget == NULL)
	{
		// 사라졌다.
		m_bDelMe = true;
		return;
	}

	// 유도 이동처리
	vec3 vTargetPos = GetTargetPos(m_uidTarget, m_nTargetGroupID, m_nTargetCapsuleID);
	switch(m_pProjectileInfo->m_nVisualType)
	{
	case TPVT_STRAIGHT:
		Move_Straight(fDelta, &vTargetPos);
		break;
	case TPVT_CURVE:
		Move_Curve(fDelta, &vTargetPos);
		break;
	case TPVT_PARABOLA:
		Move_Parabola(fDelta, &vTargetPos);
		break;
	}
}

void XProjectile::Projectile_HitCapsule( float fDelta )
{
	// 탤런트 판정 캡슐 위치로 발사(중간에 충돌 처리 없다.)
	switch(m_pProjectileInfo->m_nVisualType)
	{
	case TPVT_STRAIGHT:
		Move_Straight(fDelta, &m_vTargetPos);
	case TPVT_CURVE:
		Move_Curve(fDelta, &m_vTargetPos);
		break;
	case TPVT_PARABOLA:
		Move_Parabola(fDelta, &m_vTargetPos);
		break;
	}
}

void XProjectile::Move_Straight( float fDelta, vec3 * vTargetPos /*= NULL*/ )
{
	// 직선 운동(중력 없음)
	vec3 vStartPos	= m_vStartPos;
	vec3 vVelocity	= m_vVelocity;
	float fTime		= m_fElapsedTime;

	if(vTargetPos && (*vTargetPos) != vec3::ZERO)
	{
		// 좌표가 있다면...
		// 따라간다.
		vStartPos	= m_vPos;
		vVelocity	= ((*vTargetPos) - m_vPos).Normalize() * m_pProjectileInfo->m_fSpeed;
		fTime		= fDelta;
	}

	vec3 vPos = CSTalentInfoHelper::MoveStraight(vStartPos, vVelocity, fTime);
	
	SetMoveData(vPos);
}

void XProjectile::Move_Curve(float fDelta, vec3 * vTargetPos /*= NULL*/)
{
	// 유도
	// 곡선률 없음
	if(vTargetPos && (*vTargetPos) != vec3::ZERO)
	{
		vec3 vDir;
		vec3 ndir;
		float fD = (m_vStartPos - (*vTargetPos)).Length();
		vec3 vPos = MoveSmoothInTime(m_vDir, ndir, m_vPos, (*vTargetPos), fDelta, fD, m_fEstimateTime);
		vDir = ndir;

		SetMoveData(vPos);
		m_vDir = vDir;
	}
	else
	{
		// 타겟이 없다면 직선...
		Move_Straight(fDelta);
	}
}

void XProjectile::Move_Parabola( float fDelat, vec3 * vTargetPos /*= NULL*/ )
{
	if(vTargetPos && (*vTargetPos) != vec3::ZERO)
	{
		vec3 vPos = MoveParabolaInTime(m_vStartPos, (*vTargetPos), m_vPos, m_vCalParabola, 500.0f, fDelat, m_fEstimateTime);

		SetMoveData(vPos);
	}
	else
	{
		Move_Straight(fDelat);
	}
}

// 이동 패턴--
//----------------------------------------------------------------------------//

bool XProjectile::UpdateCol(float fDelta)
{
	// 충돌 검사
	bool bCol = false;
	if(m_bColCheck)
	{
		switch(m_pProjectileInfo->m_nType)
		{
		case TPT_MISSILE:
			{
				// 직선
				if(CheckRange() ||									// 사거리 체크
					CheckColObj(m_vOldPos, m_vPos) ||				// 오브젝트 체크
					CheckColTerrain(m_vOldPos, m_vPos))				// 지형 체크
				{
					bCol = true;
				}
				break;
			}
		case TPT_MISSILE_GUIDED:
			{
				// 유도
				if(CheckColObj(m_vOldPos, m_vPos) ||				// 오브젝트 체크
					CheckGPS())										// 위치 체크
				{
					bCol = true;
				}
				break;
			}
		case TPT_HITCAPSULE_GUIDED:
			{
				// 판정캡슐
				bool bCheck = true;
				// 낙하시에만 검사해도 될듯...
				if(m_pProjectileInfo->m_nVisualType == TPVT_PARABOLA &&
					m_vDir.z > 0.0f)
				{
					bCheck = false;
				}

				if(bCheck && CheckGPS())							// 위치 체크
				{
					bCol				= true;
					
					// 히트 캡슐은 서버 판정과 상관없이 피격 이펙트가 나온다.
					m_bColJudgment		= true;
					m_ColJudgmentType	= PCJT_HIT;
				}

				break;
			}
		}
	}

	return bCol;
}

#define PROJECT_RANGE 3000.0f	// 30미터

bool XProjectile::CheckRange()
{
	// 사거리 체크
	// 사거리를 넘어가면 맞지 않습니다.
	// 지형하고만 충돌 처리 합니다.
	vec3 vMove = m_vPos - m_vStartPos;
	float fRange = vMove.Length();

	if(fRange >= m_pProjectileInfo->m_fRange)
	{
		m_uidTarget = MUID::ZERO;
		return true;
	}

	return false;
}

bool XProjectile::CheckTimeOut()
{
	// 타임 아웃 체크
	if(m_fElapsedTime >= m_fAutoDelTime)
	{
		return true;
	}

	return false;
}

bool XProjectile::CheckGPS()
{
	vec3 vTargetPos = vec3::ZERO;
	// 발사자로 부터 피격에 대한 정보를 가져온다.
	if(m_uidTarget.IsValid())
	{
		XObject* pTarget = gg.omgr->Find(m_uidTarget);

		if(pTarget == NULL)
		{
			// 타겟이 없다면... 삭제
			return true;
		}

		vTargetPos = pTarget->GetPosition();
	}
	else if(m_vTargetPos != vec3::ZERO)
	{
		vTargetPos = m_vTargetPos;
	}
	else
	{
		// 목표지점이 없어 체크 하지 않는다.
		return false;
	}

	if(vTargetPos != vec3::ZERO)
	{
		// 지나갔는가?
		vec3 vNoZTargetPos = vTargetPos;
		vec3 vNoZPos = m_vPos;
		vNoZTargetPos.z = vNoZPos.z = 0.0f;
		
		vec3 vDir = vNoZTargetPos - vNoZPos;
		vDir.Normalize();

		vec3 vNoZDir = m_vDir;
		vNoZDir.z = 0.0f;
		vNoZDir.Normalize();

		float fDot = vNoZDir.DotProduct(vDir);

		if(fDot < 0.0f)
		{
			m_pColObject = NULL;
			m_vCoPos = vTargetPos;
			m_vColDir = m_vDir;
			return true;
		}
	}

	return false;
}

bool XProjectile::CheckColTerrain(vec3& old_pos, vec3& pos)
{
	if (old_pos != pos)
	{
		vec3 vOutPos, vOutNormal;

		if(gg.currentgamestate->GetWorld()->PickCollision(old_pos, pos, &vOutPos, &vOutNormal) == true)
		{
			m_Effect.CreateGroundEffect(vOutPos, m_pProjectileInfo);
			SetMoveData(vOutPos);

			// 기본적으로 충돌시 발사체가 삭제가 안된다. 판정문제 때문이다.
			// 그러므로 지형충돌은 삭제되게 한다.
			m_bDelMe = true;
			return true;
		}
	}

	return false;
}

bool XProjectile::CheckColObj(vec3& old_pos, vec3& pos)
{
	// 오브젝트 충돌
	if (old_pos != pos)
	{
		XObject * pColObject	= NULL;
		bool bCol				= false;
		float fDistance			= 0.0f;
		vec3 vOutPos;
		vec3 dir				= pos - old_pos;
		dir.Normalize();


		// 타겟을 가지고 있다면...
		// 타겟들은 BaseColCheck에서 마지막 인자에 true를 넣어주세요(꼬옥)
		XObject* pTarget = gg.omgr->Find(m_uidTarget);
		if(pTarget)
		{
			// 타겟에 대한 충돌 검사
			if(CheckColBase(old_pos, pos, pTarget, fDistance, vOutPos, true))
			{
				bCol		= true;
				pColObject	= pTarget;
			}
		}
		else
		{
			// 모든 오브젝트 충돌 검사
			float fMinDistance = 99999.0f;
			for (XObjectMap::iterator itor = gg.omgr->BeginItor();itor != gg.omgr->EndItor(); ++itor)
			{
				XObject* pObj = (*itor).second;
				if(pObj->GetUID() == m_uidAttaker ||
					pObj->GetEntityType() == m_eAttackerTypeID ||					// 타겟형이 아닌 발사체는 같은 캐릭터끼리는 충돌처리를 안한다.
					IsActorDead(pObj))											// 죽은 것은 패스
				{
					continue;
				}

				if(CheckColBase(old_pos, pos, pObj, fDistance, vOutPos) &&
					fDistance < fMinDistance)
				{
					bCol			= true;
					pColObject		= pObj;

					fMinDistance	= fDistance;
				}
			}

			fDistance = fMinDistance;
		}

		// 충돌 했으면... 이펙트를
		if(bCol == true)
		{
			m_pColObject = pColObject;
			m_vCoPos = vOutPos;
			m_vColDir = m_vDir;
			SetMoveData(old_pos);
			return true;
		}
	}

	return false;
}

bool XProjectile::CheckColBase(vec3& old_pos, vec3& pos, XObject * pObject, float & out_Distance, vec3& out_colPos, bool bDirCheck /* = false */)
{
	vec3 tar_center		= GetTargetPos(pObject->GetUID(), m_nTargetGroupID, m_nTargetCapsuleID);
	XModuleCollision* pModuleCollision = pObject->GetModuleCollision();
	if(pModuleCollision == NULL ||
		tar_center == vec3::ZERO)
	{
		return false;
	}

	out_Distance		= 0.0f;
	vec3 vImpactPos		= vec3::ZERO;
	vec3 vImpactDir		= vec3::ZERO;
	vec3 dir			= pos - old_pos;
	vec3 tar_dir		= tar_center - pos;
	float fLen			= dir.Length();
	
	dir.Normalize();
	tar_dir.Normalize();

	// 충돌 준비
	MCapsule capsuleCollision(old_pos, pos, m_pProjectileInfo->m_fHitRadius);

	// 충돌 검사
	if(pModuleCollision->ColCapsuleToObject(capsuleCollision, capsuleCollision, pos - old_pos, vImpactPos, vImpactDir, out_Distance))
	{
		out_colPos = vImpactPos;
		return true;
	}

	if(bDirCheck)
	{
		float fDist = dir.DotProduct(tar_dir);
		if(fDist < 0.0f)
		{
			// 지났다.
			out_Distance	= (tar_center - pos).Length();
			out_colPos		= tar_center;
			return true;
		}
	}

	return false;
}

bool XProjectile::OnCreate()
{
	return true;
}

void XProjectile::OnDestroy()
{
	Destroy();
}

void XProjectile::Setup( MUID& uidPlayer 
						, XTalentInfo* pInfo
						, CSProjectileInfo* pProjectileInfo 
						, vec3& vStartPos 
						, vec3& velocity 
						, vec3 vAttackerStartPos /*= vec3::ZERO*/
						, MUID uidTarget /*= MUID::Invalid() */
						, int nGroupID /*= -1 */
						, int nCapsuleID /*= -1*/)
{
	if(pInfo == NULL ||
		pProjectileInfo == NULL)
	{
		// 탤런트 정보 꼭 필요... 내용이 없더라두~~
		_ASSERT(pInfo != NULL);
		return;
	}

	m_pTalentInfo			= pInfo;
	m_pProjectileInfo		= pProjectileInfo;

	m_uidTarget				= uidTarget;
	m_uidAttaker			= uidPlayer;
	m_nTargetGroupID		= nGroupID;
	m_nTargetCapsuleID		= nCapsuleID;

	m_vStartPos				= vStartPos;
	m_vCalParabola			= vStartPos;
	m_vPos = m_vOldPos		= m_vStartPos;
	m_vDir					= velocity;
	m_vDir.Normalize(); 

	m_vVelocity				= velocity;

	// Entity에 좌표 셋팅
	SetPosition(m_vPos);
	SetDirection(m_vDir);

	XObject* pAttacker = gg.omgr->Find(uidPlayer);
	if(pAttacker)
	{
		m_eAttackerTypeID	= pAttacker->GetEntityType();
	}
	else
	{
		DeleteMe();
		return;
	}

	// 발사체 타입 설정
	MMatrix matAttackerStart;
	matAttackerStart.SetLocalMatrix(pAttacker->GetPosition(), -pAttacker->GetDirection(), pAttacker->GetUpvector());
	if(vAttackerStartPos != vec3::ZERO)
		matAttackerStart.SetTranslation(vAttackerStartPos);

	SetTargetByProjectileType(matAttackerStart);

	// 이펙트 생성
	m_Effect.Create(this, pInfo, m_pProjectileInfo);
	m_Effect.Update(m_vPos, m_vDir);

	// 공격 타입에 따라 속성을 다르게 준다.
	// 화살은 Trace가 존재한다.
	// [12/12/2007 isnara] 부분 수정
	if(pInfo->m_nSkillType == ST_ARCHERY ||
		pInfo->m_nSkillType == ST_MAGIC)
	{
		SetAtt(true);
	}
}

void XProjectile::SetAtt( bool bCol, float fAutoDelTime /*= AUTO_DELETE_TIME*/ )
{
	m_bColCheck			= bCol;					// 충돌
	m_fAutoDelTime		= fAutoDelTime;			// 자동 소멸 시간
}

void XProjectile::SetIndex(int nIdx, int nMaxIdx)
{
	float fMinAngle, fMaxAngle;

	fMinAngle = MMath::DegreeToRadian(-20.0f);
	fMaxAngle = MMath::DegreeToRadian(20.0f);

	float fAngle = fMinAngle + (fMaxAngle - fMinAngle) * ((float)nIdx / (float)nMaxIdx);

	if (nMaxIdx <= 0) fAngle = 0.0f;
	if (m_pProjectileInfo &&
		m_pProjectileInfo->m_nType == TPT_MISSILE_GUIDED)
	{
		RMatrix zrot;
		zrot.SetRotationZ(fAngle);
		zrot.TransformVect(m_vDir);
		m_vDir.z += 0.2f;
		m_vDir.Normalize();
	}
}

void XProjectile::OnRender()
{
	m_Effect.RenderTrace();
}

vec3 XProjectile::GetTargetPos( MUID uidTarget, int nGroupID /*= -1*/, int nDummyID /*= -1*/ )
{
	XObject* pTarget = gg.omgr->Find(uidTarget);
	if(pTarget)
	{
		XModuleCollision* pTargetModuleCollision = pTarget->GetModuleCollision();
		if(pTargetModuleCollision)
		{
			return pTargetModuleCollision->GetHitEffectProjectileTargetPos(nGroupID, nDummyID);
		}
	}

	return vec3::ZERO;
}

void XProjectile::SetTargetByProjectileType( MMatrix matAttacker )
{
	if(m_pProjectileInfo->m_nType == TPT_MISSILE)
	{
		// 미사일 타입은 TargetUID를 쓰지 않습니다.
		m_uidTarget = MUID::ZERO;
	}

	if (m_pProjectileInfo->m_nType == TPT_HITCAPSULE_GUIDED)
	{
		// 판정캡슐위치 가져오기
		int nSegmentIndex = m_pProjectileInfo->m_nSegmentIndexForHitCapsuleType;
		int nCapsuleIndex = m_pProjectileInfo->m_nCapsuleIndexForHitCapsuleType;

		if(nSegmentIndex < 0 ||
			nCapsuleIndex < 0)
			return;

		if(m_pTalentInfo && (int)m_pTalentInfo->m_HitInfo.m_vSegments.size() > nSegmentIndex)
		{
			if((int)m_pTalentInfo->m_HitInfo.m_vSegments[nSegmentIndex].m_vCapsules.size() > nCapsuleIndex)
			{
				MCapsule hitCapsule = m_pTalentInfo->m_HitInfo.m_vSegments[nSegmentIndex].m_vCapsules[nCapsuleIndex].Multiply(matAttacker);

				m_vTargetPos = hitCapsule.GetCenter();

				m_vDir = m_vTargetPos - m_vStartPos;
				m_vDir.Normalize();

				m_vVelocity = m_vDir * m_pProjectileInfo->m_fSpeed;

				m_uidTarget.SetZero();
			}
		}
	}

	if (m_pProjectileInfo->m_nType == TPT_MISSILE_GUIDED ||	m_pProjectileInfo->m_nType == TPT_HITCAPSULE_GUIDED)
	{
		m_fEstimateTime = 5.0f;
		vec3 vTargetPos = m_vTargetPos;
		XObject* pTarget = gg.omgr->Find(m_uidTarget);
		if(pTarget)
		{
			m_pColObject = pTarget;			// 타겟이 있다는건... 타겟형 발사체이다. 그러므로 충돌 오브젝트는 이미 정해져 있다.
			vTargetPos = pTarget->GetPosition();
		}

		float len = (m_vStartPos - vTargetPos).Length();
		float speed = m_pProjectileInfo->m_fSpeed;

		m_fEstimateTime = (len / speed);

		// 발사체 타입이 유도, 판정캡슐이면...
		// 비쥬얼 타입이 Curve면 방향을 바꾼다.
		if(m_pProjectileInfo->m_nVisualType == TPVT_CURVE)
		{
			vec3 vCurveDir = m_vDir;
			vCurveDir.z = 0.0f;
			vCurveDir.Normalize();
			
			vec3 vRight = CrossProduct(vCurveDir, UP_VECTOR);

			// 각도
			int nLeftRight = MMath::RandomNumber(0, 1);
			//float fDegree = MMath::RandomNumber(30.0f, 45.0f);
			float fDegree = 45.0f;
			if(nLeftRight == 1)
			{
				fDegree *= -1.0f;
			}

			// 방향 구하기
			mat4 mat = mat4::IDENTITY;
			mat.SetRotationMatrixAxis(UP_VECTOR, DegreeToRadian(fDegree));
			vCurveDir = vCurveDir * mat;
			vCurveDir.Normalize();

			m_vDir = vCurveDir;
			m_vVelocity = m_vDir * m_pProjectileInfo->m_fSpeed;
		}
	}
}

void XProjectile::ProjectileColResult( ProjectileColJudgmentType eType, XObject* pTarget )
{
	if(pTarget == NULL)
		return;

	// 이미 충돌했냐?
	if(IsWaitColJudgment())
	{
		if(pTarget != m_pColObject)
		{
			// 어라.. 다르네...
			// 그렇다면 충돌 위치와 방향을 내 기준으로 변경한다.
			//stColJedgment colJedgment;
			//colJedgment.eType = eType;
			//colJedgment.pTarget = pTarget;
			//m_vecColJedgment.push_back(colJedgment);
			m_vecColJedgment.push_back(stColJedgment(eType, pTarget));
			return;
		}

		ShowColEffect(m_pColObject, eType, m_vCoPos, m_vColDir);
		DeleteMe();
		//mlog("이미 충돌된 발사체\n");
		return;
	}

	// 충돌을 못한 상태라면...
	// 타겟형이나 미사일형이냐?
	if(m_uidTarget == MUID::ZERO)
	{
		// 히트 캡슐
		// 발사체가 히트캡슐이라면 별도 이펙트가 필요없을까? 일단 되돌린다.
		if(m_pProjectileInfo->m_nType == TPT_HITCAPSULE_GUIDED)
			return;

		// 미사일
		// 바로 계산해서 처리한다.
		//stColJedgment colJedgment;
		//colJedgment.eType = eType;
		//colJedgment.pTarget = pTarget;
		m_vecColJedgment.push_back(stColJedgment(eType, pTarget));
		//mlog("미사일 타겟\n");
	}
	else
	{
		// 타겟
		// 타겟형은 무조건 충돌이 되게 처리 되어 있으므로 끝나면 보여준다.
		m_bColJudgment = true;
		//mlog("타겟형 타겟\n");
	}

	m_ColJudgmentType = eType;
} 

bool XProjectile::IsEqualProjectile( MUID uidAttacker, int nTalentID )
{
	if(m_uidAttaker == uidAttacker &&
		m_pTalentInfo &&
		m_pTalentInfo->m_nID == nTalentID)
	{
		return true;
	}

	return false;
}

void XProjectile::ShowColEffect( XObject* pColObject, ProjectileColJudgmentType eType, vec3 vColPos, vec3 vColDir )
{
	// 관련 이펙트 출력
	m_Effect.CreateHitDefenceEffect(pColObject, m_Effect.GetProjectileColEffect(m_pProjectileInfo, eType), vColPos, vColDir);
}

void XProjectile::CheckColJedgment()
{
	// 이미 충돌 판정 정보가 왔다면...
	if(m_bColJudgment)
	{
		// 이펙트를 보여준다.
		ShowColEffect(m_pColObject, m_ColJudgmentType, m_vCoPos, m_vColDir);

		// 끝낸다.
		DeleteMe();
	}
}

void XProjectile::ProjectileColJedgment_Missile( ProjectileColJudgmentType eType, XObject* pTarget )
{
	XModuleCollision* pModuleCollision = pTarget->GetModuleCollision();

	// 충돌 검사
	vec3 vStartPos = m_vPos;
	XObject * pOwner = gg.omgr->Find(m_uidAttaker);
	if(pOwner)
	{
		pOwner->GetWorldTransform().TransformVect(m_pProjectileInfo->m_vLocalStartPos, vStartPos);
	}

	vec3 vTargetPos = pTarget->GetPosition();
	vTargetPos.z = vStartPos.z;

	vec3 vNearestPos = MMath::GetNearestPointOnRay(vTargetPos, vStartPos, m_vDir);

	ShowColEffect(pTarget, eType, vNearestPos, m_vDir);
	DeleteMe();
}

void XProjectile::UpdateColJedgment()
{
	for(vector<stColJedgment>::iterator it = m_vecColJedgment.begin(); it != m_vecColJedgment.end(); ++it)
	{
		ProjectileColJedgment_Missile(it->eType, it->pTarget);
	}

	m_vecColJedgment.clear();
}

bool XProjectile::Reload()
{
	if (m_pTalentInfo)
	{
		m_pTalentInfo = NULL;
		DeleteMe();
	}

	return XObject::Reload();
}
