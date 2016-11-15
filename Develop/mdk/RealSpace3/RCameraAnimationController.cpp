#include "StdAfx.h"

#include "RCameraAnimationController.h"
#include "RCameraSceneNode.h"

#include "RAnimation.h"
#include "RAnimationNode.h"
#include "RAnimationManager.h"

namespace rs3 {

RCameraAnimationController::RCameraAnimationController( const char* _pSzControllerID, const char* pSzAniFileName, bool bManagedByCamera, bool bResetToOrg )
: RCameraSceneNodeController( RCCT_PREUPDATE, _pSzControllerID, bManagedByCamera)
, m_fSpeed(1.f), m_nMaxFrame(0), m_nFrame(0)
, m_nRotKeyIterator(0), m_nPosKeyIterator(0)
, m_bCurrentRunning(false)
, m_bOncePlayed(false)
{
	m_matCurrent.MakeIdentity();
	m_bResetToOrg = bResetToOrg;

// 	m_pAnimation = new RAnimation;
// 	if( m_pAnimation->LoadAni( pSzAniFileName ) )

	m_pAnimation = REngine::GetSceneManager().CreateResource < RAnimationResource > ( pSzAniFileName );
	if( m_pAnimation->BuildAll(false) )
	{
		m_nMaxFrame = m_pAnimation->GetMaxFrame();
		m_bLoop = false;
	}
	else
	{
		REngine::GetSceneManager().ReleaseResource( m_pAnimation );
//		delete m_pAnimation;
		m_pAnimation = NULL;
	}

	m_fDurationTime = 0.f;
	m_fElapsedTime = 0.f;
}

RCameraAnimationController::~RCameraAnimationController()
{
	if(m_pAnimation)
	{
		REngine::GetSceneManager().ReleaseResource( m_pAnimation );
		m_pAnimation = NULL;
	}
//	SAFE_DELETE( m_pAnimation );
}

void RCameraAnimationController::UpdateCameraController(RMatrix& _rMatApplyToCamera)
{
	if(m_pAnimation == NULL )
	{
		ReserveToRemove();
		return;
	}

	DWORD nElapsedTime = REngine::GetTimer().GetElapsedTime();

	// Duration 적용하기
	if (m_fDurationTime > FLT_EPSILON)
	{
		m_fElapsedTime += (float)nElapsedTime/1000.f;
		if (m_fElapsedTime >= m_fDurationTime)
		{
			ReserveToRemove();
			return;
		}
	}

	// 프레임 변경 / 확인 / 업데이트 여부 결정
	bool bRetUpdate(true);
	m_nFrame += DWORD(4.8f*m_fSpeed*nElapsedTime);
	if(m_nFrame >= m_nMaxFrame)
	{
		m_bOncePlayed = true;

		if(m_bLoop)
		{
			m_nFrame = m_nFrame % m_nMaxFrame;
		}
		else
		{
			bRetUpdate = false;
		}
	}

	if ( bRetUpdate )
	{
		RAnimationNode* pAniNode = m_pAnimation->GetAniNode(0);

		if( pAniNode->GetRotValue(&m_currentRotation,m_nFrame,m_nRotKeyIterator) )
			m_matCurrent = m_currentRotation.Conv2Matrix();

		if( pAniNode->GetPosValue(&m_currentTranslation, m_nFrame,m_nPosKeyIterator) )
		{
			m_matCurrent._41 = m_currentTranslation.x;
			m_matCurrent._42 = m_currentTranslation.y;
			m_matCurrent._43 = m_currentTranslation.z;
		}
		_rMatApplyToCamera = m_matCurrent * _rMatApplyToCamera;
	}
	else
	{
		ReserveToRemove();
	}
}

void RCameraAnimationController::OnInsertToCamera( RCameraSceneNodeController* _pBeforeController )
{
	#ifdef _DEBUG
	if ( _pBeforeController != NULL )
		_ASSERT( _pBeforeController->IsManagedByCamera() == false );
	#endif

	m_bCurrentRunning = true;
	m_nFrame = 0;
	m_nRotKeyIterator = 0;
	m_nPosKeyIterator = 0;
	m_bRemoveReserved = false;

	m_matCurrent.MakeIdentity();

	if( m_bResetToOrg )
		m_matCameraOrg = m_pCameraSceneNode->GetLocalTransform();
}

void RCameraAnimationController::OnRemoveFromCamera()
{
	m_bCurrentRunning = false;
	if (m_bResetToOrg)
		m_pCameraSceneNode->SetTransform( m_matCameraOrg );
}

bool RCameraAnimationController::IsBlendableBeforeController()
{
	if (m_bManagedByCamera)
		return false;
	else
		return true;
}

}