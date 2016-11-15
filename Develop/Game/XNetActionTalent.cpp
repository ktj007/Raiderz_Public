#include "stdafx.h"
#include "XNetActionTalent.h"
#include "XModuleNetControl.h"
#include "XEventID.h"
#include "XModuleEffect.h"
#include "XModuleAction.h"
#include "XModuleTalent.h"
#include "XTalentHelper.h"

XNetActionTalent::XNetActionTalent( XModuleNetControl* pOwner) : XNetActionState( pOwner),
m_fCastingTime(0.0f), 
m_nCurrentTalentID(0),
m_bCastingEndEffectDone(false)
{

}


XNetActionTalent::~XNetActionTalent()
{
}


void XNetActionTalent::Enter( void* pParam /*=NULL*/)
{
	m_fCastingTime = 0.0f;
	m_nCurrentTalentID = 0;
}


void XNetActionTalent::Exit()
{
	m_fCastingTime = 0.0f;
	m_nCurrentTalentID = 0;

	XNetActionState::Exit();
}


XEventResult XNetActionTalent::Event( XEvent& msg)
{
	switch ( msg.m_nID)
	{
	case XEVTL_TALENT_START:
		{
			// 탤런트 시작을 했다.
			// 여기서 타겟수와 스타일을 잡는건... 단계별로 타겟 방식이 다를 수 있기 때문이다.
			XTalentInfo* pTalentInfo	= static_cast<XTalentInfo*>(msg.m_pData);
			if(pTalentInfo == NULL)
				return MR_FALSE;

			TalentStart(pTalentInfo);

			return MR_TRUE;
		}
	case XEVTL_TALENT_CANCELED:
		{
			m_pOwner->GetOwner()->GetModuleAction()->Idle();
			m_pOwner->DoAction( NET_ACTION_STATE_NA);
			return MR_TRUE;
		}

	case XEVTL_TALENT_ACT_SPELL:
		{
			if(m_bCastingEndEffectDone == false)
				CastingEndEffect();

			return MR_TRUE;
		}

	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:
		{
			m_pOwner->DoAction( NET_ACTION_STATE_NA);
			return MR_FALSE;
		}
	case XEVTL_NET_MOVE:
		{
			if(CheckCancelableSocialTalent())
				return MR_TRUE;
		}
		break;
	}

	return MR_FALSE;
}


void XNetActionTalent::Update( float fDelta)
{
	CheckCastingTime(fDelta);
}

void XNetActionTalent::TalentStart( XTalentInfo* pTalentInfo )
{
	// 그룹 탤런트 UI 보여주기
	if(pTalentInfo == NULL)
		return;

	m_fCastingTime = pTalentInfo->m_fCastingTime;

	m_nCurrentTalentID = pTalentInfo->m_nID;

	m_bCastingEndEffectDone = false;

	// 소셜은 채팅창에도 뿌린다.
	if(pTalentInfo->m_nSkillType == ST_SOCIAL)
	{
		global.ui->OnChatMsg(MT_SOCIAL, L"{0}님이 {1}", FSParam(m_pOwner->GetOwner()->GetName(), pTalentInfo->GetDesc()));
	}
}

void XNetActionTalent::CheckCastingTime( float fDelta )
{
	if(m_fCastingTime > 0.0f)
	{
		m_fCastingTime -= fDelta;

		if(m_fCastingTime <= 0.0f)
		{
			// 캐스팅 완료 이펙트 발사
			CastingEndEffect();
			return;
		}
	}
}

void XNetActionTalent::CastingEndEffect()
{
	XModuleEffect * pModuleEffect = m_pOwner->GetOwner()->GetModuleEffect();
	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(m_nCurrentTalentID, m_pOwner->GetOwner());
	if (pTalentInfo == NULL) return;
	if(pModuleEffect)
	{
		XTalentEffectEventData * pCastEndEffectData = new XTalentEffectEventData;
		pCastEndEffectData->Set(pTalentInfo, CAST_END_EFFECT, m_pOwner->GetOwner(), vec3(0.0f, 0.0f, 0.0f), NULL, true);
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_TALENT, pCastEndEffectData);
	}

	m_bCastingEndEffectDone = true;
}

bool XNetActionTalent::CheckCancelableSocialTalent()
{
	XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
	XModuleTalent* pModuleTalent = m_pOwner->GetOwner()->GetModuleTalent();
	if(pModuleMotion && pModuleTalent && pModuleTalent->IsActive())
	{
		wstring strMotion = pModuleMotion->GetCurrMotion();
		XTalentInfo* pTalentInfo = pModuleTalent->GetInfo();

		wstring strSocialAni = L"";
		if(pTalentInfo && pTalentInfo->m_nSkillType == ST_SOCIAL)
			strSocialAni= pTalentInfo->m_szUseAnimation;

		if(strMotion == strSocialAni)
		{
			m_pOwner->GetOwner()->GetModuleAction()->Idle();
			return true;
		}
	}

	return false;
}
