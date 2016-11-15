#include "StdAfx.h"
#include "XUpperAnimationTC.h"
#include "RAnimation.h"
#include "MLocale.h"

#define UPPER_ANIMATION_BONE_NAME			"Bip01 Spine1"
#define UPPER_ANIMATION_START_BLEND_TIME	0.0f
#define UPPER_ANIMATION_END_BLEND_TIME		0.15f

XUpperAnimationTC::XUpperAnimationTC( XCharacter* pCharacter )
:XTransformController(pCharacter)
, m_bUseUpperAnimation(false)
, m_fCurrentElapsed(0.0f)
, m_bEndUpperAnimation(false)
{
	if(m_pActor == NULL)
		return;

	m_pRAnimationController = new XRAnimationController(RSNCTRL_CUSTOMUPDATE, RSNCTRL_CUSTOMUPDATE);
	m_pActor->AddController(m_pRAnimationController);
}

XUpperAnimationTC::~XUpperAnimationTC( void )
{
	SAFE_DELETE(m_pRAnimationController);
}

void XUpperAnimationTC::Init()
{
	m_strLastUpperAni.clear();
	m_bUseUpperAnimation = false;

	//m_pRAnimationController->Stop();
	m_pRAnimationController->Reset();
}

bool XUpperAnimationTC::SetAnimation( tstring& strAniName )
{
	RActorNodeSelection nodeSelection;
	if(nodeSelection.SelectNodeAndChildren(m_pActor, UPPER_ANIMATION_BONE_NAME, true) == false)
		return false;

	m_fCurrentElapsed = 0.0f;

	RPLAYINFO playInfo;
	playInfo.blendMode = RBLENDMODE_BLEND;
	playInfo.pNodeSet = &nodeSelection;
	//playInfo.fBlendIn = UPPER_ANIMATION_START_BLEND_TIME;
	//playInfo.fBlendOut = UPPER_ANIMATION_END_BLEND_TIME;

	m_bUseUpperAnimation = m_pRAnimationController->SetAnimation( MLocale::ConvTCHARToAnsi(strAniName.c_str()).c_str(), &playInfo );
	if(m_bUseUpperAnimation)
	{
		m_pRAnimationController->Play();

		m_strLastUpperAni = strAniName;

		//mlog("상체 애니메이션 %s\n", strAniName.c_str());
		return true;
	}

	return false;
}

void XUpperAnimationTC::Update( float fDelta )
{
	if(m_bUseUpperAnimation == false)
		return;

	if(IsLoop() == false)
	{
		// 종료 블렌딩은 루프가 아닌경우에만 한다.
		// 이것이 좀더 이쁘게 나온다.
		float fMaxTime = GetMaxFrameToSecond();
		float fTime = GetAniFrameToSecond();
		if((fMaxTime - fTime) <= UPPER_ANIMATION_END_BLEND_TIME + 0.05f)
		{
			m_fCurrentElapsed += fDelta;

			float fWeight = (UPPER_ANIMATION_END_BLEND_TIME - m_fCurrentElapsed) / UPPER_ANIMATION_END_BLEND_TIME;

			if(fWeight <= 0.0f)
			{
				fWeight = 0.0f;
			}

			m_pRAnimationController->SetWeight(fWeight);
		}
	}

	if(IsPlayDone() == true)
	{
		m_fCurrentElapsed = 0.0f;
		m_bUseUpperAnimation = false;
	}

	m_pRAnimationController->OnUpdate();
}

bool XUpperAnimationTC::IsOncePlayDone()
{
	return m_pRAnimationController->IsOncePlayDone();
}

bool XUpperAnimationTC::IsPlayDone()
{
	return m_pRAnimationController->IsPlayDone();
}

bool XUpperAnimationTC::IsLoop()
{
	RAnimation* pAni = m_pRAnimationController->GetAnimation();
	return (pAni && pAni->GetAnimationLoopType() == RAniLoopType_Loop) ? true : false;
}

int XUpperAnimationTC::GetAniFrame()
{
	return m_pRAnimationController->GetFrame();
}

int XUpperAnimationTC::GetAniMaxFrame()
{
	return m_pRAnimationController->GetMaxFrame();
}

void XUpperAnimationTC::SetAnimationFrame( unsigned int nFrame )
{
	m_pRAnimationController->SetFrame(nFrame);
	m_pRAnimationController->DisableAniamtionEventPreFrame(nFrame);
}

void XUpperAnimationTC::PreUpdate( float fDelta )
{
	m_pRAnimationController->OnPreUpdate();
}

void XUpperAnimationTC::Stop()
{
	if(m_bUseUpperAnimation)
		m_pRAnimationController->Stop();
}

void XUpperAnimationTC::SetAnimationSpeed( float fSpeed )
{
	if(m_pRAnimationController == NULL)
		return;

	m_pRAnimationController->SetSpeed(fSpeed);
}

float XUpperAnimationTC::GetAnimationSpeed()
{
	if(m_pRAnimationController == NULL)
		return 0.0f;

	return m_pRAnimationController->GetSpeed();
}

float XUpperAnimationTC::GetMaxFrameToSecond()
{
	if(m_pRAnimationController == NULL)
		return 0.0f;

	DWORD dwMaxFrame = m_pRAnimationController->GetMaxFrame();
	float fSpeed = m_pRAnimationController->GetSpeed();

	return dwMaxFrame / 4.8f / (fSpeed * 1000.0f);
}

float XUpperAnimationTC::GetAniFrameToSecond()
{
	float fSpeed = 0.0f;

	if(m_bUseUpperAnimation)
		fSpeed = m_pRAnimationController->GetSpeed();

	return GetAniFrame() / 4.8f / (fSpeed * 1000.f);
}
