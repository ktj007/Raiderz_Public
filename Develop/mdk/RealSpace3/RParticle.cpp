#include "StdAfx.h"
#include "RParticle.h"
#include "REmitter.h"
#include "RUtil.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////

//RParticle::RParticle(void)
//: m_vCurPos(RVector::ZERO)
//, m_vCurVel(RVector::ZERO)
//, m_vPrevPos(RVector::ZERO)
//, m_vRange(RVector::ZERO)
//, m_vParentPos(RVector::ZERO)
//, m_pInitScales(NULL)
//{
//	m_fLife		= 0.0f;
//	m_fTime		= 0.0f;
//	m_fStepTime = 0.0f;
//	m_fStepLife = 0.0f;
//	m_fNext		= 0.0f;
//	
//	m_fCurVelocity = 0.0f;
//	m_fNextVelocity = 0.0f;
//
//	m_pEmitter = NULL;
//	m_iIndex	= 0;
//
//	m_fZ		= 0.0f;
//	m_fRadian	= 0.0f;
//	m_fRadiansum= 0.0f;
//}

//RParticle::~RParticle(void)
//{
//	Destroy();
//}

void RParticle::Init(const REmitterDescription* pDesc, const RVector& vCreatePosition, const RVector& vRange, const RVector& vDir, float fSpeed, float fLifeTime, float fRadian, RMatrix matWorldTransform)
{
	m_vRange		= vRange * fSpeed * 0.01f;
	m_vCreatePosition	= vCreatePosition;
	m_vStartPos		= vCreatePosition + vRange;
	m_vCurPos		= m_vStartPos;
	m_vPrevPos		= m_vCurPos;
	m_vCurVelocity	= vDir * fSpeed;
	m_fLife			= fLifeTime;
	m_fRadian		= fRadian;

	m_WorldRotationTransform = matWorldTransform;
	m_WorldRotationTransform.SetTranslation(RVector3::ZERO);

	Init( pDesc );
}

void RParticle::Init(const REmitterDescription* pDesc)
{
	_ASSERT(pDesc);
	_ASSERT(pDesc->vecParticleVar.size() >= 2 );

	m_iIndex = 0;

	float fCurrent = pDesc->vecParticleVar[0].fEventTime;
	m_fNext		= pDesc->vecParticleVar[1].fEventTime;

	m_fTime		= 0.0f;
	m_fStepTime = 0.0f;
	m_fStepLife = m_fLife * ( m_fNext - fCurrent );

	m_vColor = InitColor(pDesc->vecParticleVar[0].vColor, GetLastKeyColor(pDesc->vecParticleVar), pDesc->bColorRand, pDesc->bColorRandRange);

	// scale valiation init
	SAFE_DELETE_ARRAY( m_pInitScales );
	m_pInitScales = InitScale(pDesc->vecParticleVar);


	UpdateSpeed(pDesc->vecParticleVar[0].fMinSpeed, pDesc->vecParticleVar[0].fMaxSpeed, 
		pDesc->vecParticleVar[1].fMinSpeed, pDesc->vecParticleVar[1].fMaxSpeed);

	m_fRadiansum = m_fRadian;
	m_fZ = 0.f;

	m_nPrevFrame = 0;
	m_nCurrentFrame = 0;
}

RVector4 RParticle::GetLastKeyColor(const std::vector<RParticleVariable>& vecParticleVar)
{
	int endIndex = vecParticleVar.size() - 1;
	return vecParticleVar[endIndex].vColor;
}

RVector4 RParticle::InitColor(RVector4 vFirstKeyColor, RVector4 vLastKeyColor, bool bColorRand, bool bColorRandRange)
{
	RVector4 vResult;

	if( bColorRand )
	{
		vResult.Set( RandomNumber(0.0f, 1.0f), RandomNumber(0.0f, 1.0f), RandomNumber(0.0f, 1.0f), vFirstKeyColor.a);
	}
	else if( bColorRandRange )
	{
		// 첫 키와 끝 키의 파티클 색상 범위 내에서 랜덤 선택
		vResult.Set( RandomNumber(vFirstKeyColor.r, vLastKeyColor.r), RandomNumber(vFirstKeyColor.g, vLastKeyColor.g), RandomNumber(vFirstKeyColor.b, vLastKeyColor.b), vFirstKeyColor.a);
	}
	else
	{
		vResult = vFirstKeyColor;
	}

	return vResult;
}

RVector2* RParticle::InitScale(const std::vector<RParticleVariable>& vecParticleVar)
{
	size_t count = vecParticleVar.size();
	if(count<=0)
		return NULL;

	RVector2* pInitScales = new RVector2[count];
	_ASSERT(pInitScales);

	for( size_t i = 0; i < count; ++i )
	{
		const RParticleVariable& particleVar = vecParticleVar[i];

		if( particleVar.fInitScaleX == particleVar.fInitScaleY )
		{
			float scale = RandomNumber(0.0f, particleVar.fInitScaleX) * 0.01f;
			pInitScales[i].x = scale;
			pInitScales[i].y = scale;
		}
		else
		{
			pInitScales[i].x = RandomNumber(0.0f, particleVar.fInitScaleX) * 0.01f;
			pInitScales[i].y = RandomNumber(0.0f, particleVar.fInitScaleY) * 0.01f;
		}
	}

	return pInitScales;
}

void RParticle::Destroy()
{
	SAFE_DELETE_ARRAY(m_pInitScales);
}

void RParticle::Clear()
{
	m_pInitScales = NULL;
}

bool RParticle::Update(const REmitterDescription* pDesc, float fElapseTime, const RVector& vGravity)
{
	_ASSERT(pDesc);

	float fUpdateTime = 0.0f;
	do 
	{
		fUpdateTime = fElapseTime;
		float fLife = m_fStepLife - m_fStepTime;
		if ( fUpdateTime > fLife )
			fUpdateTime = fLife;

		fElapseTime -= fUpdateTime;

		m_fTime += fUpdateTime;
		m_fStepTime += fUpdateTime;

		float f = m_fStepLife - m_fStepTime;
		if ( f < FLT_EPSILON )
		{
			// go to next track 
			++m_iIndex;
			if ( m_iIndex >= (int)( pDesc->vecParticleVar.size()-1))
				return false;
			
			float fCurrent = m_fNext;
			m_fNext = pDesc->vecParticleVar[m_iIndex+1].fEventTime;

			m_fStepTime -= m_fStepLife;
			m_fStepLife = m_fLife * ( m_fNext - fCurrent );

			// m_fStepLife는 0.0f이 되어서는 안된다.
			if ( m_fStepLife <= 0.0f )
				m_fStepLife = FLT_EPSILON;

			m_fStepTime = 0.0f;


			UpdateSpeed(pDesc->vecParticleVar[m_iIndex].fMinSpeed, pDesc->vecParticleVar[m_iIndex].fMaxSpeed, 
				pDesc->vecParticleVar[m_iIndex+1].fMinSpeed, pDesc->vecParticleVar[m_iIndex+1].fMaxSpeed);
		}

		UpdateParticle( pDesc, fUpdateTime, vGravity );
	} while(fElapseTime>FLT_EPSILON);

	return true;
}

void RParticle::UpdateParticle(const REmitterDescription* pDesc, float fUpdateTime, const RVector& vGravity)
{
	_ASSERT(pDesc);

	//Particle Variable
	const RParticleVariable &rVarA = pDesc->vecParticleVar[m_iIndex];
	const RParticleVariable &rVarB = pDesc->vecParticleVar[m_iIndex+1];
	float fInterpolationFactor = m_fStepTime/m_fStepLife;
	_ASSERT(fInterpolationFactor<=1.0f);
	_ASSERT(fInterpolationFactor>=0.0f);

	if( false == pDesc->bColorRand && false == pDesc->bColorRandRange )
	{
		UpdateColor( rVarA.vColor, rVarB.vColor, fInterpolationFactor );
	}

	UpdateAlpha( rVarA.vColor, rVarB.vColor, fInterpolationFactor );

	UpdateRotation(pDesc->bRotate, fUpdateTime);

	UpdatePosition(pDesc, rVarA, rVarB, fInterpolationFactor, vGravity, fUpdateTime);
}

void RParticle::UpdateColor( const RVector4& vColorFirst, const RVector4& vColorSecond, float fWeight )
{
	m_vColor.r = GetInterpolatedValue( vColorFirst.r, vColorSecond.r, fWeight );
	m_vColor.g = GetInterpolatedValue( vColorFirst.g, vColorSecond.g, fWeight );
	m_vColor.b = GetInterpolatedValue( vColorFirst.b, vColorSecond.b, fWeight );
}

void RParticle::UpdateAlpha( const RVector4& vColorFirst, const RVector4& vColorSecond, float fWeight )
{
	m_vColor.a = GetInterpolatedValue( vColorFirst.a, vColorSecond.a, fWeight );
}

void RParticle::UpdateRotation(bool bRotate, float fUpdateTime)
{
	if( bRotate )
		m_fRadiansum += m_fRadian * fUpdateTime * 30.0f;
	else
		m_fRadiansum = m_fRadian;
}

void RParticle::UpdateSpeed(float fCurMin, float fCurMax, float fNextMin, float fNextMax)
{
	CMinMax<float> kCurSpeedMinMax(fCurMin, fCurMax);
	CMinMax<float> kNextSpeedMinMax(fNextMin, fNextMax);

	m_fCurSpeed = kCurSpeedMinMax.GetRandomNumInRange();
	m_fNextSpeed = kNextSpeedMinMax.GetRandomNumInRange();
}

void RParticle::UpdatePosition(const REmitterDescription* pDesc, const RParticleVariable &rVarA, const RParticleVariable &rVarB, 
							   float fInterpolationFactor, const RVector& vGravity, float fUpdateTime)
{
	if( pDesc->bExplosion )
	{
		float fa = m_fTime/m_fLife;

		// 이미터와의 로컬 좌표를 고수.
		m_vCurPos = MMath::LinearInterpolation(m_vStartPos, m_vStartPos+m_vRange, fa);
		return;
	}
	else if( pDesc->bGather )
	{
		float fa = m_fTime/m_fLife;
		m_vCurPos = MMath::LinearInterpolation(m_vStartPos+m_vRange, m_vStartPos, fa);
		return;
	}


	RVector3 vVelocity;
	if(pDesc->bVelRand)
		vVelocity = GetRandomVelocity(fInterpolationFactor);
	else
		vVelocity = GetVelocity(rVarA.vDirection, rVarB.vDirection, fInterpolationFactor, pDesc->bWorldVeolocity);

	RVector3 vGForce = GetGForce( rVarA.fGRate, rVarB.fGRate, fInterpolationFactor, vGravity);

	m_vCurVelocity += (vVelocity+vGForce) *fUpdateTime;

	m_vPrevPos = m_vCurPos;
	m_vCurPos += m_vCurVelocity*fUpdateTime;
}

RVector3 RParticle::GetRandomVelocity(float fInterpolationFactor)
{
	float fSpeed = MMath::LinearInterpolation(m_fCurSpeed, m_fNextSpeed, fInterpolationFactor);

	// 30 이라는 값은 30 fps 를 기준으로 한 값. 정확하지 않다.
	return RandomNumber(RVector(-30,-30,-30), RVector( 30, 30, 30)) * fSpeed;
}

RVector3 RParticle::GetVelocity(RVector3 vDirectionA, RVector3 vDirectionB, float fInterpolationFactor, bool bWorldVeolocity)
{
	RVector3 vResultDirectionA, vResultDirectionB;
	if(bWorldVeolocity)
	{
		m_WorldRotationTransform.TransformNormal(vDirectionA, vResultDirectionA);
		m_WorldRotationTransform.TransformNormal(vDirectionB, vResultDirectionB);
	}
	else
	{
		vResultDirectionA = vDirectionA;
		vResultDirectionB = vDirectionB;
	}


	RVector vDirection = MMath::LinearInterpolation( vResultDirectionA, vResultDirectionB, fInterpolationFactor);
	if ( vDirection.x + vDirection.y + vDirection.z != 0.0f )
		vDirection.FastNormalize();

	return vDirection * MMath::LinearInterpolation(m_fCurSpeed, m_fNextSpeed, fInterpolationFactor);
}

RVector3 RParticle::GetGForce(float fGRateA, float fGRateB, float fInterpolationFactor, RVector3 vGravity)
{
	float fGRate = MMath::LinearInterpolation( fGRateA, fGRateB, fInterpolationFactor);
	return (vGravity * fGRate);
}

float RParticle::GetInterpolatedValue( float fFirst, float fSecond, float fWeight )
{
	_ASSERT( fWeight >= 0.0f && fWeight <= 1.0f );
	float fInterpolatedValue = MMath::LinearInterpolation( fFirst, fSecond, fWeight );
	_ASSERT( fInterpolatedValue >= 0.0f && fInterpolatedValue <= 1.0f );
	return fInterpolatedValue;
}

RVector2 RParticle::GetParticleScale(const REmitterDescription* pDesc)
{
	_ASSERT(pDesc);

	const RParticleVariable &rVarA = pDesc->vecParticleVar[m_iIndex];
	const RParticleVariable &rVarB = pDesc->vecParticleVar[m_iIndex+1];
	float fInterpolationFactor = m_fStepTime/m_fStepLife;

	assert(fInterpolationFactor<=1.0f);
	assert(fInterpolationFactor>=0.0f);

	float fNoise = CMinMax<float>(0.0f, MMath::LinearInterpolation(rVarA.fNoise, rVarB.fNoise, fInterpolationFactor)).GetRandomNumInRange();

	float fScaleX = MMath::LinearInterpolation(rVarA.fScaleX, rVarB.fScaleX, fInterpolationFactor) + fNoise;
	float fScaleY = MMath::LinearInterpolation(rVarA.fScaleY, rVarB.fScaleY, fInterpolationFactor) + fNoise;

	float fMulScale = MMath::LinearInterpolation( rVarA.fMulScale, rVarB.fMulScale, fInterpolationFactor);
	float fAddScale = MMath::LinearInterpolation( rVarA.fAddScale, rVarB.fAddScale, fInterpolationFactor);

	float fPScale = ( pDesc->fParticleScale * fMulScale ) + fAddScale;

	float fInitScaleX = MMath::LinearInterpolation(m_pInitScales[m_iIndex].x, m_pInitScales[m_iIndex+1].x, fInterpolationFactor);
	float fInitScaleY = MMath::LinearInterpolation(m_pInitScales[m_iIndex].y, m_pInitScales[m_iIndex+1].y, fInterpolationFactor);

	return RVector2( fPScale * fScaleX + fScaleX * fInitScaleX, fPScale * fScaleY + fScaleY * fInitScaleY );
}

void RParticle::GetResultPos( const RMatrix& matTransform, bool bFollowParentPosition, bool bFollowParentRotation, RVector& outPos )
{
	outPos = GetPos();
	if( bFollowParentPosition )
	{
		RVector3 vLocalPosition = outPos - m_vCreatePosition;

		if( bFollowParentRotation )
			matTransform.TransformVect( vLocalPosition, outPos );
		else
			outPos = vLocalPosition + RVector( matTransform._41, matTransform._42, matTransform._43 );
	}
}

}	//namespace rs3