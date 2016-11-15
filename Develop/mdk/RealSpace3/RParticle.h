#pragma once

namespace rs3 {

	struct REmitterDescription;
	struct RParticleVariable;

	struct RS_API RParticle
	{
	public:
		float	m_fLife;		// Life
		float	m_fStepLife;	// 구간의 간격 값
		float	m_fStepTime;	// 구간단위의 시간값.
		float	m_fTime;		// 지나간 시간
		float	m_fNext;		// 다음구간 시작 시간값 0 ~ 1 사이 값
		RVector2* m_pInitScales; // 초기 시작 Scale

		int		m_iIndex;		// track index
								
		// ( explosion/gather 에서 사용 )
		RVector	m_vStartPos;	//생성위치 기억, 폭발 효과시 시작위치에서 변화값 생성.
		RVector m_vRange;		//생성될때 이미터좌표에 대한 로컬 위치
		RVector m_vCreatePosition;	//생성될때 부모의 위치

		// update 할 때 사용
		RVector m_vCurPos;
		RVector m_vCurVelocity;
		RVector m_vPrevPos;

		float	m_fRadian;
		float	m_fRadiansum;

		// linear interpolation 되는 값 (난수 생성되는 속도 값)
		float	m_fCurSpeed;
		float	m_fNextSpeed;

		// sorting 할 때 사용
		float	m_fZ;

		// UV시퀀스 애니메이션에 사용
		int		m_nPrevFrame;
		int		m_nCurrentFrame;

		RVector4 m_vColor;

		RMatrix m_WorldRotationTransform;

	public:
		// 파티클 풀이 malloc을 사용하기 때문에 생성자, 소멸자가 불리지 않는다.
	//	RParticle(void);
	//	~RParticle(void);

		void	Init(const REmitterDescription* pDesc, const RVector& vCreatePosition, const RVector& vRange, const RVector& vDir, float fSpeed, float fLifeTime, float fRadian, RMatrix matWorldTransform);
		void	Init(const REmitterDescription* pDesc);
		void	Destroy();
		void	Clear();

		bool	Update(const REmitterDescription* pDesc, float fElapseTime, const RVector& vGravity);
		void	UpdateParticle(const REmitterDescription* pDesc, float fUpdateTime, const RVector& vGravity);
		
		RVector2 GetParticleScale(const REmitterDescription* pDesc);

		void	GetResultPos( const RMatrix& matTransform, bool bFollowParentPosition, bool bFollowParentRotation, RVector& outPos );

		const RVector&	GetPos()	{	return m_vCurPos; }
		const RVector&	GetVel()	{	return m_vCurVelocity; }

	protected:
		void	UpdateColor( const RVector4& vColorFirst, const RVector4& vColorSecond, float fWeight );
		void	UpdateAlpha( const RVector4& vColorFirst, const RVector4& vColorSecond, float fWeight );
		void	UpdateRotation(bool bRotate, float fUpdateTime);
		void	UpdateSpeed(float fCurMin, float fCurMax, float fNextMin, float fNextMax);
		void	UpdatePosition(const REmitterDescription* pDesc, const RParticleVariable &rVarA, const RParticleVariable &rVarB, float fInterpolationFactor, const RVector& vGravity, float fUpdateTime);

		float	GetInterpolatedValue( float fFirst, float fSecond, float fWeight );

		RVector3 GetVelocity(RVector3 vVelocityA, RVector3 vVelocityB, float fInterpolationFactor, bool bWorldVeolocity);
		RVector3 GetRandomVelocity(float fInterpolationFactor);
		RVector3 GetGForce(float fGRateA, float fGRateB, float fInterpolationFactor, RVector3 vGravity);
		RVector4 GetLastKeyColor(const std::vector<RParticleVariable>& vecParticleVar);

		RVector4 InitColor(RVector4 vFirstKeyColor, RVector4 vLastKeyColor, bool bColorRand, bool bColorRandRange);
		RVector2* InitScale(const std::vector<RParticleVariable>& vecParticleVar);
	};
}	//namespace rs3