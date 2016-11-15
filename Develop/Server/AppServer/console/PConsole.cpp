#include "stdafx.h"
#include "PConsole.h"
#include "PServer.h"
#include "PCmdHandler_App.h"
#include "SCommandTable_App.h"
#include "PCommandCenter.h"
#include "SAppHelper.h"


PConsole::PConsole() 
: m_pAppCmdHandler(NULL)
{

}

PConsole::~PConsole()
{
	Destroy();
}

void PConsole::OutputLogProc(const char* szText, MLogLevel nLevel)
{
	USES_CONVERSION_EX;

	DWORD dwCurrentThreadID = GetCurrentThreadId();
	if (PConsole::GetInstance().IsMainThread(dwCurrentThreadID))
	{		
		PConsole::GetInstance().OutputLog(A2W_EX(szText, 0), nLevel);
	}
	else
	{
		if (NULL == gsys.pCommandCenter) return;
		SAppHelper::PostLogOutputCommand(gsys.pCommandCenter, gsys.pServer->GetUID(), A2CT_EX(szText, 0), nLevel);
	}
}

void PConsole::OutputLog(const wchar_t* szText, MLogLevel nLevel)
{
	wstring strDateTime = SAppHelper::MakeCurrentDateTimeString();
	wstring strText = strDateTime + L" ";
	strText += szText;

	SAppHelper::OutputLogToConsole(nLevel, strText.c_str());
}

bool PConsole::OnCreate()
{
	MGetDefaultLogger()->SetCustomOutputCallback(PConsole::OutputLogProc);

	if (PBaseApplication::OnCreate() == false)
	{
		return false;
	}

	m_pAppCmdHandler = new PCmdHandler_App(gsys.pCommandCenter->GetNetServer());

	return true;
}

void PConsole::OnDestroy()
{
	SAFE_DELETE(m_pAppCmdHandler);

	PBaseApplication::OnDestroy();
}



