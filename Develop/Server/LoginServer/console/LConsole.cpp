#include "stdafx.h"
#include "LConsole.h"
#include "LServer.h"
#include "LCmdHandler_App.h"
#include "LCommandCenter.h"
#include "SAppHelper.h"

const float DEBUG_CONSOLE_OUTPUT_TIME_SEC = 60.0f;

LConsole::LConsole()
: LBaseApplication()
, m_pAppCmdHandler(NULL)
{
}

LConsole::~LConsole()
{
	Destroy();
}


void LConsole::OutputLogProc(const char* szText, MLogLevel nLevel)
{
	USES_CONVERSION_EX;

	DWORD dwCurrentThreadID = GetCurrentThreadId();
	if (LConsole::GetInstance().IsMainThread(dwCurrentThreadID))
	{		
		LConsole::GetInstance().OutputLog(A2W_EX(szText, 0), nLevel);
	}
	else
	{
		if (NULL == gsys.pCommandCenter) return;
		SAppHelper::PostLogOutputCommand(gsys.pCommandCenter, gsys.pServer->GetUID(), A2T_EX(szText, 0), nLevel);
	}
}

void LConsole::OutputLog(const wchar_t* szText, MLogLevel nLevel)
{
	wstring strDateTime = SAppHelper::MakeCurrentDateTimeString();
	wstring strText = strDateTime + L" ";
	strText += szText;

	SAppHelper::OutputLogToConsole(nLevel, strText.c_str());
}


bool LConsole::OnCreate()
{
	MGetDefaultLogger()->SetCustomOutputCallback(LConsole::OutputLogProc);

	if (LBaseApplication::OnCreate() == false)
	{
		return false;
	}

	m_pAppCmdHandler = new LCmdHandler_App(gsys.pCommandCenter->GetNetServer());

	m_RegPrintServerTick.SetTickTime(DEBUG_CONSOLE_OUTPUT_TIME_SEC);
	m_RegPrintServerTick.Start();

	return true;
}

bool LConsole::OnUpdate(float fDelta)
{
	if (!LBaseApplication::OnUpdate(fDelta))
		return false;

	UpdatePrintServerTick(fDelta);

	return true;
}

void LConsole::OnDestroy()
{
	mlog("LConsole > OnDestroy \n");

	SAFE_DELETE(m_pAppCmdHandler);

	LBaseApplication::OnDestroy();
}

void LConsole::UpdatePrintServerTick( float fDelta )
{
	if (gsys.pServer == NULL) return;

	if (m_RegPrintServerTick.IsReady(fDelta))
	{
		LServerInfo& serverInfo = gsys.pServer->GetServerInfo();

		mlog("FPS = %d , Link = %d , Player = %d , Recv BPS = %u , Send BPS = %u\n", 
			serverInfo.nFPS,
			serverInfo.nLinkCount,
			serverInfo.nCurrPlayerCount,
			serverInfo.nRecvBPS,
			serverInfo.nSendBPS
			);
	}
}

