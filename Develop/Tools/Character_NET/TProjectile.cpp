#include "stdafx.h"
#include "TProjectile.h"
#include "TEffectCharacterToolInvoker.h"
#include "TEffectCharacterToolSinglyInvoker.h"

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

TProjectile::TProjectile()
{
	m_fElapsedTime	= 0.0f;
	m_fAutoDelTime	= 0.0f;
	m_fEstimateTime	= 0.0f;

	m_bCol			= false;
}

TProjectile::~TProjectile()
{
	DelProjectile();
}

bool TProjectile::LaunchProjectile( CSProjectileInfo& csProjectilInfo, TCharacter* pActor, CSTalentInfo* pInfo )
{
	if(pActor == NULL ||
		pInfo == NULL)
		return false;

	m_pActor = pActor;
	m_pTalentInfo = pInfo;
	m_pProjectileInfo = &csProjectilInfo;

	// 발사 위치 찾기
	m_vStartPos	= GetProjectileStartPos();
	m_vPos		= m_vStartPos;
	m_vCalParabola = m_vStartPos;
	m_vOldPos	= m_vStartPos;
	m_vDir		= -m_pActor->GetDirection();
	m_vVelocity	= m_vDir * m_pProjectileInfo->m_fSpeed;

	// 타겟 셋팅
	// 기본은 시작에서 일정 거리까지이다.
	m_vTargetPos = m_vStartPos + (m_vDir * 1000.0f);
	SetTargetByProjectileType();

	// 발사 이펙트 생성
	TEffectCharacterToolSinglyInvoker fireInvoker;
	fireInvoker.Invoke(m_pProjectileInfo->m_strFireEffectName, m_vStartPos, m_vDir, vec3(vec3::AXISZ));

	// 발사체 이펙트 생성
	TEffectCharacterToolInvoker projectileInvoker;
	projectileInvoker.Invoke(m_pActor, m_pProjectileInfo->m_strEffectName, string(""), string(""), &m_EffectDelStorage);
	UpdateEffect();

	return true;
}

void TProjectile::DelProjectile()
{
	vector<EFFECT_RESULT_DATA>::iterator itProjectileEffect = m_EffectDelStorage.begin();
	while(itProjectileEffect != m_EffectDelStorage.end())
	{
		// 발사체 이펙트 업데이트
		if((*itProjectileEffect).type == CET_MODEL)
		{
			g_pMainApp->m_pEffectManager->DeleteTEffect((*itProjectileEffect).uidEffect);
		}

		itProjectileEffect++;
	}

	m_EffectDelStorage.clear();
}

vec3 TProjectile::GetProjectileStartPos()
{
	vec3 vStartPos;

	m_pActor->GetWorldTransform().TransformVect(m_pProjectileInfo->m_vLocalStartPos, vStartPos);

	return vStartPos;
}

void TProjectile::Update( float fDelta )
{
	m_fElapsedTime += fDelta;
	
	// 이동
	UpdateMoving(fDelta);

	// 충돌 체크
	if(UpdateCol(m_vOldPos, m_vPos))
	{
		m_bCol = true;
		HitEffect();
		DelProjectile();
	}

	// 이펙트 업데이트
	UpdateEffect();
}

void TProjectile::UpdateEffect()
{
	vector<EFFECT_RESULT_DATA>::iterator itProjectileEffect = m_EffectDelStorage.begin();
	while(itProjectileEffect != m_EffectDelStorage.end())
	{
		// 발사체 이펙트 업데이트
		if((*itProjectileEffect).type == CET_MODEL)
		{
			MMatrix matResult;
			if(g_pMainApp->m_pEffectManager->SetTransform((*itProjectileEffect).uidEffect, m_vPos, -m_vDir, matResult, (*itProjectileEffect).bTerrainAlign, (*itProjectileEffect).bTerrainPos) == false)
			{
				// 발사체 이펙트가 존재하지 않으면 지운다.
				itProjectileEffect = m_EffectDelStorage.erase(itProjectileEffect);
				continue;
			}
		}

		itProjectileEffect++;
	}
}

void TProjectile::SetTargetByProjectileType()
{
	if(m_pProjectileInfo->m_nType == TPT_MISSILE)
	{

	}

	if (m_pProjectileInfo->m_nType == TPT_HITCAPSULE_GUIDED)
	{
		// 판정캡슐위치 가져오기
		int nSegmentIndex = m_pProjectileInfo->m_nSegmentIndexForHitCapsuleType;
		int nCapsuleIndex = m_pProjectileInfo->m_nCapsuleIndexForHitCapsuleType;

		if(nSegmentIndex < 0 ||
			nCapsuleIndex < 0)
			return;

		if((int)m_pTalentInfo->m_HitInfo.m_vSegments.size() > nSegmentIndex)
		{
			if((int)m_pTalentInfo->m_HitInfo.m_vSegments[nSegmentIndex].m_vCapsules.size() > nCapsuleIndex)
			{
				MMatrix matWorld;
				matWorld.SetLocalMatrix(vec3::ZERO, m_pActor->GetDirection(), vec3::AXISZ);
				MCapsule hitCapsule = m_pTalentInfo->m_HitInfo.m_vSegments[nSegmentIndex].m_vCapsules[nCapsuleIndex].Multiply(matWorld);

				m_vTargetPos = hitCapsule.GetCenter();

				m_vDir = m_vTargetPos - m_vStartPos;
				m_vDir.Normalize();

				m_vVelocity = m_vDir * m_pProjectileInfo->m_fSpeed;
			}
		}
	}

	if (m_pProjectileInfo->m_nType == TPT_MISSILE_GUIDED ||	m_pProjectileInfo->m_nType == TPT_HITCAPSULE_GUIDED)
	{
		m_fEstimateTime = 5.0f;

		float len = (m_vStartPos - m_vTargetPos).Length();
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

void TProjectile::UpdateMoving(float fDelta)
{
	m_vOldPos = m_vPos;

	switch(m_pProjectileInfo->m_nType)
	{
	case TPT_MISSILE:
		{
			// 타겟 위치및 방향으로 발사
			switch(m_pProjectileInfo->m_nVisualType)
			{
			case TPVT_STRAIGHT:
			case TPVT_CURVE:
				Move_Straight(fDelta);
				break;
			}
			break;
		}
	case TPT_MISSILE_GUIDED:
	case TPT_HITCAPSULE_GUIDED:
		{
			switch(m_pProjectileInfo->m_nVisualType)
			{
			case TPVT_STRAIGHT:
				Move_Straight(fDelta, &m_vTargetPos);
				break;
			case TPVT_CURVE:
				Move_Curve(fDelta, &m_vTargetPos);
				break;
			case TPVT_PARABOLA:
				Move_Parabola(fDelta, &m_vTargetPos);
				break;
			}
			break;
		}
	}
}

void TProjectile::Move_Straight( float fDelta, vec3 * vTargetPos /*= NULL*/ )
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

	m_vPos = CSTalentInfoHelper::MoveStraight(vStartPos, vVelocity, fTime);
}

void TProjectile::Move_Curve( float fDelta, vec3 * vTargetPos /*= NULL*/ )
{
	// 유도
	// 곡선률 없음
	if(vTargetPos && (*vTargetPos) != vec3::ZERO)
	{
		vec3 vDir;
		vec3 ndir;
		float fD = (m_vStartPos - (*vTargetPos)).Length();
		m_vPos = MoveSmoothInTime(m_vDir, ndir, m_vPos, (*vTargetPos), fDelta, fD, m_fEstimateTime);
		vDir = ndir;

		m_vDir = vDir;
	}
	else
	{
		// 타겟이 없다면 직선...
		Move_Straight(fDelta);
	}
}

void TProjectile::Move_Parabola( float fDelat, vec3 * vTargetPos /*= NULL*/ )
{
	if(vTargetPos && (*vTargetPos) != vec3::ZERO)
	{
		m_vPos = MoveParabolaInTime(m_vStartPos, (*vTargetPos), m_vPos, m_vCalParabola, 500.0f, fDelat, m_fEstimateTime);
	}
	else
	{
		Move_Straight(fDelat);
	}
}

bool TProjectile::UpdateCol( vec3& old_pos, vec3& pos )
{
	// 충돌 검사
	bool bCol = false;

	switch(m_pProjectileInfo->m_nType)
	{
	case TPT_MISSILE:
	case TPT_MISSILE_GUIDED:
		{
			if(CheckGPS())
			{
				return true;
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
				m_vPos = m_vTargetPos;
				return true;
			}

			break;
		}
	}

	return false;
}

bool TProjectile::CheckGPS()
{
	vec3 vTargetPos = m_vTargetPos;
	if(m_vTargetPos != vec3::ZERO)
	{
		// 지나갔는가?
		vec3 vNoZTargetPos = vTargetPos;
		vec3 vNoZPos = m_vPos;
		vTargetPos.z = vNoZPos.z = 0.0f;

		vec3 vDir = vNoZTargetPos - vNoZPos;
		vDir.Normalize();

		vec3 vNoZDir = m_vDir;
		vNoZDir.z = 0.0f;
		vNoZDir.Normalize();

		float fDot = vNoZDir.DotProduct(vDir);

		if(fDot <= 0.0f)
		{
			return true;
		}
	}

	return false;
}

void TProjectile::HitEffect()
{
	vector<EFFECT_RESULT_DATA> vecHitEffectResult;

	TEffectCharacterToolInvoker projectileInvoker;
	projectileInvoker.Invoke(m_pActor, m_pProjectileInfo->m_strAttackEffectName, string(""), string(""), &vecHitEffectResult);

	// 맞은 위치로 수정
	for(vector<EFFECT_RESULT_DATA>::iterator it = vecHitEffectResult.begin(); it != vecHitEffectResult.end(); it++)
	{
		if(it->type == CET_MODEL)
		{
			MMatrix matResult;
			g_pMainApp->m_pEffectManager->SetTransform((*it).uidEffect, m_vPos, m_vDir, matResult, it->bTerrainAlign, it->bTerrainPos);
		}
	}
}