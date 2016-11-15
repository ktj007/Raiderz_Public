#include "stdafx.h"
#include "REmitter.h"
#include "RProfileInfo.h"
#include "REffectSceneNode.h"
#include "RRenderHelper.h"
#include "RXMLTemplate.h"
#include "REmitterType.h"
#include "REffectPassRenderer.h"
#include "RParticleShader.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "RInferredRenderingStrategy.h"
#include "RShaderFX.h"
#include "RUtil.h"

namespace rs3 {

#define _NOT_UP

MImplementRTTI(REmitter, REffectBase);

// EmitterTypes 구조체 : ( eType <-> String )

typedef pair<EMITTER_TYPE, std::string> EMITTER_TYPE_STRING;

EMITTER_TYPE_STRING EmitterTypes[] =
{
	EMITTER_TYPE_STRING(EMITTER_SPHERE, "SPHERE"),
	EMITTER_TYPE_STRING(EMITTER_BOX, "BOX"),
	EMITTER_TYPE_STRING(EMITTER_RING, "RING"),
	EMITTER_TYPE_STRING(EMITTERTYPE_END, "")
};

const char* EmitterTypeToString(EMITTER_TYPE eType)
{
	switch ( eType )
	{
	case EMITTER_SPHERE:	return "SPHERE";
	case EMITTER_BOX:		return "BOX";
	case EMITTER_RING:		return "RING";
	};
	return NULL;
}

EMITTER_TYPE EmitterStringToType(const char *pcString)
{
	if ( stricmp(pcString,"SPHERE") == 0 )	return EMITTER_SPHERE;
	if ( stricmp(pcString,"BOX") == 0 )		return EMITTER_BOX;
	if ( stricmp(pcString,"RING") == 0 )	return EMITTER_RING;

	return EMITTERTYPE_END;
}

TCHAR* BillboardTypeToString(BILLBOARD_TYPE type)
{
	switch ( type )
	{
	case BILLBOARD_XYZ :return _T("BILLBOARD");
	case BILLBOARD_XZ:	return _T("VERTICAL");
	case PLANE_XY:		return _T("PLANE_XY");
	case PLANE_YZ:		return _T("PLANE_YZ");
	case PLANE_ZX:		return _T("PLANE_ZX");
	case LINE_SPREAD:	return _T("SPREAD");
	case LINE_SPREAD_XY:return _T("SPREAD_XY");
	case LINE_SPREAD_YZ:return _T("SPREAD_YZ");
	case LINE_SPREAD_ZX:return _T("SPREAD_ZX");
	case EMITTER_ALIGN: return _T("EMITTER_ALIGN");
	}
	return NULL;
}

BILLBOARD_TYPE StringToBillboardType(const TCHAR *tString)
{
	if ( _tcsicmp(tString,_T("VERTICAL")) == 0 )return BILLBOARD_XZ;
	if ( _tcsicmp(tString,_T("HORIZON")) == 0)	return PLANE_XY; // 이전버전 호환
	if ( _tcsicmp(tString,_T("PLANE_XY")) == 0)	return PLANE_XY;
	if ( _tcsicmp(tString,_T("PLANE_YZ")) == 0)	return PLANE_YZ;
	if ( _tcsicmp(tString,_T("PLANE_ZX")) == 0)	return PLANE_ZX;
	if ( _tcsicmp(tString,_T("SPREAD")) == 0)	return LINE_SPREAD;
	if ( _tcsicmp(tString,_T("SPREAD_XY")) == 0)return LINE_SPREAD_XY;
	if ( _tcsicmp(tString,_T("SPREAD_YZ")) == 0)return LINE_SPREAD_YZ;
	if ( _tcsicmp(tString,_T("SPREAD_ZX")) == 0)return LINE_SPREAD_ZX;
	if ( _tcsicmp(tString,_T("EMITTER_ALIGN")) == 0)return EMITTER_ALIGN;

	return BILLBOARD_XYZ;
}

//sort for event time
template<class T>
struct Tsort
{
	bool operator()(const T& t1,const T& t2) const	{ return t1.fEventTime < t2.fEventTime; }
};

void REmitterDescription::reset()
{
	eEmitterType = EMITTER_SPHERE;
	eBillboardType = BILLBOARD_XYZ;
	eUVModifierType = EMITTER_UVMOD_DEFAULT;

	bLooping = false;
	bSorting = false;
	bRotate = false;
	bFixAmount = false;
	bColorRand = false;
	bColorRandRange = false;
	bVelRand = false;
	bGather = false;
	bFollowParentPosition = false;
	bFollowParentRotation = false;
	bExplosion = false;
	bWorldVeolocity =false;
	fEmitterLife = 0.0f;
	fRoatateRad = CMinMax<float>(-3.f, 3.f);
	fEmitterLife = 0.0f;

	fParticleScale = 1.0f;
	fParticleLife = CMinMax<float>(0, 0);
	vTexUV = RVector4(0, 0, 1, 1);
	vTexUVPivot = RVector2(0.5f, 0.5f);

	vecEmitterVar.clear();
	vecEmitterVar.resize(2);
	vecEmitterVar[0].fEventTime = 0.f;
	vecEmitterVar[1].fEventTime = 1.f;

	vecParticleVar.clear();
	vecParticleVar.resize(2);
	vecParticleVar[0].fEventTime = 0.0f;
	vecParticleVar[1].fEventTime = 1.0f;

	return;
}

REmitter::REmitter()
: REffectBase(RRP_PARTICLE)
, m_pRenderer(NULL)
, m_pParticleCreator(NULL)
, m_pUVModifier(NULL)
, m_vGravityVector(REffectSceneNode::m_vGravity)
{
	m_fElapsedAfterStart = 0.0f;
	m_fStartTime = 0.0f;
	m_bOncePlayed = false;

	m_dwParticleNum = 0;
	m_fAmountUnder = 0.0f;
	m_fMaxParticleScale = 0.0f;
	m_bCreateParticle = true;

	m_eOldPlayState = NOT_PLAY;
	m_eCurrentPlayState = NOT_PLAY;

	SetTransform(RVector(0, 0, 0), RVector(0, 1, 0));

	// emitter variable array
	m_iIndex	= 0;
	m_fTime		= 0.0f;
	m_fNext		= 0.0f;
	m_fStepTime = 0.0f;
	m_fStepLife = 0.0f;

	m_iAmount = 0;

	m_fAngle = 0.0f;

	m_vRange = RVector::ZERO;

	m_pVarCurrent = NULL;
	m_pVarNext = NULL;

	m_bPath = false;

	SetAutoUpdate(true);
}

REmitter::~REmitter()
{
	SAFE_DELETE(m_pRenderer);
	SAFE_DELETE(m_pParticleCreator);
	SAFE_DELETE(m_pUVModifier);

	DeleteAllParticles();
}

bool REmitter::UpdateResource()
{
	SetEmitterType(m_sEmitterDesc.eEmitterType);
	SetBillboardType(m_sEmitterDesc.eBillboardType);
	SetLooping(m_sEmitterDesc.bLooping);

	return true;
}

bool REmitter::CreateNew()
{
	m_sEmitterDesc.reset();
	UpdateResource();

	return true;
}

bool REmitter::IsLoadingCompleted()
{
	return true;
}


RVector3 REmitter::CalcRange()
{
	float fScale = GetResultScale(GetScale().x, GetParent());
	fScale = powf( fScale, 0.3333f ); // 실제 적용되는 스케일을 부피값으로부터 구한다.
	return m_vRange * fScale;
}

int REmitter::CreateParticle(int nNum)
{
	int nCreateCount = 0;
	for( int i = 0; i < nNum; ++i )
	{
		float fLifeTime = GetDesc()->fParticleLife.GetRandomNumInRange();
		if ( fLifeTime <= 0.0f )
			continue;

		RParticle* pParticle = GetNewParticle();
		if( pParticle )
		{
			InitParticle( pParticle, fLifeTime, (float)i/(float)nNum );
			++nCreateCount;
		}
	}

	return nCreateCount;
}

void REmitter::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	_ASSERT(pCurrentCamera);

	RPFC_THISFUNC;

	Render( pCurrentCamera );
	if ( m_bPath )
		m_CrdL.RenderPath(REngine::GetDevicePtr());
#if 0
	RRenderHelper::SetRenderState();
	RRenderHelper::RenderAxis(GetWorldTransform(), 50.f);
	RRenderHelper::EndRenderState();
#endif
}

void REmitter::OnPostUpdate()
{
	RPFC_THISFUNC;

	m_vGravityVector = REffectSceneNode::m_vGravity;
	m_kTimer.UpdateCustom(REngine::GetTimer().GetElapsedTime());

	float fElapsedTime = m_kTimer.GetElapsedTime()/1000.f;
	Update(fElapsedTime);
	UpdateAABB();
}

void REmitter::TBBPostUpdate()
{
	RPFC_THISFUNC;

	m_vGravityVector = REffectSceneNode::m_vGravity;
	float fElapsedTime = m_kTimer.GetElapsedTime()/1000.f;
	TBBUpdate(fElapsedTime);
	UpdateAABB();
}

void REmitter::Reset()
{
	m_fElapsedAfterStart = 0.0f;

	m_iIndex		= 0;
	m_fTime			= 0.0f;
	m_fStepTime		= 0.0f;
	m_fStepLife		= 0.0f;

	m_kTimer.Reset();

	m_CrdL.Reset();

	const REmitterDescription *pDesc = GetDesc();

	_ASSERT(pDesc->vecEmitterVar.size()>1);
	_ASSERT(pDesc->vecParticleVar.size()>1);

	m_pVarCurrent = &pDesc->vecEmitterVar[m_iIndex];
	m_pVarNext = &pDesc->vecEmitterVar[m_iIndex+1];

	m_iAmount	= m_pVarCurrent->uAmount;
	m_fSpeed = CMinMax<float>(m_pVarCurrent->fMinSpeed, m_pVarCurrent->fMaxSpeed);

	UpdateEmitterVariable();
}

void REmitter::Start()
{
	m_eOldPlayState = m_eCurrentPlayState;
	if (m_eCurrentPlayState == PLAY_REST || m_eCurrentPlayState == PLAY_PAUSE )
	{
		DeleteAllParticles();
	}

//	_ASSERT( IsEmitterLifeValid() );
	if( IsEmitterLifeValid() )
	{
		m_eCurrentPlayState = PLAY_NORMAL;

		Reset();

		// Update()시 위치 업데이트가 안될 수 있으므로, 처음에 한번 위치를 초기화 해준다.
		SetPosition( m_CrdL.Update(0.0f) );
	}
}

void REmitter::Stop()
{
	if (m_eCurrentPlayState == PLAY_NORMAL)
	{
		m_eOldPlayState = m_eCurrentPlayState;
		m_eCurrentPlayState = PLAY_REST;
	}
	else if (m_eCurrentPlayState == PLAY_PAUSE)
	{
		m_eOldPlayState = m_eCurrentPlayState;
		m_eCurrentPlayState = NOT_PLAY;
		DeleteAllParticles();
	}
}

void REmitter::Pause()
{
	if (m_eCurrentPlayState == PLAY_NORMAL || m_eCurrentPlayState == PLAY_REST)
	{
		m_eOldPlayState = m_eCurrentPlayState;
		m_eCurrentPlayState = PLAY_PAUSE;
	}
	else if (m_eCurrentPlayState == PLAY_PAUSE)
	{
		m_eCurrentPlayState = m_eOldPlayState;
		m_eOldPlayState = PLAY_PAUSE;
	}
}

bool REmitter::IsAlive()
{
	// 라이프가 없으면 살아있으면 안된다.
//	_ASSERT( IsEmitterLifeValid() );
	if( false == IsEmitterLifeValid() )
		return false;

	// 루프중에서
	if( IsLooping() )
	{
		if( GetCurrentPlayState() == PLAY_REST )
		{
			if( GetCurrentCount() != 0 )
				return true;
		}
		else // 그렇지 않은경우
		{
			return true;
		}
	}
	// 루프가 아닌것 중에서 플레이 완료가 안되었거나 파티클이 남아있다면 아직 죽지 말아야할것
	else
	if( IsOncePlayDone() == false || GetCurrentCount() != 0 )
	{
		return true;
	}

	return false;
}

RParticle* REmitter::GetNewParticle()
{
	UINT nHandle = REffectManager::GetNewParticle();
	RParticleHandle particleHandle(nHandle);
	m_listParticles.push_back(particleHandle);
	return REffectManager::GetParticle(nHandle);
}

void REmitter::DeleteAllParticles()
{
	for( list<RParticleHandle>::iterator it = m_listParticles.begin(); it != m_listParticles.end(); ++it )
	{
		REffectManager::RemoveParticle((*it).m_Handle);
	}
	m_listParticles.clear();
}

void REmitter::TBBUpdateParticles(float fElapsedTime)
{
	list<RParticleHandle>::iterator it = m_listParticles.begin();
	while( it != m_listParticles.end() )
	{
		if( (*it).m_bAlive )	// 살아있는 것 업데이트
		{
			UINT nHandle = (*it).m_Handle;
			(*it).m_bAlive = REffectManager::GetParticle(nHandle)->Update(GetDesc(), fElapsedTime, GetGravityVector());			
		}
		++it;
	}
}

void REmitter::TBBDeleteParticle()
{
	list<RParticleHandle>::iterator it = m_listParticles.begin();
	while( it != m_listParticles.end() )
	{
		if( !(*it).m_bAlive )	// 죽은것 지운다.
		{
			UINT nHandle = (*it).m_Handle;
			it = m_listParticles.erase(it);
			REffectManager::RemoveParticle(nHandle);
		}
		else
			++it;
	}
}

void REmitter::UpdateParticles(float fElapsedTime)
{
	list<RParticleHandle>::iterator it = m_listParticles.begin();
	while( it != m_listParticles.end() )
	{
		UINT nHandle = (*it).m_Handle;
		if( !REffectManager::GetParticle(nHandle)->Update(GetDesc(), fElapsedTime, GetGravityVector()) )
		{
			m_listParticles.erase(it++);
			REffectManager::RemoveParticle(nHandle);
		}
		else
		{
			++it;
		}
	}
}

void REmitter::ParticleSort(RMatrix* pViewMatrix)
{
	RPFC_THISFUNC;
	RVector V;
	RParticle* pParticle;
	for( list<RParticleHandle>::iterator it = m_listParticles.begin(); it != m_listParticles.end(); ++it )
	{
		// Z값계산
		pParticle = REffectManager::GetParticle((*it).m_Handle);
		pViewMatrix->TransformVect(pParticle->m_vCurPos, V);
		pParticle->m_fZ = V.z;
	}

	m_listParticles.sort();
}

void REmitter::InitAllParticle()
{
	for( list<RParticleHandle>::iterator it = m_listParticles.begin(); it != m_listParticles.end(); ++it )
		REffectManager::GetParticle((*it).m_Handle)->Init(this->GetDesc());
}

void REmitter::UpdateEmitterVariable()
{
	const REmitterDescription *pDesc = GetDesc();

	float fCurrent	= m_pVarCurrent->fEventTime;
	m_fNext			= m_pVarNext->fEventTime;
	m_fStepLife		= pDesc->fEmitterLife * ( m_fNext - fCurrent );

	float fAlphaValue = ( m_fStepLife > 0.0f ? m_fStepTime/m_fStepLife : 0.0f );
	_ASSERT(fAlphaValue>=0.0f);
	_ASSERT(fAlphaValue<=1.0f);

	if( !pDesc->bFixAmount )
		m_iAmount = MMath::LinearInterpolation( m_pVarCurrent->uAmount, m_pVarNext->uAmount, fAlphaValue);

	m_fAngle = MMath::LinearInterpolation(m_pVarCurrent->fAngle, m_pVarNext->fAngle, fAlphaValue);
	m_vRange = MMath::LinearInterpolation(m_pVarCurrent->vRange, m_pVarNext->vRange, fAlphaValue);
	
	m_fSpeed.m_Min = MMath::LinearInterpolation(m_pVarCurrent->fMinSpeed, m_pVarNext->fMinSpeed, fAlphaValue);
	m_fSpeed.m_Max = MMath::LinearInterpolation(m_pVarCurrent->fMaxSpeed, m_pVarNext->fMaxSpeed, fAlphaValue);
}


DWORD REmitter::TBBAllocateHandle( int _nNum )
{
	_ASSERT( m_pParticleCreator );
	const REmitterDescription *pDesc = GetDesc();
	DWORD dwCreateNum(0);

	for(int i = 0 ; i < _nNum ; ++i)
	{
		float fLifeTime( pDesc->fParticleLife.GetRandomNumInRange() );
		if ( fLifeTime <= 0.0f )
			continue;

		UINT nHandle( REffectManager::GetNewParticle() );
		RParticleHandle particleHandle(nHandle, fLifeTime);
		m_listParticles.push_back(particleHandle);
		++dwCreateNum;
	}

	return dwCreateNum;
}


void REmitter::TBBPreCreateParticle()
{
	DWORD dwNum(0);
	float fUpdateTime(0.f);
	float fLife = 0.f;

	m_kTimer.UpdateCustom(REngine::GetTimer().GetElapsedTime());
	float fElapsedTime(m_kTimer.GetElapsedTime() / 1000.f);

	do
	{		
		CalcUpdateTime( fUpdateTime, fElapsedTime );

		if( m_bCreateParticle )
		{
			int		nNum( 0 );
			SetParticleAmount( nNum, fUpdateTime );

			// 정상이거나, Looping이 아닌데 Stop을 눌러서 PLAY_REST가 된 경우에는 파티클을 계속 생성해준다.
			if ( m_eCurrentPlayState == PLAY_NORMAL && nNum > 0 )
				dwNum += TBBAllocateHandle( nNum );
		}

	} while( fUpdateTime > FLT_EPSILON ? true : false );

	m_dwCreateNum = dwNum;
}


void REmitter::SetCardinal()
{
	float fElapsedTime(m_kTimer.GetElapsedTime()/1000.f);

	RVector vPos = m_CrdL.Update( fElapsedTime );
	if( GetPosition() != vPos )
	{
		SetPosition( vPos );
		UpdateTransform();
	}
}

void REmitter::SetCardinal( float _fUpdateTime )
{
	RVector vPos = m_CrdL.Update( _fUpdateTime );
	if( GetPosition() != vPos )
	{
		SetPosition( vPos );
		UpdateTransform();
	}
}

bool REmitter::PreEmitterUpdate(float fUpdateTime)
{
	const REmitterDescription *pDesc = GetDesc();

	m_fTime += fUpdateTime;
	m_fStepTime += fUpdateTime;

	// Next Change
	float f = m_fStepLife - m_fStepTime;
	if ( f < FLT_EPSILON  )		// fStep 값으로도 검사 .
	{
		++m_iIndex;

		if ( m_iIndex < (int)pDesc->vecEmitterVar.size()-1 )
		{
			m_pVarCurrent = &pDesc->vecEmitterVar[m_iIndex];
			m_pVarNext = &pDesc->vecEmitterVar[m_iIndex+1];

			m_iAmount	= m_pVarCurrent->uAmount;
			m_fStepTime = 0.0f;
		}
		else
		{
			m_bOncePlayed = true;

			// 반복할것 다시시작
			if( pDesc->bLooping )
			{
				// play normal 혹은 play rest만 업데이트가 되어 들어올 수 있다.
				_ASSERT( m_eCurrentPlayState != PLAY_PAUSE );
				_ASSERT( m_eCurrentPlayState != NOT_PLAY );
				if (m_eCurrentPlayState == PLAY_NORMAL)
					Reset();
			}
			else
			{
				m_eCurrentPlayState = PLAY_REST;
			}

			return false;
		}
	}

	_ASSERT(m_pVarCurrent);
	_ASSERT(m_pVarNext);

	UpdateEmitterVariable();

	return true;
}

bool REmitter::EmitterUpdate(float fUpdateTime)
{
	if( !PreEmitterUpdate( fUpdateTime ) )
		return false;

	SetCardinal( fUpdateTime );
	return ( fUpdateTime > FLT_EPSILON ? true : false);
}


void REmitter::TBBParticleInit( DWORD _dwNum )
{
	list<RParticleHandle>::iterator itr = m_listParticles.begin();

	for( DWORD i = 0; i < _dwNum && itr != m_listParticles.end(); ++i )
	{
		if( itr->m_bAlive )
		{
			--i;
			++itr;
			continue;
		}

		RParticle* pParticle = REffectManager::GetParticle( itr->m_Handle );

		if( pParticle )
		{
			InitParticle( pParticle, itr->m_fLifeTime, (float)i/(float)_dwNum );
			itr->m_bAlive = true;
		}

		++itr;
	}
}

RVector3 REmitter::GetParticleInitDirection(bool bFollowParentPosition)
{
	_ASSERT( m_pParticleCreator );

	if( bFollowParentPosition )
		return m_pParticleCreator->GetDirection(m_fAngle);
	else
	{
		RVector3 vWorldTransformedDirection;
		GetWorldTransform().TransformNormal( m_pParticleCreator->GetDirection(m_fAngle), vWorldTransformedDirection );

		return vWorldTransformedDirection;
	}
}

RVector3 REmitter::GetParticleInitPosition(bool bFollowParentPosition, bool bFollowParentRotation)
{
	_ASSERT( m_pParticleCreator );

	if( bFollowParentPosition && bFollowParentRotation )
		return m_pParticleCreator->GetPosition( CalcRange() );
	else
	{
		RVector3 vWorldRotatedPosition;
		GetWorldTransform().TransformNormal( m_pParticleCreator->GetPosition( CalcRange() ), vWorldRotatedPosition );

		return vWorldRotatedPosition;
	}
}

RVector3 REmitter::GetInterpolatedWorldPosition(float fInterpolationFactor)
{
	RVector3 vPrevPosition = GetWorldPositionPrev();
	if(vPrevPosition==RVector3::ZERO)
		return GetWorldPosition();
	else
		return MMath::LinearInterpolation(vPrevPosition, GetWorldPosition(), fInterpolationFactor);
}

void REmitter::InitParticle( RParticle* _pParticle, float _fLifeTime, float fPositionInterpolationFactor)
{
	const REmitterDescription *pDesc( GetDesc() );

	RVector3 vEmitterPosition = GetInterpolatedWorldPosition(fPositionInterpolationFactor);
	RVector3 vParticleDirection = GetParticleInitDirection(pDesc->bFollowParentPosition);
	RVector3 vParticlePosition = GetParticleInitPosition(pDesc->bFollowParentPosition, pDesc->bFollowParentRotation);
	float fSpeed= m_fSpeed.GetRandomNumInRange();
	float fRotationRad = pDesc->fRoatateRad.GetRandomNumInRange();

	_pParticle->Init( pDesc, vEmitterPosition, vParticlePosition, vParticleDirection, fSpeed, _fLifeTime, fRotationRad, GetWorldTransform() );
}


bool REmitter::IsPlayDisable()
{
	if ( m_eCurrentPlayState == PLAY_PAUSE || m_eCurrentPlayState == NOT_PLAY )
		return true;

	return false;
}

bool REmitter::StartTimeCheck( float fElapseTime )
{
	bool bEmitterUpdate = true;
	m_fElapsedAfterStart += fElapseTime;
	if (m_fElapsedAfterStart < m_fStartTime)
		bEmitterUpdate = false;

	return bEmitterUpdate;
}

void REmitter::CalcUpdateTime( float& _fUpdateTime, float& _fElapseTime )
{
	_fUpdateTime =  _fElapseTime;
	float fLife( m_fStepLife - m_fStepTime );
	if ( _fUpdateTime > fLife )
		_fUpdateTime = fLife;
	_fElapseTime -= _fUpdateTime;
}


void REmitter::TBBUpdate(float fElapseTime)
{
	_ASSERT( m_pParticleCreator );

	if ( IsPlayDisable() )
	{
		SetCurrentUpdate( false );
		return;
	}

	TBBUpdateParticles( fElapseTime );

	if ( !StartTimeCheck( fElapseTime ) )
	{
		SetCurrentUpdate( false );
		return;
	}

	float fUpdateTime( 0.f );
	CalcUpdateTime( fUpdateTime, fElapseTime );

	// 정상이거나, Looping이 아닌데 Stop을 눌러서 PLAY_REST가 된 경우에는 파티클을 계속 생성해준다.
	if ( m_eCurrentPlayState == PLAY_NORMAL && m_dwCreateNum > 0 )
		TBBParticleInit( m_dwCreateNum );

	PreEmitterUpdate( fUpdateTime );
}


void REmitter::SetParticleAmount( int& _nNum, float& _fUpdateTime )
{
	float	fNum( 0.f );
	if( GetDesc()->bFixAmount )
	{
		_ASSERT(m_iAmount >= 0);
		fNum = max(m_iAmount, 0.0f);
		_nNum = (int)(fNum);
		m_fAmountUnder = 0.0f;
		m_iAmount = 0;
	}
	else
	{
		_ASSERT(m_iAmount >= 0);
		fNum = max((m_iAmount * _fUpdateTime) + m_fAmountUnder, 0.0f);
		_nNum = (int)(fNum);

		m_fAmountUnder = fNum - _nNum;
		_ASSERT(m_fAmountUnder>=0.0f);
	}
}


DWORD REmitter::Update(float fElapseTime)
{
	_ASSERT( m_pParticleCreator );

	if ( IsPlayDisable() )
		return 0;

	// 파티클 업데이트 -> 현재 이미터 상태만큼 생성 -> 이미터 상태 업데이트 ( loop )
	// 파티클 업데이트


	UpdateParticles( fElapseTime );

	if ( !StartTimeCheck( fElapseTime ) )
		return 0;

	// Emitter 적용
	DWORD dwNum = 0;
	float fUpdateTime = 0.0f;
	do 
	{
		CalcUpdateTime( fUpdateTime, fElapseTime );

		if( m_bCreateParticle )
		{
			int		nNum( 0 );
			SetParticleAmount( nNum, fUpdateTime );

			// 정상이거나, Looping이 아닌데 Stop을 눌러서 PLAY_REST가 된 경우에는 파티클을 계속 생성해준다.
			if ( m_eCurrentPlayState == PLAY_NORMAL && nNum > 0 )
				dwNum += CreateParticle(nNum);
		}

	} while ( EmitterUpdate(fUpdateTime));

	return dwNum;
}

void REmitter::Render( RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;
	if ( m_pRenderer == NULL )
		return;
	if ( m_pParticleCreator == NULL )
		return;

	RMatrix matView =pCurrentCamera->GetViewMatrix();
	m_dwParticleNum = Render( pCurrentCamera, &matView);
}

DWORD REmitter::Render(  RCameraSceneNode* pCurrentCamera, RMatrix *pWorldViewMatrix)
{
	RPFC_THISFUNC;
	_ASSERT(REngine::GetDevicePtr()!=NULL);
	_ASSERT(m_pRenderer);
	_ASSERT(m_pParticleCreator);

	if ( GetCurrentCount() == 0 )
		return 0;

	const REmitterDescription *pDesc = GetDesc();

	if ( pDesc->bSorting )
		ParticleSort(pWorldViewMatrix);

	// DrawParticle은 버텍스버퍼에 채워넣는다.
	DWORD dwNum = m_pRenderer->FillParticle(this, &m_listParticles, pWorldViewMatrix);
	_ASSERT(dwNum!=0);

	RenderPrimitiveMStage( pCurrentCamera );
	
	return dwNum;
}

bool REmitter::CheckUseSoftParticle()
{
	return
		(REngine::GetSceneManager().GetRenderingStrategy()->IsEnableSoftParticle()) &&
		(GetDesc()->eBillboardType != PLANE_XY);	// XY평면 Align 빌보드는 수평면에 가까우면 렌더가 아예 안될 수 있으므로 소프트 파티클을 하지 않는다.
}

DWORD REmitter::RenderPrimitiveMStage(  RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;
	bool bDistortion = BeginDistortion(pCurrentCamera);

	bool bSoftParticleBlendAlpha = false;
	bool bSoftParticleBlendColor = false;
	if( CheckUseSoftParticle() )
	{
		if( GetSrcBlendMode() == RBLEND_SRCALPHA || GetDestBlendMode() == RBLEND_INVSRCALPHA )
			bSoftParticleBlendAlpha = true;

		if( GetSrcBlendMode() == RBLEND_SRCCOLOR || GetDestBlendMode() == RBLEND_INVSRCCOLOR )
			bSoftParticleBlendColor = true;
	}

	RParticleShaderCreateParam shaderCreateParam;
	shaderCreateParam.bAlphaTest				= IsUseAlphaTest();
	shaderCreateParam.bSelfIllumination			= GetSITexture() ? true : false;
	shaderCreateParam.bDistortion				= bDistortion;
	shaderCreateParam.bSoftParticleBlendAlpha	= bSoftParticleBlendAlpha;
	shaderCreateParam.bSoftParticleBlendColor	= bSoftParticleBlendColor;
	shaderCreateParam.bMultiLayerBlend			= IsUVAniBlending();

	RParticleShader* pParticleShader = REngine::GetSceneManager().GetPassRenderer<REffectPassRenderer>()->GetParticleShader();
	_ASSERT( pParticleShader );
	RShaderFX* pShader = pParticleShader->GetShader( shaderCreateParam );
	_ASSERT( pShader );

	int nPrimitive = 0;
	if( pShader )
	{
		SetRenderState( pCurrentCamera, pParticleShader, bDistortion );
		{
			RTechnique technique = pShader->GetTechniqueByIndex(0);

			technique.Begin(NULL, TECH_PRESERVE_NOT);
			technique.BeginPass();

			if( nPrimitive = REffectManager::DrawPrimitiveTriangleList() )
				REngine::GetResourceProfiler().AddPolygon( PPT_PARTICLE, nPrimitive );

			technique.EndPass();
			technique.End();
		}
		RestoreRenderState( pCurrentCamera, pParticleShader );
	}

	if(bDistortion)
		EndDistortion(pCurrentCamera, false);

	return nPrimitive;
}

void REmitter::SetLooping(bool loop)
{
	m_sEmitterDesc.bLooping = loop;
}

bool REmitter::IsLooping()
{
	return m_sEmitterDesc.bLooping;
}

void REmitter::SortingEmitterVar()
{
	sort(m_sEmitterDesc.vecEmitterVar.begin(), m_sEmitterDesc.vecEmitterVar.end(), Tsort<REmitterVariable>());

#ifdef _DEBUG
	// vecEmitterVar 이벤트 시간 정렬 체크
	{
		const REmitterDescription *pDesc = GetDesc();

		size_t lastIndex = pDesc->vecEmitterVar.size() - 1;
		_ASSERT(pDesc->vecEmitterVar[0].fEventTime == 0.0f);
		_ASSERT(pDesc->vecEmitterVar[lastIndex].fEventTime == 1.0f);
		for ( size_t si = 0; si<pDesc->vecEmitterVar.size(); ++si )
		{
			_ASSERT(pDesc->vecEmitterVar[si].fEventTime >= 0.0f);
			_ASSERT(pDesc->vecEmitterVar[si].fEventTime <= 1.0f);
		}
	}
#endif
}

void REmitter::SortingParticleVar()
{
	sort(m_sEmitterDesc.vecParticleVar.begin(), m_sEmitterDesc.vecParticleVar.end(), Tsort<RParticleVariable>());

#ifdef _DEBUG
	// vecParticleVar 이벤트 시간 정렬 체크
	{
		const REmitterDescription *pDesc = GetDesc();

		int lastIndex = pDesc->vecParticleVar.size()-1;
		_ASSERT(pDesc->vecParticleVar[0].fEventTime == 0.0f );
		_ASSERT(pDesc->vecParticleVar[lastIndex].fEventTime == 1.0f);

		for ( UINT ui = 0; ui<pDesc->vecParticleVar.size(); ++ui )
		{
			_ASSERT(pDesc->vecParticleVar[ui].fEventTime >= 0.0f);
			_ASSERT(pDesc->vecParticleVar[ui].fEventTime <= 1.0f);
		}
	}
#endif
}

void REmitter::UpdateEmitterVar()
{
	SortingEmitterVar();

	if( IsEmitterLifeValid() )
		GenerateMovingTable();
}

void REmitter::UpdateParticleVar()
{
	SortingParticleVar();
	CalcMaxParticleScale( GetDesc() );
}

void REmitter::CalcMaxParticleScale( const REmitterDescription * pDesc ) 
{
	// 최대 파티클 크기 구하기
	float fMaxParticleScale = 0.0f;
	for ( size_t si = 0; si<pDesc->vecParticleVar.size(); ++si )
	{
		float fMaxScale = max(pDesc->vecParticleVar[si].fScaleX, pDesc->vecParticleVar[si].fScaleY);

		float fMaxInitScale = max(pDesc->vecParticleVar[si].fInitScaleX, pDesc->vecParticleVar[si].fInitScaleY) * 0.01f;
		if( fMaxInitScale < 0.0f )
			fMaxInitScale = -fMaxInitScale;

		float fParticleScale = fMaxScale * pDesc->vecParticleVar[si].fMulScale +
			pDesc->vecParticleVar[si].fAddScale +
			fMaxScale * fMaxInitScale +
			pDesc->vecParticleVar[si].fNoise;

		if (fMaxParticleScale < fParticleScale)
			fMaxParticleScale = fParticleScale;
	}
	RVector4 texUV = GetVertexScale();
	fMaxParticleScale *= max( max(texUV.x, texUV.z), max(texUV.w, texUV.y) ) * 2;
	m_fMaxParticleScale = fMaxParticleScale;
}

//tools
int REmitter::GetEmittertimetrackNum(float time)
{
	const REmitterDescription *pDesc = GetDesc();

	for ( UINT ui = 0; ui<pDesc->vecEmitterVar.size(); ++ui )
	{
		if( pDesc->vecEmitterVar[ui].fEventTime == time)
			return ui;
	}

	return -1;
}

//tools
int REmitter::GetParticletimetrackNum(float time)
{
	const REmitterDescription *pDesc = GetDesc();

	for ( UINT ui = 0; ui<pDesc->vecParticleVar.size(); ++ui )
	{
		if( pDesc->vecParticleVar[ui].fEventTime == time)
			return ui;
	}

	return -1;
}

void REmitter::UpdateAABB()
{
	const REmitterDescription *pDesc = GetDesc();

	// AABB 를 같이 업데이트, Emitter 크기만큼 초기화 시킨다.
	RBoundingBox rangeBox;
	rangeBox.vmin =-m_vRange * .5f;
	rangeBox.vmax = m_vRange * .5f;

	RBoundingBox aabb;
	TransformBox(&aabb, rangeBox, GetWorldTransform());

	if ( GetCurrentCount() )
	{
		RBoundingBox particleBox;
		particleBox.Initialize();
		RVector pos;
		for( list<RParticleHandle>::iterator it = m_listParticles.begin(); it != m_listParticles.end(); ++it )
		{
			RParticle *pParticle = REffectManager::GetParticle((*it).m_Handle);
			pParticle->GetResultPos(GetWorldTransform(), pDesc->bFollowParentPosition, pDesc->bFollowParentRotation, pos);
			particleBox.Add(pos);
		}

		aabb.Add(particleBox);
	}

	// 파티클 최대 크기 만큼 넓힌다.
	float fTransformedScale = GetResultScale( GetScale().x, GetParent() );
	RVector vParticleMaxSize(m_fMaxParticleScale, m_fMaxParticleScale, m_fMaxParticleScale);
	vParticleMaxSize *= fTransformedScale;
	aabb.vmin -= vParticleMaxSize;
	aabb.vmax += vParticleMaxSize;

	SetAABB( aabb );
}

BILLBOARD_TYPE REmitter::GetBillboardType()
{
	return m_sEmitterDesc.eBillboardType;
}

EMITTER_TYPE REmitter::GetEmitterType()
{
	return m_sEmitterDesc.eEmitterType;
}

EMITTER_UVMOD_TYPE REmitter::GetUVModifierType()
{
	return m_sEmitterDesc.eUVModifierType;
}

void REmitter::SetEmitLife(float fn)
{
	m_sEmitterDesc.fEmitterLife = fn;
	UpdateEmitterVar();
	Reset();
}

float REmitter::GetEmitLife()
{
	return m_sEmitterDesc.fEmitterLife;
}

void REmitter::SetSorting(bool b)
{
	m_sEmitterDesc.bSorting = b;
}

bool REmitter::IsSorting()
{
	return m_sEmitterDesc.bSorting;
}

void REmitter::SetRotate(bool b)
{
	m_sEmitterDesc.bRotate = b;
}

bool REmitter::IsRotate()
{
	return m_sEmitterDesc.bRotate;
}

void REmitter::SetRotationRadian(const CMinMax<float> &data)
{
	m_sEmitterDesc.fRoatateRad = data;
}

CMinMax<float> REmitter::GetRotationRadian() const
{
	return m_sEmitterDesc.fRoatateRad;
}

void REmitter::SetPScale(float fn)
{
	m_sEmitterDesc.fParticleScale = fn;
}

float REmitter::GetPScale()
{
	return m_sEmitterDesc.fParticleScale;
}

void REmitter::SetParticleLife(const CMinMax<float> data)
{
	m_sEmitterDesc.fParticleLife = data;
}

CMinMax<float> REmitter::GetParticleLife() const
{
	return m_sEmitterDesc.fParticleLife;
}

void REmitter::SetRandomColor(bool b)
{
	m_sEmitterDesc.bColorRand = b;
}

bool REmitter::IsRandomColor()
{
	return m_sEmitterDesc.bColorRand;
}

void REmitter::SetRandomColorRange(bool b)
{
	m_sEmitterDesc.bColorRandRange = b;
}

bool REmitter::IsRandomColorRange()
{
	return m_sEmitterDesc.bColorRandRange;
}

void REmitter::SetRandomVel(bool b)
{
	m_sEmitterDesc.bVelRand = b;
}

bool REmitter::IsRandomVel()
{
	return m_sEmitterDesc.bVelRand;
}

void REmitter::SetGather(bool b)
{
	m_sEmitterDesc.bGather = b;
}

bool REmitter::IsGather()
{
	return m_sEmitterDesc.bGather;
}

void REmitter::SetExplosion(bool b)
{
	m_sEmitterDesc.bExplosion = b;
}

bool REmitter::IsExplosion()
{
	return m_sEmitterDesc.bExplosion;
}

void REmitter::SetWithMe(bool b)
{
	m_sEmitterDesc.bFollowParentPosition = b;
}

bool REmitter::IsWithMe()
{
	return m_sEmitterDesc.bFollowParentPosition;
}

void REmitter::SetFollowParentRotation(bool b)
{
	m_sEmitterDesc.bFollowParentRotation = b;
}

bool REmitter::IsFollowParentRotation()
{
	return m_sEmitterDesc.bFollowParentRotation;
}

void REmitter::SetFixAmount(bool b)
{
	m_sEmitterDesc.bFixAmount = b;
}

bool REmitter::IsFixAmount()
{
	return m_sEmitterDesc.bFixAmount;
}

bool REmitter::IsUVAniBlending()
{
	if( m_pUVModifier )
		return true;

	return false;
}

void REmitter::SetTexUV(const RVector4 &v )
{
	m_sEmitterDesc.vTexUV = v;
}

const RVector4* REmitter::GetTexUV()
{
	return &m_sEmitterDesc.vTexUV;
}

void REmitter::SetTextUVPivot(const RVector2 &v)
{
	m_sEmitterDesc.vTexUVPivot = v;
}

const RVector2 *REmitter::GetTexUVPivot()
{
	return &m_sEmitterDesc.vTexUVPivot;
}

RVector4 REmitter::GetVertexScale()
{
	const RVector2 *pTexPivot = GetTexUVPivot();

	if( m_pUVModifier )
		return m_pUVModifier->GetVertexScale( pTexPivot );

	const RVector4 *pTexUV = GetTexUV();

	// 스케일 노멀라이징.
	// UV 영역 상관없이 일정 크기를 유지.
	// 로직을 바꾸면 기존 이펙트 수정해야 할것이 많아 적용하지 않음.
	//float width	= pTexUV->z - pTexUV->x;
	//if( width < FLT_EPSILON )
	//	width = FLT_EPSILON;
	//
	//float height= pTexUV->w - pTexUV->y;
	//if( height < FLT_EPSILON )
	//	height = FLT_EPSILON;
	//
	//RVector4 vertexScale(
	//(pTexPivot->x - pTexUV->x)/width,
	//(pTexPivot->y - pTexUV->y)/height,
	//(pTexUV->z - pTexPivot->x)/width,
	//(pTexUV->w - pTexPivot->y)/height );

	// UV 영역에 따라 크기가 바뀌는 기존 방식.
	RVector4 vertexScale(
		(pTexPivot->x - pTexUV->x)*2,
		(pTexPivot->y - pTexUV->y)*2,
		(pTexUV->z - pTexPivot->x)*2,
		(pTexUV->w - pTexPivot->y)*2 );

	return vertexScale;
}

PVERTEXFORMAT* REmitter::GetVertexArray(int particleNum, int& outVertexCount)
{
	outVertexCount = particleNum * GetVertexCountPerParticle();
	return REffectManager::GetVertexArray( outVertexCount );
}

PVERTEXFORMAT* REmitter::FillVertex( PVERTEXFORMAT* pVertex, const RVector* pPos, RParticle* pParticle)
{
	RPFC_THISFUNC;
	if( m_pUVModifier )
		return m_pUVModifier->FillVertex( pVertex, pPos, pParticle );
	
	return FillParticleVertexApplyVisibilityResult( pVertex, pPos, GetTexUV(), &RVector4::ZERO, pParticle->m_vColor, 0.f );
}

PVERTEXFORMAT* REmitter::FillParticleVertexApplyVisibilityResult( PVERTEXFORMAT* pVertex, const RVector* pPos,
	const RVector4* pTexUV, const RVector4* pTexUV2, const RVector4& vColor, float fBlend )
{
	RPFC_THISFUNC;

	_ASSERT( pVertex );
	_ASSERT( pPos );
	_ASSERT( pTexUV );
	_ASSERT( pTexUV2 );

	DWORD dwColor = GetVisibilityAppliedColor( vColor, GetVisibilityResult() );

	// 1 : left-top
	pVertex->vPos = pPos[0];//LT;
	pVertex->dwColor = dwColor;
	pVertex->u = pTexUV->x;
	pVertex->v = pTexUV->y;
	pVertex->s = pTexUV2->x;
	pVertex->t = pTexUV2->y;
	pVertex->blend = fBlend;
	++pVertex;

	// 2 : right-top
	pVertex->vPos = pPos[1];//RT;
	pVertex->dwColor = dwColor;
	pVertex->u = pTexUV->z;
	pVertex->v = pTexUV->y;
	pVertex->s = pTexUV2->z;
	pVertex->t = pTexUV2->y;
	pVertex->blend = fBlend;
	++pVertex;

	// 3 : left-bottom
	pVertex->vPos = pPos[2];//LB;
	pVertex->dwColor = dwColor;
	pVertex->u = pTexUV->x;
	pVertex->v = pTexUV->w;
	pVertex->s = pTexUV2->x;
	pVertex->t = pTexUV2->w;
	pVertex->blend = fBlend;
	++pVertex;

	// 4 : left-bottom
	*pVertex = *(pVertex-1);
	++pVertex;

	// 5 : right-top
	*pVertex = *(pVertex-3);
	++pVertex;

	// 6 : right-bottom
	pVertex->vPos = pPos[3];//RB;
	pVertex->dwColor = dwColor;
	pVertex->u = pTexUV->z;
	pVertex->v = pTexUV->w;
	pVertex->s = pTexUV2->z;
	pVertex->t = pTexUV2->w;
	pVertex->blend = fBlend;
	++pVertex;

	return pVertex;
}

int REmitter::GetVertexCountPerParticle()
{
	return 6;
}

std::vector<REmitterVariable>& REmitter::GetEmitterTrack()
{
	return m_sEmitterDesc.vecEmitterVar;
}

std::vector<RParticleVariable>& REmitter::GetParticleTrack()
{
	return m_sEmitterDesc.vecParticleVar;
}

RSceneNode* REmitter::Clone()
{
	REmitter *pNew = new REmitter();
	_ASSERT( pNew != NULL );

	pNew->CopyEffectBaseParameter( this );

	REmitterDescription& newDesc = const_cast<REmitterDescription&> ( *pNew->GetDesc() );
	newDesc = m_sEmitterDesc;

	if( m_pUVModifier )
	{
		REmitterUVAniDesc uvAniDesc;
		m_pUVModifier->GetDesc( uvAniDesc );
		pNew->SetUVModifierAnimation( &uvAniDesc );
	}

	pNew->UpdateResource();

	pNew->Reset();
	pNew->m_CrdL = m_CrdL;
	pNew->SetStartTime( GetStartTime() );
	pNew->m_fMaxParticleScale = m_fMaxParticleScale;

	return pNew;
}

void REmitter::SaveToXML(MXmlElement &element)
{
	using namespace TXML;

	const REmitterDescription &desc = m_sEmitterDesc;

	_SetAttribute(&element,"name", GetNodeName());
	_SetAttribute(&element,"starttime", m_fStartTime);
	_SetAttribute(&element,"type", EmitterTypeToString(desc.eEmitterType));

	MXmlElement *pAttributeElement = new MXmlElement("ATTRIBUTE");
	element.LinkEndChild(pAttributeElement);

	// TODO: 이전 버전에서 _SetElementValue의 버그로 bool 값을 int형으로 저장하는 바람에
	// 호환성 유지를 위해 코드가 지저분해졌다. 반드시 정리하자. - charmke
	_SetElementValue(pAttributeElement,"LIFE", "sec", desc.fEmitterLife);
	_SetElementValue(pAttributeElement,"LOOPING", "int", desc.bLooping ? 1: 0);
	_SetElementValue(pAttributeElement,"SORTING", "int", desc.bSorting ? 1: 0);
	_SetElementValue(pAttributeElement,"ROTATE" , "int", desc.bRotate ? 1: 0);
	_SetElementValue(pAttributeElement,"MINROTATIONRADIAN", "float", desc.fRoatateRad.m_Min);
	_SetElementValue(pAttributeElement,"MAXROTATIONRADIAN", "float", desc.fRoatateRad.m_Max);
	_SetElementValue(pAttributeElement,"RANDCOLOR", "int", desc.bColorRand ? 1: 0);
	_SetElementValue(pAttributeElement,"RANDCOLORRANGE", "int", desc.bColorRandRange ? 1: 0);
	_SetElementValue(pAttributeElement,"RANDVEL", "int", desc.bVelRand ? 1: 0);
	_SetElementValue(pAttributeElement,"GATHER", "int", desc.bGather ? 1: 0);
	_SetElementValue(pAttributeElement,"EXPLOSION", "int", desc.bExplosion ? 1: 0);
	_SetElementValue(pAttributeElement,"WITHME", "int", desc.bFollowParentPosition ? 1: 0);
	_SetElementValue(pAttributeElement,"FOLLOWPARENTROTATION", "int", desc.bFollowParentRotation ? 1: 0);
	_SetElementValue(pAttributeElement,"FIXAMOUNT", "int", desc.bFixAmount ? 1: 0);
	_SetElementValue(pAttributeElement,"DISTORTION", "int", IsDistortion() ? 1: 0);
	_SetElementValue(pAttributeElement,"MINLIFE", "sec", desc.fParticleLife.m_Min);
	_SetElementValue(pAttributeElement,"MAXLIFE", "sec", desc.fParticleLife.m_Max);
	_SetElementValue(pAttributeElement,"SCALE", "mul", desc.fParticleScale);
	_SetElementValue(pAttributeElement,"BILLBOARDTYPE", "type", BillboardTypeToString( desc.eBillboardType));
	_SetElementValue(pAttributeElement,"WORLDVELOCITY", "int", desc.bWorldVeolocity? 1: 0);

	//Distortion Property
	if( IsDistortion() )
		m_DistortionParam.SaveToXML(*pAttributeElement);

	if( GetVisibleSwitch() == false )
	{
		MXmlElement *pVisibleElement = new MXmlElement("INVISIBLE");
		pAttributeElement->LinkEndChild(pVisibleElement);
	}

	// Emitter variable array
	MXmlElement *pEmitterVarElement = new MXmlElement("EMITTERVAR");
	element.LinkEndChild(pEmitterVarElement);
	_SetAttribute(pEmitterVarElement,"type","array");
	_SetAttribute(pEmitterVarElement,"size",(int)desc.vecEmitterVar.size());

	for ( size_t si = 0; si<desc.vecEmitterVar.size(); ++si )
	{
		MXmlElement *pVarElement = new MXmlElement("EMITTERVARIABLE");
		_SetAttribute(pVarElement, "index", (int)si);
		pEmitterVarElement->LinkEndChild(pVarElement);

		const REmitterVariable &ptVar = desc.vecEmitterVar[si];
		_SetElementValue(pVarElement,"EVENTTIME","alpha", ptVar.fEventTime);
		_SetElementValue(pVarElement,"AMOUNT","int", (int)ptVar.uAmount);
		_SetElementValue(pVarElement,"RANGE","vec3", ptVar.vRange);
		_SetElementValue(pVarElement,"POSITION","vec3", ptVar.vPos);
		_SetElementValue(pVarElement,"ANGLE", "degree", ptVar.fAngle);
		_SetElementValue(pVarElement,"MINVELOCITY", "float", ptVar.fMinSpeed);
		_SetElementValue(pVarElement,"MAXVELOCITY", "float", ptVar.fMaxSpeed);
	}

	// <TEXTURE>
	MXmlElement *pTextureElement = new MXmlElement("TEXTURE");
	_SetAttribute(pTextureElement,"name", GetTextureName());
	SaveXMLBlendInfo(pTextureElement);
	_SetElementValue(pTextureElement,"TEXUV" , "vec4", desc.vTexUV);
	_SetElementValue(pTextureElement,"TEXUVPIVOT", "vec2", desc.vTexUVPivot);
	if( m_pUVModifier )
	{
		MXmlElement *pUVAniElement = new MXmlElement("UV_ANI");
		pTextureElement->LinkEndChild(pUVAniElement);
		m_pUVModifier->SaveToXML(*pUVAniElement);
	}
	if( GetSITextureName().length() )
	{
		_SetElementValue(pTextureElement,"SELFILLUMINATION", "string", GetSITextureName().c_str());
		_SetElementValue(pTextureElement,"SELFILLUMINATIONSCALE","float", GetSelfIlluminationScale());
	}
	element.LinkEndChild(pTextureElement);


	//Particle Variable Array
	MXmlElement *pParticleVar = new MXmlElement("VECVAR");
	_SetAttribute(pParticleVar,"type","array");
	_SetAttribute(pParticleVar,"size",(int)desc.vecParticleVar.size());
	element.LinkEndChild(pParticleVar);

	for ( size_t si = 0; si<desc.vecParticleVar.size(); ++si )
	{
		MXmlElement *pVarElement = new MXmlElement("PARTICLEVARIABLE");
		pParticleVar->LinkEndChild(pVarElement);
		_SetAttribute(pVarElement,"index", (int)si);

		const RParticleVariable &ptVar = desc.vecParticleVar[si];

		_SetElementValue(pVarElement,"EVENTTIME","alpha", ptVar.fEventTime);
		_SetElementValue(pVarElement,"COLORRED","alpha", ptVar.vColor.r);
		_SetElementValue(pVarElement,"COLORGREEN","alpha", ptVar.vColor.g);
		_SetElementValue(pVarElement,"COLORBLUE","alpha", ptVar.vColor.b);
		_SetElementValue(pVarElement,"COLORALPHA","alpha", ptVar.vColor.a);
		_SetElementValue(pVarElement,"ADDSCALE","add", ptVar.fAddScale);
		_SetElementValue(pVarElement,"MULSCALE","mul", ptVar.fMulScale);
		_SetElementValue(pVarElement,"INITSCALEX","float", ptVar.fInitScaleX);
		_SetElementValue(pVarElement,"INITSCALEY","float", ptVar.fInitScaleY);
		_SetElementValue(pVarElement,"MAXVEL","vel", ptVar.fMaxSpeed);
		_SetElementValue(pVarElement,"MINVEL","vel", ptVar.fMinSpeed);
		_SetElementValue(pVarElement,"VELOCITY", "vec3", ptVar.vDirection);
		_SetElementValue(pVarElement,"GRATE","grate", ptVar.fGRate);
		_SetElementValue(pVarElement,"SCALEX","scale", ptVar.fScaleX);
		_SetElementValue(pVarElement,"SCALEY","scale", ptVar.fScaleY);
		_SetElementValue(pVarElement,"NOISE","noise", ptVar.fNoise);
	}

	return;
}

bool REmitter::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	using namespace TXML;
	/**
	Attribute 가 없는 구 버전은 고려하지 않았다. ( 7개 정도 존재 )
	*/
//	_ASSERT(pRootElement);

	// pElement 의 이름은 REMITTER 이어야 한다.
	REmitterDescription &desc = m_sEmitterDesc;
	MXmlElement *pAttribute = element.FirstChildElement("ATTRIBUTE");
	if ( pAttribute == NULL )
		return false;

	_Attribute( m_fStartTime, &element, "starttime" );
	desc.eEmitterType = EmitterStringToType(element.Attribute("type"));
	desc.fEmitterLife = _GETElement<float>::Value(pAttribute, "LIFE", "float");
	desc.bLooping = _GETElement<int>::Value(pAttribute,"LOOPING","int") ? true : false;
	desc.bSorting = _GETElement<int>::Value(pAttribute,"SORTING", "int") ? true : false;
	desc.bRotate = _GETElement<int>::Value(pAttribute,"ROTATE", "int") ? true : false;
	bool bDistortion = _GETElement<int>::Value(pAttribute,"DISTORTION","int") ? true : false;
	SetDistortion( bDistortion );

	if ( MXmlElement *pBillboard = pAttribute->FirstChildElement("BILLBOARDTYPE") )
	{
		char pcBuffer[128];
		pcBuffer[0] = '\0';

		_Contents(pcBuffer, pBillboard, "");
		desc.eBillboardType = StringToBillboardType(pcBuffer);
	}
	else
	{
		desc.eBillboardType = BILLBOARD_XYZ;
	}

	desc.fRoatateRad = CMinMax<float>(-0.1f, 0.1f);
	if ( MXmlElement *pMinRotationRad = pAttribute->FirstChildElement("MINROTATIONRADIAN"))
		desc.fRoatateRad.m_Min = _GETElement<float>::Value(pAttribute,"MINROTATIONRADIAN", "float");

	if ( MXmlElement *pMaxRotationRad = pAttribute->FirstChildElement("MAXROTATIONRADIAN"))
		desc.fRoatateRad.m_Max = _GETElement<float>::Value(pAttribute,"MAXROTATIONRADIAN", "float");

	desc.bColorRand = _GETElement<int>::Value(pAttribute,"RANDCOLOR","int") ? true : false;
	desc.bColorRandRange = _GETElement<int>::Value(pAttribute,"RANDCOLORRANGE","int") ? true : false;
	desc.bVelRand = _GETElement<int>::Value(pAttribute,"RANDVEL","int") ? true : false;
	desc.bGather = _GETElement<int>::Value(pAttribute,"GATHER","int") ? true : false;
	desc.bExplosion = _GETElement<int>::Value(pAttribute,"EXPLOSION", "int") ? true : false;
	desc.bFollowParentPosition = _GETElement<int>::Value(pAttribute,"WITHME","int") ? true : false;
	desc.bFollowParentRotation = _GETElement<int>::Value(pAttribute,"FOLLOWPARENTROTATION","int") ? true : false;
	desc.bFixAmount = _GETElement<int>::Value(pAttribute,"FIXAMOUNT","int") ? true : false;
	desc.fParticleLife.m_Min = _GETElement<float>::Value(pAttribute,"MINLIFE","float");
	desc.fParticleLife.m_Max = _GETElement<float>::Value(pAttribute,"MAXLIFE","float");
	desc.fParticleScale = _GETElement<float>::Value(pAttribute,"SCALE","float");
	desc.bWorldVeolocity = _GETElement<int>::Value(pAttribute,"WORLDVELOCITY","int") ? true : false;

	//Distortion Property
	m_DistortionParam.CreateToXML(*pAttribute);

	if( MXmlElement *pVisible = pAttribute->FirstChildElement("INVISIBLE") )
		SetVisibleSwitch( false );

	// emitter variable array
	int nSize = 0;
	MXmlElement *pEmitterVar = element.FirstChildElement("EMITTERVAR");
	_Attribute(&nSize, pEmitterVar, "size");

	desc.vecEmitterVar.clear();
	desc.vecEmitterVar.resize(nSize);

	bool bVelocityInEmitterTrack = true;
	for ( MXmlElement *pVarElement = pEmitterVar->FirstChildElement("EMITTERVARIABLE"); pVarElement != NULL; pVarElement = pVarElement->NextSiblingElement())
	{
		REmitterVariable ptVar;
		int nIndex = 0;
		_Attribute(&nIndex, pVarElement, "index");

		ptVar.fEventTime = _GETElement<float>::Value(pVarElement,"EVENTTIME","float");
		ptVar.uAmount = _GETElement<float>::Value(pVarElement,"AMOUNT","int");
		_GetElementValue(&ptVar.vRange, pVarElement, "RANGE", "vec3");
		_GetElementValue(&ptVar.vPos, pVarElement, "POSITION", "vec3");
		ptVar.fAngle = _GETElement<float>::Value(pVarElement,"ANGLE","float");

		MXmlElement *pMinVelocity = pVarElement->FirstChildElement("MINVELOCITY");
		MXmlElement *pMaxVelocity = pVarElement->FirstChildElement("MAXVELOCITY");
		if ( pMinVelocity == NULL 
			|| pMaxVelocity == NULL )
		{
			bVelocityInEmitterTrack = false;
		}
		else
		{
			ptVar.fMinSpeed = _GETElement<float>::Value(pVarElement,"MINVELOCITY","float");
			ptVar.fMaxSpeed = _GETElement<float>::Value(pVarElement,"MAXVELOCITY","float");
		}

		desc.vecEmitterVar[nIndex] = ptVar;
	}

	// <TEXTURE>
	if ( MXmlElement *pTextureElement = element.FirstChildElement("TEXTURE"))
	{
		char pcBuffer[128];
		pcBuffer[0] = '\0';
		_Attribute(pcBuffer, pTextureElement, "name");
		SetTextureName( pcBuffer );

		LoadXMLBlendInfo( pTextureElement );

		_GetElementValue(&desc.vTexUV, pTextureElement, "TEXUV", "vec4");
		if ( !_GetElementValue(&desc.vTexUVPivot, pTextureElement, "TEXUVPIVOT","vec2"))
		{
			// 없을 때 center 로 지정
			desc.vTexUVPivot.x = ( desc.vTexUV.x + desc.vTexUV.z) * .5f;
			desc.vTexUVPivot.y = ( desc.vTexUV.y + desc.vTexUV.w) * .5f;
		}
		
		if ( MXmlElement *pUVAniElement = pTextureElement->FirstChildElement("UV_ANI"))
		{
			SetUVModifierAnimation();
			m_pUVModifier->CreateFromXML(*pUVAniElement);
		}
		else
		{
			SetUVModifierDefault();
		}

		if ( _Contents(pcBuffer, pTextureElement->FirstChildElement("SELFILLUMINATION")))
		{
			SetSITextureName( pcBuffer );
		}

		if ( pTextureElement->FirstChildElement("SELFILLUMINATIONSCALE"))
		{
			float fSelfIlluminationScale = _GETElement<float>::Value(pTextureElement,"SELFILLUMINATIONSCALE", "float");
			SetSelfIlluminationScale( fSelfIlluminationScale );
		}
	}

	// particle variable array
	MXmlElement *pParticleVar = element.FirstChildElement("VECVAR");
	_Attribute(&nSize, pParticleVar, "size");

	desc.vecParticleVar.clear();
	desc.vecParticleVar.resize(nSize);

	for ( MXmlElement *pVarElement = pParticleVar->FirstChildElement("PARTICLEVARIABLE"); pVarElement != NULL; pVarElement = pVarElement->NextSiblingElement())
	{
		RParticleVariable ptVar;
		int nIndex = 0;
		_Attribute(&nIndex, pVarElement, "index");

		ptVar.fEventTime = _GETElement<float>::Value(pVarElement, "EVENTTIME", "float");
		ptVar.vColor.r = _GETElement<float>::Value(pVarElement,"COLORRED" ,"float");
		ptVar.vColor.g = _GETElement<float>::Value(pVarElement,"COLORGREEN" ,"float");
		ptVar.vColor.b = _GETElement<float>::Value(pVarElement,"COLORBLUE" ,"float");
		ptVar.vColor.a = _GETElement<float>::Value(pVarElement,"COLORALPHA" ,"float");
		ptVar.fAddScale = _GETElement<float>::Value(pVarElement,"ADDSCALE" ,"float");
		ptVar.fMulScale = _GETElement<float>::Value(pVarElement,"MULSCALE" ,"float");
		if ( pVarElement->FirstChildElement("INITSCALE"))
		{
			float fInitScale = _GETElement<float>::Value(pVarElement,"INITSCALE","float");
			ptVar.fInitScaleX = fInitScale;
			ptVar.fInitScaleY = fInitScale;
		}
		else
		{
			ptVar.fInitScaleX = _GETElement<float>::Value(pVarElement,"INITSCALEX" ,"float");
			ptVar.fInitScaleY = _GETElement<float>::Value(pVarElement,"INITSCALEY" ,"float");
		}
		ptVar.fMaxSpeed = _GETElement<float>::Value(pVarElement,"MAXVEL" ,"float");
		ptVar.fMinSpeed = _GETElement<float>::Value(pVarElement,"MINVEL" ,"float");
		_GetElementValue(&ptVar.vDirection, pVarElement, "VELOCITY", "vec3");
		ptVar.fGRate = _GETElement<float>::Value(pVarElement,"GRATE","float");
		if ( pVarElement->FirstChildElement("SCALE"))
		{
			float fScale = _GETElement<float>::Value(pVarElement,"SCALE","float");
			ptVar.fScaleX = fScale;
			ptVar.fScaleY = fScale;
			ptVar.fNoise = 0.f;
		}
		else if ( pVarElement->FirstChildElement("SCALEMAX"))
		{
			float fScale = _GETElement<float>::Value(pVarElement,"SCALEMAX","float");
			float fScaleMin = _GETElement<float>::Value(pVarElement,"SCALEMIN","float");
			ptVar.fScaleX = fScale;
			ptVar.fScaleY = fScale;

			ptVar.fNoise = fScale - fScaleMin;
		}
		else if ( pVarElement->FirstChildElement("SCALEXMAX"))
		{
			ptVar.fScaleX = _GETElement<float>::Value(pVarElement,"SCALEXMAX","float");
			ptVar.fScaleY = _GETElement<float>::Value(pVarElement,"SCALEYMAX","float");

			float fScaleMin = _GETElement<float>::Value(pVarElement,"SCALEXMIN","float");

			ptVar.fNoise = ptVar.fScaleX - fScaleMin;
		}
		else
		{
			ptVar.fScaleX = _GETElement<float>::Value(pVarElement,"SCALEX","float");
			ptVar.fScaleY = _GETElement<float>::Value(pVarElement,"SCALEY","float");
			ptVar.fNoise = _GETElement<float>::Value(pVarElement,"NOISE","float");
		}

		desc.vecParticleVar[nIndex] = ptVar;
	}

	if ( bVelocityInEmitterTrack == false )
	{
		const float fMinSpeed = desc.vecParticleVar[0].fMinSpeed;
		const float fMaxSpeed = desc.vecParticleVar[0].fMaxSpeed;

		for ( size_t si = 0; si<desc.vecEmitterVar.size(); ++si )
		{
			desc.vecEmitterVar[si].fMinSpeed = fMinSpeed;
			desc.vecEmitterVar[si].fMaxSpeed = fMaxSpeed;
		}
	}

	LoadAllTexture(bBackgroundCreation);
	m_DistortionParam.LoadNormalMap(bBackgroundCreation);

	UpdateResource();

	Init();

	return true;
}


void REmitter::SetEmitterType(EMITTER_TYPE eType)
{
	SAFE_DELETE(m_pParticleCreator);
	switch ( eType )
	{
	case EMITTER_SPHERE:
		m_pParticleCreator = new RSphereEmit;
		break;
	case EMITTER_BOX:
		m_pParticleCreator = new RBoxEmit;
		break;
	case EMITTER_RING:
		m_pParticleCreator = new RRingEmit;
		break;
	};

	m_sEmitterDesc.eEmitterType = eType;
}

void REmitter::GenerateMovingTable()
{
	const REmitterDescription *pDesc = GetDesc();
	m_CrdL.Generate(pDesc->bLooping, pDesc->vecEmitterVar, pDesc->fEmitterLife);
}

bool REmitter::IsEmitterLifeValid()
{
	return ( GetDesc()->fEmitterLife > FLT_EPSILON );
}

void REmitter::Init()
{
	UpdateEmitterVar();
	UpdateParticleVar();
	Reset();
}

DWORD REmitter::GetVisibilityAppliedColor( const RVector4& vColor, float fVisibility )
{
	_ASSERT( vColor.r >= 0.f && vColor.r <= 1.f );
	_ASSERT( vColor.g >= 0.f && vColor.g <= 1.f );
	_ASSERT( vColor.b >= 0.f && vColor.b <= 1.f );
	_ASSERT( vColor.a >= 0.f && vColor.a <= 1.f );

	RVector4 vVisibilityAppliedColor = vColor;
	vVisibilityAppliedColor.a *= fVisibility;
	return RVector4ToDWORD( vVisibilityAppliedColor );
}

}