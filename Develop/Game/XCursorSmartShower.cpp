#include "stdafx.h"
#include "XCursorSmartShower.h"
#include "XUISystem.h"

bool XCursorSmartShower::IsMouseCursorVisible()
{
	if (global.ui)
	{
		return global.ui->IsMouseVisible();	
	}
	return false;
}

void XCursorSmartShower::ShowCursor( bool bVisible )
{
	if (global.ui)
	{
		global.ui->ShowMouse(bVisible);	
	}
}

bool XCursorSmartShower::OnKeyDown()
{
	if (m_bNowKeyDown)
	{
		return false;
	}

	m_bNowKeyDown = true;

	ShowCursor( ! IsMouseCursorVisible() );

	return true;
}

void XCursorSmartShower::OnKeyUp()
{
	m_bNowKeyDown = false;
}