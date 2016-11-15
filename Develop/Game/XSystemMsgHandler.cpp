#include "stdafx.h"
#include "XSystemMsgHandler.h"
#include "XStrings.h"
#include "XGameFrameManager.h"
#include "XSoundResMgr.h"

XSystemMsgHandler::XSystemMsgHandler()
{
	// do nothing
}

XSystemMsgHandler::~XSystemMsgHandler()
{
	// do nothing
}

bool XSystemMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	int nSystemCode = m_nAdditional;
	const wchar_t* szMsg = XCmdResultStr(nSystemCode);

	if (global.gfmgr == NULL) return false;

	switch (global.gfmgr->GetCurrFrameID())
	{
	case PLAY_FRAME_ID:
		{
			XCmdResultSound(nSystemCode);

			global.ui->OnSystemMsg(szMsg);
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
