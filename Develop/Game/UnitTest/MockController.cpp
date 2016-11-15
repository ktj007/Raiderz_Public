#include "stdafx.h"
#include "MockController.h"
#include "XGame.h"
#include "XMyPlayer.h"
#include "XModuleMyControl.h"

MockController::MockController() : m_vMockBackViewCameraDirection(vec3(0.0f, 1.0f, 0.0f))
{
	for (int i = 0; i < VKEY_COUNT; i++)
	{
		m_bVirtualKeyPressed[i] = false;
	}

	gg.controller = this;
}

void MockController::TestKeyPress( XVirtualKey nVKey, float fPressTime )
{
	m_bVirtualKeyPressed[nVKey] = true;

	PostEventToMyPlayer(XEVTL_VIRTUAL_KEY_PRESSEDONCE, nVKey);
	PostEventToMyPlayer(XEVTL_VIRTUAL_KEY_PRESSED, nVKey);

	if (fPressTime > 0.0f)
	{
		KeyNode keyNode(nVKey, fPressTime);

		m_PressKeyMap.insert(map<XVirtualKey, KeyNode>::value_type(nVKey, keyNode));
	}
}

void MockController::TestKeyRelease( XVirtualKey nVKey )
{
	m_bVirtualKeyPressed[nVKey] = false;

	PostEventToMyPlayer(XEVTL_VIRTUAL_KEY_RELEASED, nVKey);

	map<XVirtualKey, KeyNode>::iterator itor = m_PressKeyMap.find(nVKey);
	if (itor != m_PressKeyMap.end())
	{
		m_PressKeyMap.erase(itor);
	}
}

void MockController::TestKeyClick( XVirtualKey nVKey )
{
	PostEventToMyPlayer(XEVTL_VIRTUAL_KEY_PRESSEDONCE, nVKey);
	PostEventToMyPlayer(XEVTL_VIRTUAL_KEY_PRESSED, nVKey);
	PostEventToMyPlayer(XEVTL_VIRTUAL_KEY_RELEASED, nVKey);
}

bool MockController::IsVirtualKeyPressed( XVirtualKey nKey )
{
	return m_bVirtualKeyPressed[nKey];
}

bool MockController::IsVirtualKeyPressedAsync( XVirtualKey nKey )
{
	return m_bVirtualKeyPressed[nKey];
}

void MockController::SetBackViewCameraDir( const vec3& dir)
{
	m_vMockBackViewCameraDirection = dir;
}

vec3 MockController::GetBackViewCameraDir()
{
	return m_vMockBackViewCameraDirection;
}

#include "XCameraManager.h"

void MockController::TestSetCameraDir( vec3 dir )
{
	m_vMockBackViewCameraDirection = dir;

	if (global.camera && global.camera->GetCamera())
	{
		global.camera->GetCamera()->SetDirection(dir);
	}
}

void MockController::PostEventToMyPlayer( XEventID nEventID, XVirtualKey nVKey )
{
	XEvent msg;
	msg.m_nID = nEventID;
	msg.m_pData = &nVKey;
	if (gg.omgr)
		gg.omgr->PostEvent(XGetMyUID(), msg);
}

void MockController::TestSpecialKeyPress( SPECIAL_ACTION_COMMAND nSpecialActionCommand )
{
	XMyPlayer* pPlayer = gg.currentgamestate->GetObjectManager()->FindMyPlayer();
	pPlayer->GetModuleMyControl()->m_pActionFSM->OnSpecialAction(nSpecialActionCommand);
}

void MockController::OnUpdate( float fDelta )
{
	vector<XVirtualKey> vecReleaseKeys;

	for (map<XVirtualKey, KeyNode>::iterator itor = m_PressKeyMap.begin(); itor != m_PressKeyMap.end(); ++itor)
	{
		KeyNode& node = (*itor).second;
		node.fElapsedTime += fDelta;

		if (node.fElapsedTime >= node.fPressTime)
		{
			vecReleaseKeys.push_back(node.nKey);
		}
	}

	for (size_t i = 0; i < vecReleaseKeys.size(); i++)
	{
		TestKeyRelease(vecReleaseKeys[i]);
	}
}

void MockController::TestGuardAction()
{
	XMyPlayer* pPlayer = gg.currentgamestate->GetObjectManager()->FindMyPlayer();
	pPlayer->GetModuleMyControl()->DoAction(ACTION_STATE_GUARD);
}

void MockController::TestGuardRelease()
{
	XMyPlayer* pPlayer = gg.currentgamestate->GetObjectManager()->FindMyPlayer();
	XEvent evntMsg;
	evntMsg.m_nID = XEVTL_VIRTUAL_KEY_PRESSEDONCE;
	int nKey = VKEY_ACTION;
	evntMsg.m_pData = &nKey;
	pPlayer->Event(evntMsg);
}

void MockController::TestMouseMove( vec3 vPos )
{
	XMyPlayer* pPlayer = gg.currentgamestate->GetObjectManager()->FindMyPlayer();
	pPlayer->GetModuleMyControl()->SetMouseMove(vPos);
}