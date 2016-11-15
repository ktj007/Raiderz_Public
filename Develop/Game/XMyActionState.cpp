#include "stdafx.h"
#include "XMyActionState.h"
#include "XModuleMyControl.h"
#include "XMyPlayer.h"
#include "XPlayer.h"
#include "XEventID.h"
#include "XSpecialActionTransition.h"
#include "XModuleMotion.h"

static wstring _ActionStateStr[] = 
{
	L"ACTION_STATE_IDLE",
	L"ACTION_STATE_JUMP",
	L"ACTION_STATE_FALLING",
	L"ACTION_STATE_CHANGE_STANCE",
	L"ACTION_STATE_HIT",	
	L"ACTION_STATE_GUARD",
	L"ACTION_STATE_NORMAL_ATTACK",
	L"ACTION_STATE_USE_TALENT",
	L"ACTION_STATE_SHOOT_ARROW",
	L"ACTION_STATE_DIE",
	L"ACTION_STATE_USE_ITEM",
	L"ACTION_STATE_WEAPON_CHANGE",
	L"ACTION_STATE_LOOTINGITEM",
	L"ACTION_STATE_HOLD",
	L"ACTION_STATE_REBIRTH",
	L"ACTION_STATE_GRAPPLED",
	L"ACTION_STATE_MAPOBJ",
	L"ACTION_STATE_SIT",
	L"ACTION_STATE_CHANGE_CHANNEL",
	L"ACTION_STATE_QUEST_INTERACTION",
	L"ACTION_STATE_DEACTIVATIVE",
	L"ACTION_STATE_SWALLOWED",
	L"ACTION_STATE_GESTURE",
	L"ACTION_STATE_INTERACTION",
	L"ACTION_STATE_ENCHANT",
	L"ACTION_STATE_DYE"	
};

XPlayer* XMyActionState::GetOwnerPlayer()
{
	return static_cast<XPlayer*>(m_pOwner->GetOwner());
}

const wchar_t* XMyActionState::GetDebugName()
{
	int nID = GetID();
	return _ActionStateStr[nID].c_str();
}

void XMyActionState::Enter(void* pParam)
{
	int nID = GetID();
	if ((nID >= 0) && (nID < MAX_ACTION_STATE)) 
	{
//		mlog_filter("birdkr", "%s : Enter\n", _ActionStateStr[nID].c_str());
	}
}

void XMyActionState::Exit()
{
  	int nID = GetID();
	if ((nID >= 0) && (nID < MAX_ACTION_STATE)) 
	{
//		mlog_filter("birdkr", "%s : Exit\n", _ActionStateStr[nID].c_str());
	}
}

XEventResult XMyActionState::Event(XEvent& msg)
{
	return MR_FALSE;
}


void XMyActionState::Update( float fDelta )
{
	
}

void XMyActionState::PostUpdate( float fDelta )
{

}
////////////////////////////////////////////////////////////////////////////////////
// XMyActionFSM ////////////////////////////////////////////////////////////////////
bool XMyActionFSM::OnSpecialAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand )
{
	XMyActionState* pCurrentState = static_cast<XMyActionState*>(GetCurrentState());
	if (pCurrentState) return pCurrentState->OnSpecialAction(nSpecialActionCommand);
	return false;
}

XMyActionFSM::XMyActionFSM( XModuleMyControl* pOwner ) : XStateMachine<XModuleMyControl>(pOwner)
{
	
}

XMyActionFSM::~XMyActionFSM()
{
	
}