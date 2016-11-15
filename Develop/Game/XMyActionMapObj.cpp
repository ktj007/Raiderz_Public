#include "StdAfx.h"
#include "XNonPlayer.h"
#include "XMyActionMapObj.h"
#include "XEventID.h"
#include "XModuleMyControl.h"
#include "XModuleAction.h"
#include "XModuleMovable.h"
#include "XModuleBuff.h"
#include "XPost_Interaction.h"
#include "XCharacter.h"
#include "XMyPlayer.h"
#include "XScreenFader.h"
#include "XGameState.h"
#include "XController.h"
#include "XCameraManager.h"
#include "XStrings.h"

XMyActionMapObj::XMyActionMapObj( XModuleMyControl* pOwner )
: XMyActionState(pOwner)
{
}

XMyActionMapObj::~XMyActionMapObj(void)
{
}

void XMyActionMapObj::Init()
{
	m_nMapObject = MAP_OBJECT_NONE;
	m_bMotionPlayOnce = false;
}

bool XMyActionMapObj::CheckEnterable( void* pParam )
{
	// 버프에 의한 상태 확인
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	if ( attrBuff.IsMesmerizable())
		return false;

	MUID targetUID = (pParam) ? *(MUID*)pParam : 0;
	if (targetUID == MUID::ZERO) return false;

	XNonPlayer* pNPC = gg.omgr->FindNPC(targetUID);
	if (pNPC == NULL)				return false;
	if (pNPC->GetInfo() == NULL)	return false;
	if (pNPC->IsDeSpawn())			return false;
	if (pNPC->GetInfo()->nNpcType != NPC_TYPE_OBJECT) return false;
	if (pNPC->GetIConState() == NIS_ITEM_USABLE) return false;

	m_MapObjUID = targetUID;

	return true;
}

void XMyActionMapObj::Enter( void* pParam /*= NULL*/ )
{
	XMyActionState::Enter(pParam);	//로그용.

	Init();
}

XEventResult XMyActionMapObj::Event( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_MOTION_PLAYONCE:
		{
			m_bMotionPlayOnce = true;
		}
		break;
	case XEVTD_ACTION_SLEEP:
		{
			XScreenFader* pScreenFader = gg.currentgamestate->GetFader();
			if (pScreenFader)	pScreenFader->FadeOutIn(1);
		}
		break;
	}

	return MR_FALSE;
}

bool XMyActionMapObj::CheckExitable( int nNextStateID )
{
	if(m_nMapObject == MAP_OBJECT_NONE)	return true;

	return false;
}

void XMyActionMapObj::Exit()
{
	XMyActionState::Exit();
}

void XMyActionMapObj::BeginSleep()
{
	MoveDummyPos(m_MapObjUID);

	ActionSleep();

	XEvent evt;
	evt.m_nID = XEVTD_ACTION_SLEEP;
	m_pOwner->GetOwner()->PostDelayEvent(evt, 3000.f);
}

void XMyActionMapObj::ActionSleep()
{
	m_nMapObject = MAP_OBJECT_BED;

	//XScreenFader* pScreenFader = gg.currentgamestate->GetFader();
	//if (pScreenFader)	pScreenFader->FadeOutIn(1);

	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (pModuleAction) pModuleAction->ActionBedSleep();
}

void XMyActionMapObj::EndSleep()
{
	XEvent evt;
	evt.m_nID = XEVTL_ON_MOTION_EVENT;
	evt.m_pData = MOTION_EVENT_NEXT_MOTION;
	m_pOwner->GetOwner()->PostDelayEvent(evt, 1);

	m_pOwner->OnEffectFatigueBest();
}

// 더미의 위치와 방향으로 캐릭터가 바라 보도록
void XMyActionMapObj::MoveDummyPos(MUID MapObjUID)
{
	XNonPlayer* pNPC = gg.omgr->FindNPC(MapObjUID);
	if (pNPC == NULL) return;

	vec3 vPos;
	RActorNode* pTargetActorNode = pNPC->GetActor()->GetActorNodePos(INTERACTION_DUMMY, vPos, E_TS_WORLD,true);
	_ASSERT(pTargetActorNode);
	if (pTargetActorNode == NULL)	return;

	vec3 vDir = pTargetActorNode->GetWorldDirection();

	XGetMyPlayer()->SetPosition(vPos);
	XGetMyPlayer()->SetDirection(vDir);

	gg.controller->SetBackViewCameraDir(vDir);

	m_pOwner->SyncPlayerDirToCameraDir();
}

void XMyActionMapObj::Update( float fDelta )
{
	XScreenFader* pScreenFader = gg.currentgamestate->GetFader();
	if (pScreenFader == NULL) return;

	pScreenFader->Update(fDelta);

	if (pScreenFader->IsActive() == false && m_bMotionPlayOnce == true)
	{
		m_nMapObject = MAP_OBJECT_NONE;
		m_pOwner->DoActionIdle();
	}
}
