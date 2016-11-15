#include "stdafx.h"
#include "XSystem.h"

bool XSystem::Init()
{
	m_Timer.Reset();

	return true;
}

MUID XSystem::GenerateLocalUID()
{
	return m_UIDGenerator.Make();
}

void XSystem::ShowCursor( bool bShow )
{
// 	if (bShow)
// 	{
// 		::ShowCursor(TRUE);
// 	}
// 	else
// 	{
// 		::ShowCursor(FALSE);
// 	}

	// Move to Mint
	MGetMint()->ShowCursor( bShow);
}

void XSystem::SetCursorPos( int x, int y )
{
	::SetCursorPos(x, y);
}