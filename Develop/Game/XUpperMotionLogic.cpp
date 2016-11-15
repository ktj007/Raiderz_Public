#include "stdafx.h"
#include "XUpperMotionLogic.h"
#include "XMotionTimeModifier.h"
#include "XGameTransformControllerManager.h"
#include "XMotionMgr.h"

XUpperMotionLogic::XUpperMotionLogic(XActor* pOwner, XGameTransformControllerManager* pAnimationController, XMotionTimeModifier* pMotionTimeModifier, XMotionIdleMemory* pMotionIdleMemory)
:XMotionLogic(pOwner, pAnimationController, pMotionTimeModifier, pMotionIdleMemory)
{

}

XUpperMotionLogic::~XUpperMotionLogic()
{

}

bool XUpperMotionLogic::_ChangeMotion( const wchar_t* szMotionName, XMotionTypes nType, bool bForced, bool bPreFrame /*= false */ )
{
	m_Error.Clear();

	m_bChanged = true;
	ResetAniTime();

	//if (m_pCurrMotion)
	//{
	//	if ((bForced == false) && 
	//		(m_bPlayDone == false) && 
	//		(m_pCurrMotion->IsCancelEnabled() == false) ) 
	//	{
	//		return false;
	//	}
	//}

	XMotion* pNextMotion = info.motion->GetMotion(szMotionName);
	if (pNextMotion == NULL) 
	{
		m_Error.SetMotionError(szMotionName);

		return false;
	}

	if (!bForced && (pNextMotion == m_pCurrMotion) && (nType == m_nCurrType)) 
	{
		return true;	// 이미 해당 모션이면 변경없이 true로 반환한다.
	}

	XMotion* pPreEnterRet = OnPreEnter(pNextMotion);
	if (pPreEnterRet)
	{
		if (!bForced && (pNextMotion == m_pCurrMotion) && (nType == m_nCurrType)) 
		{
			return true;	// 이미 해당 모션이면 변경없이 true로 반환한다.
		}
		pNextMotion = pPreEnterRet;
	}

	XMotion* pPrevMotion = m_pCurrMotion;

	ResetInfo();

	m_pCurrMotion = pNextMotion;
	m_nCurrType = nType;

	if (m_pCurrMotion->m_vecAniSequence[m_nCurrType].empty()) m_nCurrType = MT_DEFAULT;

	m_pMotionTimeModifier->CalcMotionLength(m_pCurrMotion, m_nCurrType, m_bUseWeaponAni, m_nWeaponType);

	if (SetAnimation(bForced) == false)
	{
		return false;
	}

	OnLeave(pPrevMotion);
	OnEnter(m_pCurrMotion);

	return true;
}

bool XUpperMotionLogic::SetAnimation( bool bForced/*=false*/, bool bPreFrame /*= false*/ )
{
	bool ret = false;
	m_Error.ClearAnimationError();

	XMotionTypes currMotionType = GetCurrMotionType();

	if (m_pCurrMotion)
	{
		if (!m_pCurrMotion->m_vecAniSequence[currMotionType].empty())
		{
			wstring strCurrAni;
			MakeCurrAniString(strCurrAni, currMotionType);

			if (m_bUseAniPostfix && m_pCurrMotion->IsAddPostfix())
			{
				wstring strCurrAniWithPostfix = strCurrAni + m_strAniPostfix;
				ret = m_pAnimationController->SetUpperAnimation(strCurrAniWithPostfix);
			}

			if (ret == false)
			{
				// postfix붙여서 실패이면 postfix를 떼고 애니메이션 변환
				ret = m_pAnimationController->SetUpperAnimation(strCurrAni);
			}

			if(ret == false)
			{
				// 실패인가? 그렇다면 애니메이션에 무기를 설정하지 않고 해본다.
				MakeCurrAniStringNoWeapon(strCurrAni, currMotionType);
				ret = m_pAnimationController->SetUpperAnimation(strCurrAni);
			}

			if (ret == false)
			{
				// 애니메이션 에러
				m_Error.SetAnimationError(strCurrAni);
			}
		}
		else	
		{
			// 만약 sequence가 하나도 없으면 custom 모션일 것이다.
			if (m_pCurrMotion->IsCustom())
			{
				wstring strCurrAni = GetCustomMotionAniName(m_pCurrMotion->GetName());

				ret = m_pAnimationController->SetUpperAnimation(strCurrAni);

				if(ret == false)
				{
					// 애니메이션 에러
					m_Error.SetAnimationError(strCurrAni);
				}
			}
		}
	}

	return ret;
}

void XUpperMotionLogic::Update( float fDelta )
{
	if (m_pCurrMotion == NULL) return;

	OnUpdate(m_pCurrMotion);

	m_bChanged = false;

	if(m_bPlayDone == false &&
		m_pAnimationController->IsUpperAnimationLoop() == false &&
		m_pAnimationController->IsUpperAnimationOncePlayDone())
	{
		PlayDone();
	}
}

void XUpperMotionLogic::PlayDone()
{
	if (m_bPlayDone == true) return;

	m_bPlayDone = true;
	m_pCurrMotion = NULL;

	m_pAnimationController->InitUpperAnimation();
	PlayNextMotion();

	SendEvent(XEVTL_UPPER_MOTION_PLAYONCE);
}

void XUpperMotionLogic::StopUpperMotion()
{
	if (m_bPlayDone == true) return;

	m_bPlayDone = true;
	m_pCurrMotion = NULL;

	m_pAnimationController->StopUpperAnimation();
	m_pAnimationController->InitUpperAnimation();
}

bool XUpperMotionLogic::IsCurrentMotion( wstring szMotionName )
{
	if(m_pCurrMotion)
		return XMotionLogic::IsCurrentMotion(szMotionName);

	return false;
}