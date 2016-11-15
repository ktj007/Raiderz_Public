#include "stdafx.h"
#include "ZConsole.h"
#include "ZServer.h"
#include "ZCmdHandler_App.h"
#include "SCommandTable_App.h"
#include "ZCommandCenter.h"
#include "SAppHelper.h"
#include <tchar.h>
#include <atlconv.h>
#include <atlexcept.h>
#include "ZConst.h"

ZConsole::ZConsole() : 
ZBaseApplication(),
m_pAppCmdHandler(NULL)
{

}

ZConsole::~ZConsole()
{
	Destroy();
}

void ZConsole::OutputLogProc(const char* szText, MLogLevel nLevel)
{
	USES_CONVERSION_EX;

	DWORD dwCurrentThreadID = GetCurrentThreadId();
	if (ZConsole::GetInstance().IsMainThread(dwCurrentThreadID))
	{
		ZConsole::GetInstance().OutputLog(A2W_EX(szText, 0), nLevel);
	}
	else
	{
		if (NULL == gsys.pCommandCenter) return;
		SAppHelper::PostLogOutputCommand(gsys.pCommandCenter, gsys.pServer->GetUID(), A2W_EX(szText, 0), nLevel);
	}
}

void ZConsole::OutputLog(const wchar_t* szText, MLogLevel nLevel)
{
	wstring strDateTime = SAppHelper::MakeCurrentDateTimeString();
	wstring strText = strDateTime + L" ";
	strText += szText;

	SAppHelper::OutputLogToConsole(nLevel, strText.c_str());
}


bool ZConsole::OnCreate()
{
	MGetDefaultLogger()->SetCustomOutputCallback(ZConsole::OutputLogProc);

	if (ZBaseApplication::OnCreate() == false)
	{
		return false;
	}

	m_pAppCmdHandler = new ZCmdHandler_App(gsys.pCommandCenter->GetNetServer());

	if (ZConst::DEBUG_CONSOLE_SHOW_GAMETICK && ZConst::DEBUG_CONSOLE_GAMETICK_TICK > 0)
	{
		m_RegPrintServerTick.SetTickTime(float(ZConst::DEBUG_CONSOLE_GAMETICK_TICK));
		m_RegPrintServerTick.Start();
	}

	return true;
}

bool ZConsole::OnUpdate(float fDelta)
{
	if (!ZBaseApplication::OnUpdate(fDelta))
		return false;

	UpdatePrintServerTick(fDelta);

	return true;
}

void ZConsole::OnDestroy()
{
	mlog("ZConsole > OnDestroy \n");

	SAFE_DELETE(m_pAppCmdHandler);

	ZBaseApplication::OnDestroy();
}

void ZConsole::UpdatePrintServerTick( float fDelta )
{
	if (gsys.pServer == NULL) return;
	
	if (ZConst::DEBUG_CONSOLE_SHOW_GAMETICK && ZConst::DEBUG_CONSOLE_GAMETICK_TICK > 0)
	{
		if (m_RegPrintServerTick.IsReady(fDelta))
		{
			ZServerInfo& serverInfo = gsys.pServer->GetServerInfo();
			
			mlog("FPS= %d, Player= %d, LS/AS/GS= %d/%d/%d(%d), RecvBPS= %u, SendBPS= %u\n", 
				serverInfo.nFPS,
				serverInfo.nCurrPlayerCount,
				serverInfo.nIsLoginServerConnected,
				serverInfo.nCurrAppServerCount,
				serverInfo.nCurrGameServerCount,
				serverInfo.nMaxRequiredGameServerCount,
				serverInfo.nRecvBPS,
				serverInfo.nSendBPS
				);
		}
	}
}



