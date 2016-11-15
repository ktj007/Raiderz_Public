#include "stdafx.h"
#include "XFakeBeatenAnimationTC.h"
#include "MLocale.h"


XFakeBeatenAnimationTC::XFakeBeatenAnimationTC( XCharacter* pCharacter )
:XTransformController(pCharacter)
, m_pCurrentReActionInfo(NULL)
, m_fWeaponVisibility(1.0f)
{
	if(m_pActor == NULL)
		return;

	m_pRAnimationController = new XRAnimationController(RSNCTRL_CUSTOMUPDATE, RSNCTRL_CUSTOMUPDATE);
	m_pActor->AddController(m_pRAnimationController);
}

XFakeBeatenAnimationTC::~XFakeBeatenAnimationTC( void )
{
	SAFE_DELETE(m_pRAnimationController);
}

void XFakeBeatenAnimationTC::Update( float fDelta )
{
	if(m_pRAnimationController->IsPlayDone() == false)
	{
		CheckWeaponVisibility();

		m_pRAnimationController->OnUpdate();
	}
}

void XFakeBeatenAnimationTC::Stop()
{

}

bool XFakeBeatenAnimationTC::StartBeaten( const TCHAR* szCurAni, const float fWeightRate, const float fSpeedRate, const TCHAR* szAniPrefix/*=NULL*/ )
{
	m_pCurrentReActionInfo = NULL;

	// 준비가 안된상태에서는 페이크비튼을 사용할수 없다.
	ANIMATION_CHECK_STATE eState = CheckAnimationUsable(tstring(szCurAni));
	if( eState != AS_LOADED)
		return false;

	m_fWeaponVisibility = 1.0f;

	XReActionAnimationInfo* pInfo = m_pActor->GetReActionAnimationInfo();
	ReAction_DATA* pReActionData = &m_ReActionDefalutInfo;
	bool bUseDefalut = true;

	if(pInfo)
	{
		ReAction_DATA* pGetReActionData = pInfo->Get(szCurAni);
		if(pGetReActionData)
		{
			pReActionData = pGetReActionData;
			bUseDefalut = false;

			// 리액션 애니메이션이 없다면 디폴트로 해준다.
			if(pGetReActionData->strReActionName.empty())
				pReActionData->strReActionName = m_ReActionDefalutInfo.strReActionName;
		}
	}

	if(pReActionData->bDisable)
		return false;

	m_pCurrentReActionInfo = pReActionData;

	RPLAYINFO playInfo;
	playInfo.blendMode = RBLENDMODE_BLEND;
	playInfo.fPlaySpeed = pReActionData->fSpeed * fSpeedRate;
	playInfo.fWeight = pReActionData->fWeight * fWeightRate;
	if(playInfo.fWeight > 1.0f)
		playInfo.fWeight = 1.0f;
	playInfo.fBlendIn = 0.3f;
	playInfo.fBlendOut = 0.7f;

	tstring strAni;
	if (szAniPrefix && bUseDefalut)
	{
		strAni = szAniPrefix + pReActionData->strReActionName;
	}
	else
	{
		strAni = pReActionData->strReActionName;
	}

	bool bUseUpperAnimation = m_pRAnimationController->SetAnimation( MLocale::ConvTCHARToAnsi(strAni.c_str()).c_str(), &playInfo );
	if(bUseUpperAnimation == false)
	{
		// 접두어 빼고 한번더...
		bUseUpperAnimation = m_pRAnimationController->SetAnimation( MLocale::ConvTCHARToAnsi(pReActionData->strReActionName.c_str()).c_str(), &playInfo );
		if(bUseUpperAnimation == false)
		{
			dlog("리액션 애니메이션 에러 %s or %s\n", MLocale::ConvTCHARToAnsi(strAni.c_str()).c_str(), MLocale::ConvTCHARToAnsi(pReActionData->strReActionName.c_str()).c_str());
			return false;
		}
	}


	m_pRAnimationController->Play();
	m_pRAnimationController->SetSpeed(playInfo.fPlaySpeed);

	return true;
}

void XFakeBeatenAnimationTC::Init( tstring& strAniName, float fWeight, float fSpeed )
{
	if (fWeight <= 0.0f) return;

	m_pRAnimationController->SetEventActive(false);

	m_ReActionDefalutInfo.strReActionName	= strAniName;
	m_ReActionDefalutInfo.fWeight			= fWeight;
	m_ReActionDefalutInfo.fSpeed			= fSpeed;

	SetEnable(true);
}

void XFakeBeatenAnimationTC::PreUpdate( float fDelta )
{
	if(m_pRAnimationController->IsPlayDone() == false)
		m_pRAnimationController->OnPreUpdate();
}

void XFakeBeatenAnimationTC::PostUpdate( float fDelta )
{
	if(m_pRAnimationController->IsPlayDone() == false)
	{
		// 무기 본이 설정이 되어 있다면...
		if(m_pCurrentReActionInfo && m_pCurrentReActionInfo->strWeaponBoneName.empty() == false)
		{
			RActorNode* pActorNode = m_pActor->GetActorNode(MLocale::ConvTCHARToAnsi(m_pCurrentReActionInfo->strWeaponBoneName.c_str()).c_str());
			if(pActorNode == NULL)
				return;

			RAnimationControllerNode* pControllerNode = m_pRAnimationController->GetControllerNode(m_pCurrentReActionInfo->strWeaponBoneName);
			if(pControllerNode)
			{
				pActorNode->SetVisibility(m_fWeaponVisibility);
			}
		}
	}
}

void XFakeBeatenAnimationTC::CheckWeaponVisibility()
{
	// 웨폰 본 visibility 값 가져오기
	if(m_pCurrentReActionInfo && m_pCurrentReActionInfo->strWeaponBoneName.empty() == false)
	{
		RActorNode* pActorNode = m_pActor->GetActorNode(MLocale::ConvTCHARToAnsi(m_pCurrentReActionInfo->strWeaponBoneName.c_str()).c_str());

		if(pActorNode)
		{
			m_fWeaponVisibility = pActorNode->GetVisibility();
		}
	}
}
