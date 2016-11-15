#include "stdafx.h"
#include "XGuideMsg.h"
#include "XUISystem.h"
#include "XScript.h"

#define GUIDE_MSG_LOW_LEVEL 2


void XGuideMsg::InitDesc()
{
	// Polling, LevelCheck, State
	m_DescTableMap.insert(XGuideMsgDescTable::value_type(GUIDE_MSG_NA,							XGuideMsgDesc(false, false, PRESENTATION_LOWER)));
	m_DescTableMap.insert(XGuideMsgDescTable::value_type(GUIDE_MSG_LEVEL_UP,					XGuideMsgDesc(false, false, PRESENTATION_LOWER)));
	m_DescTableMap.insert(XGuideMsgDescTable::value_type(GUIDE_MSG_CAN_LOOT,					XGuideMsgDesc(true, true, LOOT_GUIDE)));
	m_DescTableMap.insert(XGuideMsgDescTable::value_type(GUIDE_MSG_CAN_INTERACT_TO_NPC,			XGuideMsgDesc(true, true, NPC_INTERACTION_GUIDE)));
	m_DescTableMap.insert(XGuideMsgDescTable::value_type(GUIDE_MSG_LUA_RECV_INVITE_PARTY,		XGuideMsgDesc(false, false, PRESENTATION_LOWER)));
	m_DescTableMap.insert(XGuideMsgDescTable::value_type(GUIDE_MSG_LUA_RECV_INVITE_GUILD,		XGuideMsgDesc(false, false, PRESENTATION_LOWER)));
	m_DescTableMap.insert(XGuideMsgDescTable::value_type(GUIDE_MSG_LUA_RECV_TRADE_REQUESTED,	XGuideMsgDesc(false, false, PRESENTATION_LOWER)));
	m_DescTableMap.insert(XGuideMsgDescTable::value_type(GUIDE_MSG_LUA_RECV_DUEL_CHALLENGED,	XGuideMsgDesc(false, false, PRESENTATION_LOWER)));
}

XGuideMsg::XGuideMsg() : m_CurState(IDLE)
{
	m_bEntered = false;
	m_uidPollingTarget = MUID::ZERO;

	InitDesc();
}

void XGuideMsg::OutputMsg( XGuideMsgEnum nMsg, MUID uidTarget )
{
	if (UpdateStateAndCheck(nMsg, true, uidTarget) == false) 
		return;

	OnGuideMsg(nMsg, PRESENTATION_ICON_NONE);
}

void XGuideMsg::ReleaseMsg( XGuideMsgEnum nMsg /*= GUIDE_MSG_NA*/, MUID uidTarget )
{
	if (UpdateStateAndCheck(nMsg, false, uidTarget) == false) 
		return;

	OnReleaseMsg();
}

void XGuideMsg::OnGuideMsg( XGuideMsgEnum nMsg, XPRESENTATIONICON_TYPE nIconType )
{
	_ASSERT(nMsg != GUIDE_MSG_NA);

	if (!IsEnabled()) return;

	global.script->GetGlueGameEvent().OnGameEvent( "GUIDEMSG", "", (int)nMsg, (int)nIconType);
}

void XGuideMsg::OnReleaseMsg()
{
	if (!IsEnabled()) return;

	global.script->GetGlueGameEvent().OnGameEvent( "GUIDEMSG", "", (int)GUIDE_MSG_NA, 0);
}

bool XGuideMsg::UpdateStateAndCheck( XGuideMsgEnum nMsg, bool bEnter, MUID uidTarget )
{
	if (GUIDE_MSG_NA == nMsg)
	{
		_ASSERT(bEnter == false);
		if (m_CurState <=IDLE)
			return false;

		m_CurState = IDLE;
		return true;
	}

	XGuideMsgDesc* pGuideMsgDesc = GetMsgDesc(nMsg);
	if (pGuideMsgDesc != NULL)
	{
		if (pGuideMsgDesc->bLevelCheck)
		{
			if (!CheckLevel()) return false;
		}

		if (pGuideMsgDesc->bPollingCheck)
		{
			return UpdatePollingGuideStateAndCheck(pGuideMsgDesc->nState, bEnter, uidTarget);
		}
		else
		{
			return UpdateEventGuideStateAndCheck(bEnter);
		}
	}

	_ASSERT(0);
	return false;
}

bool XGuideMsg::UpdatePollingGuideStateAndCheck( const GUIDE_MSG_STATE _targetState, bool bEnter, MUID uidTarget )
{
	if (!bEnter)
	{
		if (m_uidPollingTarget == MUID::ZERO || m_uidPollingTarget != uidTarget)
		{
			return false;
		}
	}

	if ((m_CurState == _targetState) && (m_bEntered == bEnter)) 
	{
		return false;
	}

	m_bEntered = bEnter;
	m_CurState = _targetState;


	if (m_bEntered)
	{
		m_uidPollingTarget = uidTarget;
	}

	return true;
}


bool XGuideMsg::UpdateEventGuideStateAndCheck( bool bEnter )
{
	_ASSERT(bEnter == true);

	// 老馆利牢 PresentationLower客 悼老茄贸府
	NotifyOnPresentationLower();

	return true;
}

void XGuideMsg::NotifyOnPresentationLower()
{
	m_CurState = PRESENTATION_LOWER;
	m_nStateChangedTime = XGetNowTime();
}

bool XGuideMsg::IsEnabled()
{
	if(XGETCFG_GAME_SHOWHELPMESSAGE == false) return false;
	if (!global.script) return false;

	return true;
}

XGuideMsg::XGuideMsgDesc* XGuideMsg::GetMsgDesc(XGuideMsgEnum nMsg)
{
	XGuideMsgDescTable::iterator itor = m_DescTableMap.find(nMsg);
	if (itor != m_DescTableMap.end())
	{
		XGuideMsgDesc& desc = (*itor).second;
		return &desc;
	}
	return NULL;
}

bool XGuideMsg::CheckLevel()
{
	if (gvar.Network.IsServerModeExpo())
		return true;

	if (gvar.MyInfo.nLevel > GUIDE_MSG_LOW_LEVEL)
		return false;
	return true;
}
