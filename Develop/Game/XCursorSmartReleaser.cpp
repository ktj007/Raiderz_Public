#include "stdafx.h"
#include "XCursorSmartReleaser.h"
#include "XUISystem.h"
#include "XController.h"
#include "XMyPlayer.h"

void XCursorSmartReleaser::Update( float fDelta, XCursor* pCursor, XUISystem* pUISystem, XController* pController )
{
	if (IsUpdateEnabled(pCursor, pUISystem, pController) == false)
	{
		m_fElapsedTime = 0.0f;
		return;
	}

	m_fElapsedTime += fDelta;

	if (m_fElapsedTime >= CURSOR_SMART_RELEASE_TIME)
	{
		DoIt(pCursor, pUISystem);
		m_fElapsedTime = 0.0f;
	}
}

bool XCursorSmartReleaser::IsUpdateEnabled(XCursor* pCursor, XUISystem* pUISystem, XController* pController)
{
	if (pCursor->IsVisible() == false) return false;
	/*if (pUISystem->IsFocusMainInput() == false) return false;*/

	// ui 창이 있으면 발동하지 않는다.
	//if (pUISystem->GetWindowShowManager())
	//{
	//	if (pUISystem->GetWindowShowManager()->IsAllHided() == false) return false;
	//}

	// 채팅창 활성화 체크
	if (pUISystem->IsChattingBoxFocused() == true)
	{
		return false;
	}


	// 콘솔창 활성화 체크
	if (pUISystem->IsConsoleVisible() == true)
	{
		return false;
	}

	// 마우스 커서 토글 키 누르고 있는지 체크
	if (pUISystem->IsMouseCursorToggleKeyDown() == true)
	{
		return false;
	}

	if (gvar.Game.pMyPlayer)
	{
		if (gvar.Game.pMyPlayer->IsDead()) return false;
	}

	// 이동 키 체크
	if (pController == NULL) return false;

	bool bMovingKeyPressed = false;

	if (pController->IsVirtualKeyPressed(VKEY_FORWARD)) bMovingKeyPressed = true;
	else if (pController->IsVirtualKeyPressed(VKEY_BACKWARD)) bMovingKeyPressed = true;
	else if (pController->IsVirtualKeyPressed(VKEY_LEFT)) bMovingKeyPressed = true;
	else if (pController->IsVirtualKeyPressed(VKEY_RIGHT)) bMovingKeyPressed = true;

	if (bMovingKeyPressed == false) return false;

	return true;
}

void XCursorSmartReleaser::DoIt(XCursor* pCursor, XUISystem* pUISystem)
{
	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "UI", "HIDE_ALL_WINDOW");

/*
	if (pUISystem->GetWindowShowManager())
	{
		if (pUISystem->GetWindowShowManager()->IsAllHided() == false)
		{
			pUISystem->GetWindowShowManager()->HideAll();
		}
	}
*/

	pCursor->Show(false);

//	pUISystem->SetFocusMainInput();
}