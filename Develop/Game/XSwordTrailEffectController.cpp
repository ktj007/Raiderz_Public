#include "stdafx.h"
#include "XSwordTrailEffectController.h"
#include "XSystem.h"
#include "XCharacter.h"
#include "XModuleEntity.h"
#include "XGameTransformControllerManager.h"

MImplementRTTI(XSwordTrailEffectController, XObject);

//////////////////////////////////////////////////////////////////////////
// XSwordTrailEffectController
XSwordTrailEffectController::XSwordTrailEffectController(MUID uid) : XObject(uid)
{
	m_fMaxFrameSec		= 0.0f;
	m_fDisappearTime	= 0.0f;
	m_dwFadeValue		= 255;
	m_nMaxFrame			= 0;
	m_nFrameGapControl	= 0;
	m_nStartFrame		= 0;
	m_nEndFrame			= 0;
	
	m_bShow				= false;
	m_bFadeHide			= false;

	m_uidUser			= MUID::ZERO;
}

XSwordTrailEffectController::~XSwordTrailEffectController()
{
	DestroySwordTrail();
}

void XSwordTrailEffectController::CreateSwordTrail( MUID uidUser, SWORDTRAILSAMPLING_DATA* pSamplingData, wstring strSwordTrailTexture )
{
	// uid가 없다면... 존재 의미가 없다. 지운다.
	XObject* pObject = gg.omgr->FindObject(uidUser);
	if(pObject == NULL)
	{
		DeleteMe();
		return;
	}

	Create();

	m_uidUser			= uidUser;

	m_nStartFrame		= pSamplingData->nStartFrame;
	m_nEndFrame			= pSamplingData->nEndFrame;
	m_nFrameGapControl	= pSamplingData->nFrameGap;
	m_nMaxFrame			= m_nEndFrame - m_nStartFrame;
	m_fMaxFrameSec		= pObject->GetActor()->GetFrameToSec(m_nMaxFrame) + 0.2f;

	if(m_SwordTrailEffect.CreateSwordTrailEffect(pObject->GetActor(), pSamplingData, strSwordTrailTexture) == false)
	{
		DeleteMe();
		return;
	}
}

void XSwordTrailEffectController::DestroySwordTrail()
{
	m_SwordTrailEffect.DestroySwordTrailEffect();
}

void XSwordTrailEffectController::UpdateAppearanceFadeHide( float fDelta )
{
	if(m_fDisappearTime > 0.0f)
	{
		m_fDisappearTime -= fDelta;
		float fFadeValue = m_fDisappearTime / m_fMaxFrameSec;

		if(fFadeValue < 0.0f)
		{
			fFadeValue			= 0.0f;
			m_fDisappearTime	= 0.0f;
			m_bFadeHide			= false;
			DeleteMe();
		}

		// Fade 계산
		m_dwFadeValue = 255 * fFadeValue;
	}
}

void XSwordTrailEffectController::OnUpdate( float fDelta )
{
	XObject* pObject = gg.omgr->FindObject(m_uidUser);
	if(pObject == NULL ||
		pObject->GetActor() == NULL)
		return;

	XModuleEntity* pModuleEntity= pObject->GetModuleEntity();
	if(pModuleEntity == NULL)
		return;

	int nCurrentFrame = pModuleEntity->GetAnimationController()->GetAniFrame();
	int nVertexCount = 0;

	if(nCurrentFrame < m_nStartFrame)
		return;

	if(nCurrentFrame > m_nStartFrame &&
		nCurrentFrame <= m_nEndFrame)
	{
		if(m_bShow == false)
		{
			// 검광 보여주는 프레임 시간이다.
			m_fDisappearTime = m_fMaxFrameSec + 0.1f;
			m_bShow = true;
		}

		m_SwordTrailEffect.CalSwordTrailEffectVertexCount(nCurrentFrame, m_nStartFrame, m_nEndFrame, m_nFrameGapControl, nVertexCount);
	}
	else
	{
		nVertexCount = m_SwordTrailEffect.GetSamplingCount();
	}

	m_SwordTrailEffect.UpdateVertex(nVertexCount, pObject->GetWorldTransform());

	UpdateAppearanceFadeHide(fDelta);
}

void XSwordTrailEffectController::OnRender()
{
	if(m_bShow || m_bFadeHide)
	{
		m_SwordTrailEffect.RenderSwordTrailEffect(m_dwFadeValue);
	}
}

void XSwordTrailEffectController::DoneSwordTrail()
{
	m_bShow		= false;
	m_bFadeHide	= false;

	DeleteMe();
}
