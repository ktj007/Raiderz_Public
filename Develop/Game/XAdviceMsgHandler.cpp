#include "stdafx.h"
#include "XAdviceMsgHandler.h"
#include "XStrings.h"
#include "XGameFrameManager.h"

XAdviceMsgHandler::XAdviceMsgHandler()
{
	// do nothing
}

XAdviceMsgHandler::~XAdviceMsgHandler()
{
	// do nothing
}

bool XAdviceMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	int nSystemCode = m_nAdditional;
	const wchar_t* szMsg = XCmdResultStr(nSystemCode);

	if (global.gfmgr == NULL) return false;

	switch (global.gfmgr->GetCurrFrameID())
	{
	case PLAY_FRAME_ID:
		{
			global.ui->OnPresentationLower(szMsg, PRESENTATION_ICON_INFORMATION, true);
		}
		break;
	case CHAR_FRAME_ID:
		if ( global.script)
		{
			global.script->GetGlueGameEvent().OnCharacterEvent("CHARACTER_ERROR", MLocale::ConvUTF16ToAnsi(szMsg));
		}
		break;
	case LOGIN_FRAME_ID:
		if ( global.script)		
		{
			global.script->GetGlueGameEvent().OnLoginEvent("LOGIN_ERROR", MLocale::ConvUTF16ToAnsi(szMsg));
		}
		break;
	}

	return true;
}
