#include "StdAfx.h"
#include "RPostEffectInterface.h"
#include "RPostEffectManager.h"
#include "RPostEffectAccumMotionBlur.h"
#include "RPostEffectDOF.h"
#include "RPostEffectHDR.h"
#include "RPostEffectPixelMotionBlur.h"
#include "RPostEffectScreenMotionBlur.h"
#include "RPostEffectDistortion.h"
#include "RShaderCompositeManager.h"
#include "RPostEffectEdge.h"
#include "RPostEffectFinalScene.h"

#pragma warning(disable:4800)


namespace rs3 {


RPostEffectManager::RPostEffectManager(void)
{
	m_dwW = m_dwH = 0;
	m_nBufferIdx = 0;
	
	m_pBufferOrg = NULL;
	m_pCurrentTarget = NULL;
	m_pBufferForHDR = NULL;
	m_pBuffer[0] = NULL;
	m_pBuffer[1] = NULL;
	m_pBuffer[2] = NULL;
	m_pBuffer[3] = NULL;

	SetEnable(PE_VOID, true);
	SetEnable(PE_FINAL_SCENE, true);
}

RPostEffectManager::~RPostEffectManager(void)
{
	Destroy();

	for(POST_EFFECT_MAP::iterator itr = m_mapPostEffect.begin(); itr != m_mapPostEffect.end(); ++itr)
	{
		RPostEffectInterface* pPoseEffect = itr->second;
		if( pPoseEffect)
		{
			SAFE_DELETE( pPoseEffect);
		}

	}
	m_mapPostEffect.clear();
}

bool RPostEffectManager::Init(  int nW, int nH )
{
	m_dwW = nW;
	m_dwH = nH;

	if( m_dwW <= 0 || m_dwH <= 0)
		return false;

	bool bOK = true;

	bOK = CreateBuffers();

	for(POST_EFFECT_MAP::iterator itr = m_mapPostEffect.begin(); itr != m_mapPostEffect.end(); ++itr)
	{
		RPostEffectInterface* pPoseEffect = itr->second;
		if( pPoseEffect)
		{
			if(!pPoseEffect->Init( nW, nH))
				bOK = false;
		}
	}

	return bOK;
}

bool RPostEffectManager::CreateBuffers()
{
	bool bOK = true;

	RFORMAT eBufferFormat = RFMT_A8R8G8B8;
	if(!m_pBuffer[0])
		m_pBuffer[0] = REngine::GetDevice().CreateRenderTargetTexture( m_dwW, m_dwH, eBufferFormat);
	if(R_NONE==m_pBuffer[0]) bOK = false;

	if(!m_pBuffer[1])
		m_pBuffer[1] = REngine::GetDevice().CreateRenderTargetTexture( m_dwW, m_dwH, eBufferFormat);
	if(R_NONE==m_pBuffer[1]) bOK = false;


	eBufferFormat = RFMT_A16B16G16R16F;
	if(!m_pBuffer[2])
		m_pBuffer[2] = REngine::GetDevice().CreateRenderTargetTexture( m_dwW, m_dwH, eBufferFormat);
	if(R_NONE==m_pBuffer[2]) bOK = false;

	if(!m_pBuffer[3])
		m_pBuffer[3] = REngine::GetDevice().CreateRenderTargetTexture( m_dwW, m_dwH, eBufferFormat);
	if(R_NONE==m_pBuffer[3]) bOK = false;


	// HDR 여부를 체크하여야 한다. 우선은 무조건..
	RFORMAT eBufferFormatForHDR = RFMT_A16B16G16R16F;
	if(!m_pBufferForHDR)
		m_pBufferForHDR = REngine::GetDevice().CreateRenderTargetTexture( m_dwW, m_dwH, eBufferFormatForHDR);
	if(R_NONE==m_pBufferForHDR) bOK = false;

	return bOK;
}

void RPostEffectManager::DestroyBuffers()
{
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pBuffer[0] );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pBuffer[1] );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pBuffer[2] );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pBuffer[3] );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pBufferForHDR );
}

bool RPostEffectManager::Resize( int nW, int nH )
{
	m_dwW = nW;
	m_dwH = nH;

	if( m_dwW <= 0 || m_dwH <= 0)
		return false;

	bool bOK = true;

	DestroyBuffers();
	bOK = CreateBuffers();

	for(POST_EFFECT_MAP::iterator itr = m_mapPostEffect.begin(); itr != m_mapPostEffect.end(); ++itr)
	{
		RPostEffectInterface* pPoseEffect = itr->second;
		if( pPoseEffect)
		{
			if(!pPoseEffect->Resize( nW, nH))
				bOK = false;
		}
	}

	return bOK;
}


void RPostEffectManager::PEEdgeSet( bool _bProjection )
{
	POST_EFFECT_MAP::iterator itr = m_mapPostEffect.find( PE_EDGE );
	if( itr !=  m_mapPostEffect.end() )
	{
		RPostEffectEdge* pPEEdge = dynamic_cast<RPostEffectEdge*>((itr->second));
		if( pPEEdge )
			pPEEdge->EnablePEEdge( _bProjection );
	}
}

void RPostEffectManager::PEEdgeQuality(int _iBlurCount, int _iScreenSize)
{
	POST_EFFECT_MAP::iterator itr = m_mapPostEffect.find( PE_EDGE );
	if( itr !=  m_mapPostEffect.end() )
	{
		RPostEffectEdge* pPEEdge = dynamic_cast<RPostEffectEdge*>((itr->second));
		if( pPEEdge )
			pPEEdge->PEEdgeQuality(_iBlurCount, _iScreenSize);
	}
}


void RPostEffectManager::PEFactorTickNess(const float _fTickness, const float _fBlutFactor, const float _fColorPower)
{
	POST_EFFECT_MAP::iterator itr = m_mapPostEffect.find( PE_EDGE );
	if( itr !=  m_mapPostEffect.end() )
	{
		RPostEffectEdge* pPEEdge = dynamic_cast<RPostEffectEdge*>((itr->second));
		if( pPEEdge )
			pPEEdge->SetFactorTickNess(_fTickness, _fBlutFactor, _fColorPower);
	}
}


void RPostEffectManager::PEEdgeEnable( bool _bEdge )
{
	POST_EFFECT_MAP::iterator itr = m_mapPostEffect.find( PE_EDGE );
	if( itr !=  m_mapPostEffect.end() )
	{
		RPostEffectEdge* pPEEdge = dynamic_cast<RPostEffectEdge*>((itr->second));
		if( pPEEdge )
			pPEEdge->DisablePEEdge();
	}
}


void RPostEffectManager::AddPEEdgeCount()
{
	POST_EFFECT_MAP::iterator itr = m_mapPostEffect.find( PE_EDGE );
	if( itr !=  m_mapPostEffect.end() )
	{
		RPostEffectEdge* pPEEdge = dynamic_cast<RPostEffectEdge*>((itr->second));
		if( pPEEdge )
			pPEEdge->AddCount();
	}
}


void RPostEffectManager::MinusPEEdgeCount()
{
	POST_EFFECT_MAP::iterator itr = m_mapPostEffect.find( PE_EDGE );
	if( itr !=  m_mapPostEffect.end() )
	{
		RPostEffectEdge* pPEEdge = dynamic_cast<RPostEffectEdge*>((itr->second));
		if( pPEEdge )
			pPEEdge->MinusCount();
	}
}

void RPostEffectManager::Destroy()
{
	DestroyBuffers();

	for(POST_EFFECT_MAP::iterator itr = m_mapPostEffect.begin(); itr != m_mapPostEffect.end(); ++itr)
	{
		RPostEffectInterface* pPoseEffect = itr->second;
		if( pPoseEffect)
		{
			pPoseEffect->Destroy();
		}
	}
}

void RPostEffectManager::Invalidate()
{
	DestroyBuffers();

	for(POST_EFFECT_MAP::iterator itr = m_mapPostEffect.begin(); itr != m_mapPostEffect.end(); ++itr)
	{
		RPostEffectInterface* pPoseEffect = itr->second;
		if( pPoseEffect)
		{
			pPoseEffect->Invalidate();
		}
	}
}

void RPostEffectManager::Restore()
{
	CreateBuffers();

	_ASSERT( m_pBuffer[0] );
	_ASSERT( m_pBuffer[1] );
	_ASSERT( m_pBuffer[2] );
	_ASSERT( m_pBuffer[3] );
	_ASSERT( m_pBufferForHDR );

	if( m_pBuffer[0] )
		REngine::GetDevice().ColorFill( m_pBuffer[0] );

	if( m_pBuffer[1] )
		REngine::GetDevice().ColorFill( m_pBuffer[1] );

	if( m_pBuffer[2] )
		REngine::GetDevice().ColorFill( m_pBuffer[2] );

	if( m_pBuffer[3] )
		REngine::GetDevice().ColorFill( m_pBuffer[3] );

	if( m_pBufferForHDR )
		REngine::GetDevice().ColorFill( m_pBufferForHDR);

	for(POST_EFFECT_MAP::iterator itr = m_mapPostEffect.begin(); itr != m_mapPostEffect.end(); ++itr)
	{
		RPostEffectInterface* pPoseEffect = itr->second;
		if( pPoseEffect)
		{
			pPoseEffect->Restore();
		}
	}

	m_nBufferIdx = 0;
}


RPostEffectInterface* RPostEffectManager::CreateInstance(POST_EFFECT_TYPE eType)
{
	_ASSERT( eType < PE_COUNT );

	RPostEffectInterface* pInterface = NULL;

	switch( eType)
	{
	case PE_HDR:				pInterface = new RPostEffectHDR();	break;
	case PE_VOID:				pInterface = new RPostEffectVoid();	break;
	case PE_EDGE:				pInterface = new RPostEffectEdge();	break;
	case PE_DISTORTION:			pInterface = new RPostEffectDistortion();	break;
	case PE_ACCUM_MOTION_BLUR:	pInterface = new RPostEffectAccumMotionBlur();	break;
	case PE_DOF:				pInterface = new RPostEffectDOF();	break;
	case PE_PIXEL_MOTION_BLUR:	pInterface = new RPostEffectPixelMotionBlur();	break;
	case PE_SCREEN_MOTION_BLUR:	pInterface = new RPostEffectScreenMotionBlur();	break;
	case PE_FINAL_SCENE:		pInterface = new RPostEffectFinalScene();	break;
	}
	if( pInterface && m_dwW > 0 && m_dwH > 0)
		pInterface->Init( m_dwW, m_dwH);

	return pInterface;
}

RPostEffectEnumerator RPostEffectManager::GetPostEffectEnumerator()
{
	return RPostEffectEnumerator( m_mapPostEffect );
}

void RPostEffectManager::SetEnable( POST_EFFECT_TYPE eType, bool bEnable )
{
	_ASSERT(eType < PE_COUNT);

	if(bEnable)
	{
		Add(eType);
	}
	else
	{
		Remove(eType);
	}
}

bool RPostEffectManager::IsEnable(POST_EFFECT_TYPE eType)
{
	_ASSERT(eType < PE_COUNT);

	return ( NULL != Get( eType ) );
}

bool RPostEffectManager::GetHDRBrightPassThreshold( RVector& vOutValue )
{
	if( Get(PE_HDR) )
	{
		vOutValue = ((RPostEffectHDR*)Get(PE_HDR))->GetBrightPassThreshold();
		return true;
	}

	return false;
}

bool RPostEffectManager::SetHDRBrightPassThreshold( RVector vValue )
{
	if( Get(PE_HDR) )
	{
		((RPostEffectHDR*)Get(PE_HDR))->SetBrightPassThreshold( vValue );
		return true;
	}

	return false;
}

void RPostEffectManager::Add(POST_EFFECT_TYPE eType)
{
	_ASSERT( eType < PE_COUNT );

	POST_EFFECT_MAP::iterator itr = m_mapPostEffect.find(eType);
	if( itr == m_mapPostEffect.end() )
	{
		RPostEffectInterface* pInterface = CreateInstance(eType);
		if(pInterface)
			m_mapPostEffect.insert( POST_EFFECT_MAP::value_type( eType, pInterface));
	}
}


void RPostEffectManager::Remove(POST_EFFECT_TYPE eType)
{
	_ASSERT( eType < PE_COUNT );

	POST_EFFECT_MAP::iterator itr = m_mapPostEffect.find(eType);
	if( itr != m_mapPostEffect.end() )
	{
		RPostEffectInterface* pPostEffect = itr->second;
		pPostEffect->Destroy();
		SAFE_DELETE(pPostEffect);

		m_mapPostEffect.erase(itr);		
	}
}

RPostEffectInterface* RPostEffectManager::Get(POST_EFFECT_TYPE eType)
{
	_ASSERT( eType < PE_COUNT );

	POST_EFFECT_MAP::iterator itr = m_mapPostEffect.find( eType );
	if ( itr != m_mapPostEffect.end() )
		return itr->second;
	else
		return NULL;
}

RPostEffectInterface* RPostEffectManager::GetFirstActivePostEffect()
{
	for( POST_EFFECT_MAP::iterator itr = m_mapPostEffect.begin(); itr != m_mapPostEffect.end(); ++itr )
	{
		RPostEffectInterface* pPostEffect = itr->second;
		_ASSERT( pPostEffect );
		if( pPostEffect && pPostEffect->IsActive() )
		{
			return pPostEffect;
		}
	}

	return NULL;
}

RPostEffectInterface* RPostEffectManager::GetLastActivePostEffect()
{
	for( POST_EFFECT_MAP::reverse_iterator itr = m_mapPostEffect.rbegin(); itr != m_mapPostEffect.rend(); ++itr )
	{
		RPostEffectInterface* pPostEffect = itr->second;
		_ASSERT( pPostEffect );
		if( pPostEffect && pPostEffect->IsActive() )
		{
			return pPostEffect;
		}
	}

	return NULL;
}

void RPostEffectManager::PreRender( RCameraSceneNode* pCamera, bool bDepthClear)
{
	RPFC_THISFUNC;
	RPostEffectInterface* pFirstActivePostEffect = GetFirstActivePostEffect();
	if( NULL == pFirstActivePostEffect )
		return;

	// 후처리 효과용 랜더 타겟을 설정
	if( pFirstActivePostEffect->GetID() == PE_HDR)
		m_pCurrentTarget = m_pBufferForHDR;
	else
		m_pCurrentTarget = m_pBuffer[(int)m_nBufferIdx + ( 2 * REngine::GetSceneManager().GetRenderingStrategy()->GetGammaCorrect() ) ];
	
	m_pBufferOrg = REngine::GetDevice().SetRenderTarget( 0, m_pCurrentTarget);
	REngine::GetDevice().Clear( true, bDepthClear, false, pCamera->GetClearColor() );
}

void RPostEffectManager::RenderPost( RCameraSceneNode* pCamera, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RPFC_THISFUNC;

	if( m_mapPostEffect.empty())
		return;

	RPostEffectInterface* pLastActivePostEffect = GetLastActivePostEffect();
	_ASSERT( pLastActivePostEffect );
	for(POST_EFFECT_MAP::iterator itr = m_mapPostEffect.begin(); itr != m_mapPostEffect.end(); ++itr)
	{
		RPostEffectInterface* pPostEffect = itr->second;
		_ASSERT( pPostEffect );
		if( pPostEffect && pPostEffect->IsActive() )
		{
			// 컬러 버퍼가 들어온 타겟이였던 것을 입력 소스로
			RTexture* pSrc = m_pCurrentTarget;
			// 마지막 후처리면 백업했던 버퍼를 최종 타겟으로
			if( pPostEffect == pLastActivePostEffect )
			{
				m_pCurrentTarget = m_pBufferOrg;
				pPostEffect->SetLastPostEffect( true );
			}
			else
			{
				// 소스 입력용으로 설정 해 놓은 랜더타겟 인덱스 스왑. 우선은 상관 없으므로 HDR 여부와 상관 없이 스왑
				m_nBufferIdx = (bool)(1-m_nBufferIdx);
				m_pCurrentTarget = m_pBuffer[(int)m_nBufferIdx + ( 2 * REngine::GetSceneManager().GetRenderingStrategy()->GetGammaCorrect() ) ];
				pPostEffect->SetLastPostEffect( false );
			}
			// 타겟 설정
			REngine::GetDevice().SetRenderTarget( 0, m_pCurrentTarget );
			// 랜더
			pPostEffect->Render( pCamera, pSrc, pNormalBuffer, pDepthBuffer);
		}
	}

	m_nBufferIdx = 0;
}


}
