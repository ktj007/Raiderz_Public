#include "stdafx.h"
#include "XMyActionQuestInteraction.h"
#include "XModuleMyControl.h"
#include "XModuleAction.h"
#include "XPost_Quest.h"

XMyActionQuestInteraction::XMyActionQuestInteraction(XModuleMyControl* pOwner)
: XMyActionState(pOwner) 
{

}

bool XMyActionQuestInteraction::CheckEnterable(void* pParam)
{
	StopMove();

	return true;
}

void XMyActionQuestInteraction::StopMove()
{
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (!pModuleAction) return;

	pModuleAction->StopMove();
}

void XMyActionQuestInteraction::Enter(void* pParam)
{
	// TODO : 일단 루팅 애니
	if(m_pOwner->GetOwner()->GetModuleAction()->ChangeMotionQuestInteraction() == false)
	{
		mlog_duck("애니메이션이 없어요");
		return;
	}

	if(global.script)
	{
		//global.script->GetGlueGameEvent().OnGageBarStart(XCONST::INTERACT_QUEST_OBJECTIVE_ANI_TIME);
		global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "BEGIN", MAPT_INTERACTION, XCONST::INTERACT_QUEST_OBJECTIVE_ANI_TIME * 1000.0f);

		m_Regulator.SetTickTime(XCONST::INTERACT_QUEST_OBJECTIVE_ANI_TIME);
		m_Regulator.Start();

	}

	XMyActionState::Enter(pParam);	//로그용.
}

void XMyActionQuestInteraction::Exit()
{
	if(global.script)
	{
		//global.script->GetGlueGameEvent().OnGageBarEnd();
		global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "END");
	}

	XMyActionState::Exit();
}

void XMyActionQuestInteraction::Update( float fDelta )
{
	if(m_Regulator.IsActive())
	{
		if (m_Regulator.IsReady(fDelta))
		{
			m_Regulator.Stop();
		}
		else
		{
//			float fElapsedTime = m_Regulator.GetElapsedTime()*100;
//			global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "REFRESH", fElapsedTime);
		}
	}
}

XEventResult XMyActionQuestInteraction::Event( XEvent& msg )
{
	switch(msg.m_nID)
	{
	case XEVTL_VIRTUAL_KEY_PRESSED:
		{
			// 이동키를 눌렀다면 루팅 종료
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
			switch (nVirtualKey)
			{
			case VKEY_FORWARD:
			case VKEY_BACKWARD:
			case VKEY_LEFT:
			case VKEY_RIGHT:
				{
					XPostQuest_RequestInteractCancel();
				}
				break;
			}
		}
		break;
	case XEVTD_TALENT_HIT:
		{
			XPostQuest_RequestInteractCancel();
		}
		break;
	}

	return XMyActionState::Event(msg); // return MR_FALSE; 
}