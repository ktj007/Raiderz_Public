#include "stdafx.h"
#include "XCursor.h"
#include "XBaseApplication.h"
#include "XSystem.h"

void XCursor::Show( bool bVisible )
{
	if (m_bVisible == bVisible) return;

	const uint32 nNowTime = XGetNowTime();

	if (bVisible)
	{
		// 3초 이내에 커서가 다시 생기면 예전 위치에서 생긴다.
		if (nNowTime - m_nLastTime <= 3000)
		{
			POINT pt;
			pt.x = m_ptLast.x;
			pt.y = m_ptLast.y;
			ClientToScreen(global.app->GetWindowHandle(), &pt);
			if (global.system) global.system->SetCursorPos(pt.x, pt.y);
		}

		ShowCursor(true);
	}
	else
	{
		ShowCursor(false);

		if (global.device && global.app)
		{
			POINT ptCurr;
			if (GetCursorPos(&ptCurr) == TRUE)
			{
				ScreenToClient(global.app->GetWindowHandle(), &ptCurr);
				m_ptLast.x = ptCurr.x;
				m_ptLast.y = ptCurr.y;
			}

			POINT pt;
			pt.x = global.device->GetScreenWidth()/2;
			pt.y = global.device->GetScreenHeight()/2;
			ClientToScreen(global.app->GetWindowHandle(), &pt);
			if (global.system) global.system->SetCursorPos(pt.x, pt.y);
		}


		Mint* pMint = MGetMint();
		if ( pMint != NULL)
			pMint->ResetMouseState();
	}
	m_bVisible = bVisible;
	m_nLastTime = nNowTime;
}

XCursor::XCursor()
{
	m_ptLast = MPoint(0,0);
	m_nLastTime = 0;
	m_bVisible = true;
}

void XCursor::ShowCursor( bool bVisible )
{
	if (global.system)
	{
		global.system->ShowCursor(bVisible);
	}
}