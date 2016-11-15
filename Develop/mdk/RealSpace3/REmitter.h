#pragma once

#include "RSceneNodeID.h"
#include "RParticle.h"
#include "REmitterType.h"
#include "RParticle_Create.h"
#include "RVType.h"
#include "RPType.h"
#include <vector>
#include <string>
#include "REffectBase.h"
#include "RTimer.h"

#include "RCardinal.h"

#include "MinMax.h"
#include "RPCollection.h"
#include "REffectManager.h"


namespace rs3 {

struct RParticle;
class REffect;
class RParticleShader;

/**
Base Emitter : 기본 발생기, 공통적인 것만 ...
*/

struct RParticleHandle
{
	RParticleHandle( UINT nHandle ) : m_Handle(nHandle), m_bAlive( false ), m_fLifeTime( 0.f ) {}
	RParticleHandle( UINT nHandle, float _fLifeTime ) : m_Handle(nHandle), m_bAlive( false ), m_fLifeTime( _fLifeTime ) {}

	UINT	m_Handle;
	bool	m_bAlive;
	float	m_fLifeTime;

	bool operator < ( const RParticleHandle& rhs ) const
	{
		return ( REffectManager::GetParticle(m_Handle)->m_fZ < REffectManager::GetParticle(rhs.m_Handle)->m_fZ );
	}
};

struct REmitterDescription
{
	EMITTER_TYPE eEmitterType;
	BILLBOARD_TYPE eBillboardType;
	EMITTER_UVMOD_TYPE eUVModifierType;

	// emitter parameter
	bool bLooping;
	bool bSorting;
	bool bRotate;
	bool bFixAmount;
	bool bColorRand;
	bool bColorRandRange;
	bool bVelRand;
	bool bExplosion;
	bool bGather;
	bool bFollowParentPosition;
	bool bFollowParentRotation;
	bool bWorldVeolocity;
	float fEmitterLife;
	CMinMax<float> fRoatateRad;

	// particle parameter
	CMinMax<float> fParticleLife;
	float fParticleScale;

	RVector4 vTexUV;
	RVector2 vTexUVPivot;

	std::vector<REmitterVariable> vecEmitterVar;
	std::vector<RParticleVariable> vecParticleVar;

	void reset();
};

class RS_API REmitter : public REffectBase
{
public:
	MDeclareRTTI;
	DECLARE_NODE_ID_STRING(RSID_EMITTER);

public:
	//////////////////////////////////////////////////////////////////////////
	// 현재 파티클을 어떻게 렌더링할지 결정하기 위한 알고리즘 인터페이스.
	//
	class REmitterRenderer
	{
	public:
		virtual int		FillParticle(REmitter *pEmitter, list<RParticleHandle> *pArray, RMatrix *pWorldViewMatrix) = 0;
		static RVector*	GetRandomVector(RVector *pOut);
	};

	//////////////////////////////////////////////////////////////////////////
	// 파티클 UV 변경자
	//
	struct REmitterUVAniDesc
	{
		int		nCountX;
		int		nCountY;
		int		nMaxFrame;
		float	fFrameInterval;
		bool	bLoop;
	};

	class REmitterUVAniModifier
	{
	public:
		REmitterUVAniModifier( REmitter* pEmitter );

		RVector4	GetVertexScale( const RVector2 *pTexPivot );

		PVERTEXFORMAT* FillVertex(PVERTEXFORMAT *pVertex, const RVector* pPos, RParticle* pParticle);

		bool		SaveToXML( MXmlElement &element );
		bool		CreateFromXML( MXmlElement &element );

		void		SetDesc( const REmitterUVAniDesc& desc );
		void		GetDesc( REmitterUVAniDesc& desc );

	private:
		void		Update( RParticle* pParticle );

		REmitter*	m_pEmitter;

		int			m_nCountX;
		int			m_nCountY;
		int			m_nMaxFrame;
		float		m_fFrameInterval;
		bool		m_bLoop;

		float		m_ScaleX;
		float		m_ScaleY;

		float		m_fBlend;

		RVector4	m_UV;
		RVector4	m_UV2;
	};

	//////////////////////////////////////////////////////////////////////////
	// Emitter Play State
	//
	enum PLAY_STATE
	{
		NOT_PLAY = 0,
		PLAY_NORMAL,
		PLAY_PAUSE,
		PLAY_REST
	};


	void	TBBPostUpdate();
	void	SetCurrentUpdate( bool _bUpdate )	{ m_bCurrentUpdate = _bUpdate; }
	bool	GetCurrentUpdate()					{ return m_bCurrentUpdate; }
	void	TBBPreCreateParticle();
	void	TBBDeleteParticle();
	void	SetCardinal( float _fUpdateTime );
	void	SetCardinal();

	

private:
	bool	CheckUseSoftParticle();
	void	TBBUpdate(float fElapseTime);
	void	TBBParticleInit( DWORD _dwNum );
	void	TBBUpdateParticles(float fElapsedTime);
	DWORD	TBBAllocateHandle( int _nNum );

	bool	StartTimeCheck( float fElapseTime );
	bool	IsPlayDisable();
	void	CalcUpdateTime( float& _fUpdateTime, float& _fElapseTime );
	void	SetParticleAmount( int& _nNum, float& _fUpdateTime );
	RVector3 CalcRange();
	void	InitParticle( RParticle* _pParticle, float _fLifeTime, float fPositionInterpolationFactor );

	void	UpdateAABB();

	RVector3 GetParticleInitDirection(bool bFollowParentPosition);
	RVector3 GetParticleInitPosition(bool bFollowParentPosition, bool bFollowParentRotation);
	RVector3 GetInterpolatedWorldPosition(float fInterpolationFactor);

	// control 을 하기 위한 값
	float				m_fElapsedAfterStart;
	float				m_fStartTime;
	bool				m_bOncePlayed;
	bool				m_bCurrentUpdate;
	DWORD				m_dwCreateNum;

protected:
	virtual void		OnPostUpdate();
	virtual void		OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;

	DWORD	Update(float fElapseTime);

	DWORD	Render(  RCameraSceneNode* pCurrentCamera, RMatrix *pWorldViewMatrix);
	DWORD	RenderPrimitiveMStage(  RCameraSceneNode* pCurrentCamera);

	int		CreateParticle(int nNum);

	RParticle*	GetNewParticle();	// 파티클 풀에서 파티클 인스턴스를 얻어온다.

	RTimer	m_kTimer;

	DWORD	m_dwParticleNum;					// 디버그용
	float	m_fAmountUnder;						// 내부정보
	float	m_fMaxParticleScale;				// AABB 계산하기 위해

	bool	m_bCreateParticle;

	REmitterDescription	m_sEmitterDesc;

	// 플레이 상태
	PLAY_STATE m_eOldPlayState;
	PLAY_STATE m_eCurrentPlayState;

	// 중력 방향 ( Localtransform 상태 = withme 일 때는 적절히 변환되어져야 하므로 변수로 설정 )
	RVector m_vGravityVector;

	RCardinal		m_CrdL;						// Position 이 움직이는 경로 클래스 ( 3점 이상일 경우 Cardinal Spline으로 보간한 샘플링 테이블을 가진다. )


	// Emitter Track을 제어하기 위한 값
	float			m_fTime;		// 지나간 시간
	float			m_fNext;		// 0 ~ 1 사이 값
	float			m_fStepTime;	// 구간단위의 시간값.
	float			m_fStepLife;	// 구간의 간격 값
	int				m_iIndex;		// what da??

	// 이미터 렌더러
	REmitterRenderer	*m_pRenderer;
	// 이미터 생성
	RParticleCreator		*m_pParticleCreator;
	// 이미터 UV 변경자
	REmitterUVAniModifier *m_pUVModifier;

	const REmitterVariable *m_pVarCurrent;	// 현재 Emitter Track 값
	const REmitterVariable *m_pVarNext;		// 다음 Emitter Track 값
	
	int m_iAmount;				// 현재의 발생량
	float m_fAngle;				// 현재의 발생 각도
	RVector m_vRange;			// 현재의 발생 범위
	CMinMax<float> m_fSpeed;	// 현재의 발생 속력

	///////////////////////////////////////////////////////////////////////////
	// 파티클 관리
	list<RParticleHandle> m_listParticles; // 생성된 파티클 목록
	void UpdateParticles( float fElapsedTime );
	void ParticleSort( RMatrix* pViewMatrix );

	// Update Variable
	void UpdateEmitterVariable();
	void Reset();
	
	bool	IsEmitterLifeValid();
	void	GenerateMovingTable();
	void	CalcMaxParticleScale( const REmitterDescription * pDesc );
	DWORD	GetVisibilityAppliedColor( const RVector4& vColor, float fVisibility );

public:
	bool	m_bPath;		// emitter track에 따라 움직이는 경로를 렌더링 할 것인가 ? ( 툴에서 사용 )

	bool	IsOncePlayDone(){ return m_bOncePlayed; }
	void	SetCreateParticle( bool bCreateParticle ) { m_bCreateParticle = bCreateParticle; }

public:
	REmitter();
	virtual ~REmitter();

	virtual bool	IsLoadingCompleted();

	virtual bool	CreateFromXML(MXmlElement &element, bool bBackgroundCreation);

	virtual RSceneNode* Clone();

	virtual void	SaveToXML(MXmlElement &element);

	const REmitterDescription* GetDesc() const;

	void SortingEmitterVar();
	void SortingParticleVar();

	void	Render(  RCameraSceneNode* pCurrentCamera);

	virtual void Start() override;
	virtual void Stop() override;
	virtual void Pause() override;
	virtual bool IsAlive() override;

	int		GetCurrentCount();
	float	GetTime()							{ return m_fTime;}
	PLAY_STATE  GetCurrentPlayState()			{ return m_eCurrentPlayState; }

	DWORD	GetRenderParticleNum()				{ return m_dwParticleNum;}			

	void	DeleteAllParticles();	// 파트클 풀에 리소스를 돌려준다.

	bool	EmitterUpdate(float fUpdateTime);
	bool	PreEmitterUpdate(float fUpdateTime);

	int		GetEmittertimetrackNum(float time);
	int		GetParticletimetrackNum(float time);

	void	SetStartTime(float fStartTime)	{ m_fStartTime = fStartTime; }
	float	GetStartTime()					{ return m_fStartTime; }

	const RVector GetGravityVector() const	{ return m_vGravityVector; }

	// get/set ~er
	void	SetCurrentLife(float fn)	{	m_fTime = fn; }
	float	GetCurrentLife()			{	return m_fTime; }

	void	SetBillboardType(BILLBOARD_TYPE eType);
	BILLBOARD_TYPE GetBillboardType();

	void	SetEmitterType(EMITTER_TYPE eType);
	EMITTER_TYPE GetEmitterType();
	
	void	SetUVModifierDefault();
	void	SetUVModifierAnimation( const REmitterUVAniDesc* pDesc = NULL );
	bool	GetUVModifierAnimationDesc( REmitterUVAniDesc& desc );
	EMITTER_UVMOD_TYPE GetUVModifierType();

	void	SetEmitLife(float fn);
	float	GetEmitLife();

	void	SetLooping(bool loop);
	bool	IsLooping();

	void	SetSorting(bool b);
	bool	IsSorting();

	void	SetRotate(bool b);
	bool	IsRotate();

	void	SetRotationRadian(const CMinMax<float> &data);
	CMinMax<float>	GetRotationRadian() const;

	void	SetPScale(float fn);
	float	GetPScale();

	void	SetParticleLife(const CMinMax<float> data);
	CMinMax<float> GetParticleLife(void) const;

	void	SetRandomColor(bool b);
	bool	IsRandomColor();

	void	SetRandomColorRange(bool b);
	bool	IsRandomColorRange();

	void	SetRandomVel(bool b);
	bool	IsRandomVel();

	void	SetGather(bool b);
	bool	IsGather();

	void	SetExplosion(bool b);
	bool	IsExplosion();

	void	SetWithMe(bool b);
	bool	IsWithMe();

	void	SetFollowParentRotation(bool b);
	bool	IsFollowParentRotation();

	void	SetFixAmount(bool b);
	bool	IsFixAmount();

	bool	IsUVAniBlending();

	void SetTexUV(const RVector4 &v );
	const RVector4* GetTexUV();

	void SetTextUVPivot(const RVector2 &v);
	const RVector2* GetTexUVPivot();

	void SetWorldVelocity(bool b) { m_sEmitterDesc.bWorldVeolocity = b; }
	bool IsWorldVelocity()	{ return m_sEmitterDesc.bWorldVeolocity; }


	RVector4 GetVertexScale();

	PVERTEXFORMAT* GetVertexArray(int particleNum, int& outVertexCount);
	PVERTEXFORMAT* FillVertex( PVERTEXFORMAT* pVertex, const RVector* pPos, RParticle* pParticle );
	PVERTEXFORMAT* FillParticleVertexApplyVisibilityResult( PVERTEXFORMAT* pVertex, const RVector* pPos,
		const RVector4* pTexUV, const RVector4* pTexUV2, const RVector4& vColor, float fBlend );

	int		GetVertexCountPerParticle();

	std::vector<REmitterVariable>&		GetEmitterTrack();
	std::vector<RParticleVariable>&		GetParticleTrack();

	// REmitter에서 업데이트 되는 값의 Get
	const RVector* GetRange()	{	return &m_vRange; }

	bool		UpdateResource();
	void		Init();
	void		UpdateEmitterVar();
	void		UpdateParticleVar();

	// tool 을 위한 펑션
	bool		CreateNew();
	void		InitAllParticle();		// 현재 있는 파티클을 모두 리셋. 툴에서 호출하기 위한 용도로 만들어짐. 게임런타임시의 호출은 검증안됨. 주의요망  - 090603, OZ
};

inline int REmitter::GetCurrentCount()
{
	return m_listParticles.size();
}

inline const REmitterDescription *REmitter::GetDesc() const
{
	return &m_sEmitterDesc;
}

}
