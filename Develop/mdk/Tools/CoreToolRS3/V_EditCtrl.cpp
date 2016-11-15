#include "StdAfx.h"
#include "V_EditCtrl.h"

//////////////////////////////////////////////////////////////////////////
// enter 키 입력을 받기 위해 subclassing
CCoreToolEdit::CCoreToolEdit()
: CEdit()
, m_fnCallback(NULL)
{
}

BOOL CCoreToolEdit::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		if(m_fnCallback)
		{
			m_fnCallback();
			return TRUE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void CCoreToolEdit::SetCallback( EDIT_ENTER_HANDLER fnCallback)
{
	m_fnCallback = fnCallback;
}
