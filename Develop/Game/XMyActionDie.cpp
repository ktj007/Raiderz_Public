#include "stdafx.h"
#include "XMyActionDie.h"
#include "XModuleAction.h"
#include "XModuleMyControl.h"
#include "XEventID.h"
#include "XGame.h"
#include "XBattleArena.h"
#include "XMyPlayer.h"
#include "XPost_Char.h"

#ifdef _MADDUCK3
#include "XPost_GM.h"
#include "XBandiCapturer.h"
#endif

XMyActionDie::XMyActionDie( XModuleMyControl* pOwner )
: XMyActionState(pOwner)
, m_fRebirthTime(0.f)
{

}

bool XMyActionDie::CheckEnterable( void* pParam )
{
	if (m_pOwner->IsReservedHitByTalent() == true) return false;
	if (m_pOwner->GetMyPlayerOwner()->IsDead() == false) return false;
	if (m_pOwner->GetCurrentActionStateID() == ACTION_STATE_DIE)
	{
		_ASSERT(0);
		return false;
	}

	return true;
}

//XActor::m_nDeadTime 존재
void XMyActionDie::Enter(void* pParam)
{
	//mlog("==== %s ====\n", __FUNCTION__);

#ifdef _MADDUCK3
	m_bTestRebirth = false;
#endif

	m_bChangeState = false;

	gvar.MyInfo.SetHP(0);

	if ( global.sound)
		global.sound->Play( MLocale::ConvUTF16ToAnsi(XCONST::DIEBGMNAME).c_str(), "die");

	XMyActionState::Enter(pParam);

	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (pModuleAction)
	{
		pModuleAction->Die();
	}

#ifdef _MADDUCK3
	m_fRebirthTime = 60.f;
#else
	m_fRebirthTime = GAME_AUTO_REBIRTH_CHECK_TICK;
#endif

	if (gg.currentgamestate && gg.currentgamestate->GetBattleArena()->IsInBattleArena())
	{
		m_fRebirthTime = gg.currentgamestate->GetBattleArena()->GetRespawnTime();
	}

	if (gvar.MyInfo.fAutoRebirthRemainCoolTime > 0.f)
	{
		m_fRebirthTime = gvar.MyInfo.fAutoRebirthRemainCoolTime;
		gvar.MyInfo.fAutoRebirthRemainCoolTime = 0.f;
	}

	m_RebirthRegulator.SetTickTime(m_fRebirthTime);
	m_RebirthRegulator.Start();

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ACTION", "DIE");
}

void XMyActionDie::Exit()
{
	if ( global.sound)
		global.sound->Stop( "die");

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnRebirthHide();
	}

	XMyActionState::Exit();

	//mlog("==== XMyActionDie::Exit ====\n");
}

XEventResult XMyActionDie::Event(XEvent& msg)
{
	switch (msg.m_nID)
	{
	case XEVTL_MOTION_PLAYONCE:
		{
			//mlog("XEVTL_MOTION_PLAYONCE\n");

			//DIE 애니메이션이 끝나면 부활창 띄우기
			RebirthWindowShow();

			return MR_TRUE;
		}
		break;
	case XEVTL_DIE_ANI_END:
		{
			// 강제 처리
			RebirthWindowShow(true);
		}
		break;
	}

	return XMyActionState::Event(msg);
}

void XMyActionDie::Update( float fDelta )
{
#ifdef _MADDUCK3
	Debug_AutoRebirth();
#endif

#ifdef _MADDUCK3
	MFrame* pRebirth = (MFrame*)global.mint->FindWidget("frmRebirth");
	if (pRebirth && pRebirth->GetShow() == true)
	{
#endif
		if(m_RebirthRegulator.IsReady(fDelta))
		{
			m_RebirthRegulator.Stop();
			Rebirth();

#ifdef _MADDUCK3
			if (global.bandi_capturer && global.bandi_capturer->IsCapturing() == true)
				global.bandi_capturer->ToggleStart();
#endif
		}
#ifdef _MADDUCK3
	}
#endif

#ifndef _MADDUCK3
	if (m_RebirthRegulator.GetElapsedTime() > 5.f)
		CheckRebirthWindow();
#endif
}

void XMyActionDie::Rebirth()
{
	if (global.script)
	{
		global.script->GetGlueGameEvent().OnRebirthHide();

		if (gg.currentgamestate->GetBattleArena()->IsInBattleArena())
		{
			XPostChar_Rebirth(REBT_PLACE_IN_BATTLEGROUD);
		}
		else
		{
			XPostChar_Rebirth(REBT_SOULBINDING);
		}
	}
}

void XMyActionDie::RebirthRequireWindowShow()
{
	if (gg.currentgamestate == NULL) return;
	if (gg.currentgamestate->GetBattleArena()->IsVisibleScoreBoard()) return;

	if (global.script)
	{
		if (gg.currentgamestate->GetBattleArena()->IsInBattleArena())
		{
			global.script->GetGlueGameEvent().OnBattleArenaRebirthShow();
		}
		else
		{
			global.script->GetGlueGameEvent().OnRebirthShow();
		}
	}
}

bool XMyActionDie::CheckExitable( int nNextStateID )
{
	if(m_bChangeState == false)	return false;

	if (nNextStateID == ACTION_STATE_REBIRTH) return true;
	if (nNextStateID == ACTION_STATE_USE_ITEM) return true;

	_ASSERT(0);

	return false;
}

float XMyActionDie::GetRebirthRemainTime()
{
	return m_fRebirthTime - m_RebirthRegulator.GetElapsedTime();
}

void XMyActionDie::CheckRebirthWindow()
{
	MFrame* pRebirth = (MFrame*)global.mint->FindWidget("frmRebirth");
	if (pRebirth != NULL && pRebirth->GetShow() == false)
	{
		RebirthWindowShow();
	}
}

void XMyActionDie::RebirthWindowShow( bool bForceDieAniEnd /*= false*/ )
{
	XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
	if (pModuleMotion)
	{
		if (pModuleMotion->GetCurrMotion() == MOTION_NAME_DIE ||
			bForceDieAniEnd == true)
		{
			RebirthRequireWindowShow();
			m_bChangeState = true;
		}
		else
		{
			_ASSERT(0);
		}
	}
}


#ifdef _MADDUCK3
void XMyActionDie::Debug_AutoRebirth()
{
	MFrame* pRebirth = (MFrame*)global.mint->FindWidget("frmRebirth");
	if (m_bTestRebirth == false && pRebirth != NULL && pRebirth->GetShow() == true)
	{
		if ((m_fRebirthTime - m_RebirthRegulator.GetElapsedTime()) < (m_fRebirthTime - 5.f))
		{
			m_bTestRebirth = true;

			mlog("자동부활 : %f \n", (m_fRebirthTime - 5.f));

			vec3 pos = vec3(3843.7612f, 4261.8682f, 3.0000000f);

			XPostGM_Move(9091, pos);
			XPostGMRebirth(pos);

			if (global.bandi_capturer && global.bandi_capturer->IsCapturing() == true)
				global.bandi_capturer->ToggleStart();

			if (global.bandi_capturer && global.bandi_capturer->IsCapturing() == false)
				global.bandi_capturer->ToggleStart();
		}
	}
}
#endif