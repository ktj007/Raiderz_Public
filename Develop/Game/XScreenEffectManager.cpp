#include "stdafx.h"
#include "XGlueGameEvent.h"
#include "XScreenEffectManager.h"
#include "RSceneManager.h"
#include "XProfiler.h"

///////////////////////////////////////////////////////////////////////////////////////
// XScreenEffectManager ///////////////////////////////////////////////////////////////
XScreenEffectManager::XScreenEffectManager() : RScreenEffectManager()
{
	m_nTargetUIState	= TUS_NOT;
	m_bTargetUIStart	= false;
	m_bTargetLock		= false;
}

XScreenEffectManager::~XScreenEffectManager()
{
}

void XScreenEffectManager::Create(RSceneManager *pManager)
{
	SetSceneManager(pManager);
	ResisterTarget();
	ResisterVictory();
}

void XScreenEffectManager::Destroy()
{
	RScreenEffectManager::Destroy();
}

void XScreenEffectManager::Update()
{
	RScreenEffectManager::Update();
}

void XScreenEffectManager::Render()
{
	PFC_THISFUNC;
	RScreenEffectManager::Render();
}

void XScreenEffectManager::LoadScreenEffect()
{
	// 정적으로 스크린 이펙트 로딩하는 함수
}

void XScreenEffectManager::AddScreenEffect(wstring strListFileName, XSCREENEFFECT_CHARACTER_ATT charAtt, AnimationLoopType aniLoop)
{
	wstring strFullPath = PATH_SCREENEFFECT + strListFileName;
	RScreenEffectManager::AddScreenEffect(MLocale::ConvUTF16ToAnsi(strFullPath.c_str()), charAtt, aniLoop);
}

void XScreenEffectManager::RegisterScreenEffect(wstring strScreenEffectName, 
											   wstring strListFileName,
											   XSCREENEFFECT_CHARACTER_ATT charAtt,
											   AnimationLoopType aniLoop)
{
	wstring strFullPath = PATH_SCREENEFFECT + strListFileName;

	RScreenEffectManager::RegisterScreenEffect(MLocale::ConvUTF16ToAnsi(strScreenEffectName.c_str()),
												MLocale::ConvUTF16ToAnsi(strFullPath.c_str()),
												charAtt,
												aniLoop);
}

void XScreenEffectManager::SetString(wstring strScreenEffectName, wstring strContents)
{
	RScreenEffectManager::SetString(MLocale::ConvUTF16ToAnsi(strScreenEffectName.c_str()), MLocale::ConvUTF16ToAnsi(strContents.c_str()));
}

void XScreenEffectManager::SetPosition(wstring strScreenEffectName, 
									   vec3 vPosEffect, 
									   XSCREENEFFECT_LOCATION_TYPE scrBase)
{

	RScreenEffectManager::SetPosition(MLocale::ConvUTF16ToAnsi(strScreenEffectName.c_str()), vec3(vPosEffect.x, vPosEffect.y + XCONST::CAMERA_NEAR_Z, vPosEffect.z), scrBase);
}

void XScreenEffectManager::ChangeAnimation(wstring strScreenEffectName, wstring strAnimation, XSCREENEFFECT_ANIMATION_END_ATT aniDel)
{
	RScreenEffectManager::ChangeAnimation(MLocale::ConvUTF16ToAnsi(strScreenEffectName.c_str()), MLocale::ConvUTF16ToAnsi(strAnimation.c_str()), aniDel);
}

void XScreenEffectManager::SetDraw(wstring strScreenEffectName, bool bVisible)
{
	RScreenEffectManager::SetDraw(MLocale::ConvUTF16ToAnsi(strScreenEffectName.c_str()), bVisible);
}

void XScreenEffectManager::AnimationStop(wstring strScreenEffectName)
{
	RScreenEffectManager::AnimationStop(MLocale::ConvUTF16ToAnsi(strScreenEffectName.c_str()));
}

void XScreenEffectManager::ResisterTarget()
{
	// 타겟 등록
	RegisterScreenEffect(TARGET_UI_NAME, TARGET_UI_EFFECT_NAME, CHARACTER_NORMAL, RAniLoopType_HoldLastFrame);
}

void XScreenEffectManager::SetTarget( TARGET_UI_STATE nState, int x /*= 0*/, int y /*= 0*/ )
{
	/************************************************************************/
	/* 여기서 타겟 설정에 대한 조건도 기술한다.                             */
	/************************************************************************/
	const float fScreenEffectHeightRate = 3.4f;
	if(y != 0)
	{
		float yTemp = y;
		y = yTemp / fScreenEffectHeightRate;
	}

	//------------------------------------------------------------------------
	// 이미 진행중이다.
	if(m_nTargetUIState == nState)
		return;

	switch(nState)
	{
	case TUS_START:
		{
			//ChangeAnimation(TARGET_UI_NAME, TARGET_UI_STATE_START, ANIMATION_END_SHOW);
			m_bTargetUIStart = true;
			m_bTargetLock = false;
			////RScreenEffectManager::SetVisibility(MLocale::ConvUTF16ToAnsi(TARGET_UI_NAME), 0.4f);
			//mlog("스타트\n");
		}
		break;
	case TUS_END:
		{
			// 타겟 UI가 시작이 되어 있어야 끝날수 있다.
			if(m_bTargetUIStart == false)
				return;

			if(m_bTargetLock)
				ChangeAnimation(TARGET_UI_NAME, TARGET_UI_STATE_LOCK_END, ANIMATION_END_HIDE);
			else
				ChangeAnimation(TARGET_UI_NAME, TARGET_UI_STATE_END, ANIMATION_END_HIDE);

			m_bTargetUIStart = false;
		}
		break;
	case TUS_LOCK:
		{
			////RScreenEffectManager::SetVisibility(MLocale::ConvUTF16ToAnsi(TARGET_UI_NAME), 1.0f);
			//// 타겟 UI가 시작이 되어 있어야 사용 할 수 있다.
			//if(RScreenEffectManager::IsAnimationEnd(MLocale::ConvUTF16ToAnsi(TARGET_UI_NAME), "") == false ||
			//	m_bTargetUIStart == false)
			//{
			//	// 애니메이션이 한번 끝나기 건까지 처리 할 수 없는 것들이다.
			//	return;
			//}
			if(m_bTargetUIStart == false)
				return;

			m_bTargetLock = true;
			ChangeAnimation(TARGET_UI_NAME, TARGET_UI_STATE_LOCK, ANIMATION_END_SHOW);
		}
		break;
	case TUS_AIM:
		{
			//// 타겟 UI가 시작이 되어 있어야 사용 할 수 있다.
			//if(RScreenEffectManager::IsAnimationEnd(MLocale::ConvUTF16ToAnsi(TARGET_UI_NAME), MLocale::ConvUTF16ToAnsi(TARGET_UI_STATE_START)) == false ||
			//	m_bTargetUIStart == false)
			//{
			//	// 애니메이션이 한번 끝나기 건까지 처리 할 수 없는 것들이다.
			//	return;
			//}
			if(m_bTargetUIStart == false)
				return;

			m_bTargetLock = false;
			ChangeAnimation(TARGET_UI_NAME, TARGET_UI_STATE_START, ANIMATION_END_SHOW);
			//RScreenEffectManager::SetLastFrame(MLocale::ConvUTF16ToAnsi(TARGET_UI_NAME));
		}
		break;
	}

	m_nTargetUIState = nState;

	SetPosition(TARGET_UI_NAME, vec3(x, 300, y), LOCATION_RIGHT);
	SetDraw(TARGET_UI_NAME, true);
}

void XScreenEffectManager::ResisterVictory()
{
	RegisterScreenEffect(L"Victory", L"ui_victory", CHARACTER_NORMAL, RAniLoopType_Loop);
}

void XScreenEffectManager::SetVictory( TARGET_UI_STATE nState, int x /*= 0*/, int y /*= 0*/ )
{
	SetPosition(L"Victory", vec3(x, 800, y), LOCATION_CENTER);
	SetDraw(L"Victory", true);
}