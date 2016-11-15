#include "StdAfx.h"
#include "XLuaLogFrm.h"
#include "XPath.h"

#define FRAME_CONSOLE_X			10
#define FRAME_CONSOLE_Y			10
#define FRAME_CONSOLE_WIDTH		400
#define FRAME_CONSOLE_HEIGHT	400

XLuaLogFrm::XLuaLogFrm(const char* szName, MWidget* pParent, MListener* pListener)
: mint3::MTextFrame(szName, pParent, pListener)
{
	SetRect(MRECT(FRAME_CONSOLE_X, FRAME_CONSOLE_Y, FRAME_CONSOLE_WIDTH, FRAME_CONSOLE_HEIGHT));
	SetOpacity(0.9f);
	SetText("WLuaLog");

	m_CmdLogger.Init(MLOG_LEVEL_DEBUG, MLOG_FILE, "WLuaLog.txt", NULL,
		MLocale::ConvUTF16ToAnsi(XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE).c_str()).c_str());
}

void XLuaLogFrm::ClearMessage()
{
	ResetContent();
}

void XLuaLogFrm::AddLog( const char* sLog )
{
	AddString(sLog);

	m_CmdLogger.Output(sLog);
	m_CmdLogger.Output("\n");
}
