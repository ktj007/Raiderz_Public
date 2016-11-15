#include "StdAfx.h"
#include "GMonitorPortBase.h"
#include "MProcessController.h"
#include <ShellAPI.h>

bool GMonitorPortBase::ToViewerSender::Send( const wchar_t* szName, string strMessage )
{
	HWND DestHwnd = FindWindow(NULL, szName);
	if( !DestHwnd )	
		return false;

	HWND SourceHwnd = NULL;

	COPYDATASTRUCT SendData;
	SendData.dwData = 0;
	SendData.cbData = strMessage.length()+1;
	SendData.lpData = (void*)strMessage.c_str();

	SendMessage( DestHwnd,WM_COPYDATA,(WPARAM)SourceHwnd,(LPARAM)(LPVOID)&SendData);

	return true;
}



GMonitorPortBase::GMonitorPortBase(void)
: m_pSender(NULL)
{
	SetSender(new ToViewerSender());
}

GMonitorPortBase::~GMonitorPortBase(void)
{
	SAFE_DELETE(m_pSender);
}

void GMonitorPortBase::Send( string strMessage )
{
	if (!IsEnable())
		return;

	if (!m_pSender)
		return;

	if (!m_pSender->Send(GetName(), strMessage))
	{
		OnSendFailed();
	}
}

void GMonitorPortBase::LaunchMonitor()
{
	if (!IsEnable())
		return;

	if( FindWindow(NULL, GetName()) ) 
		return;

	
	char szPath[MAX_PATH];
	if (!GetModuleFileNameA(NULL, szPath, MAX_PATH))
		return;
	
	ShellExecuteA(0, NULL, GetFilePath(), "", NULL, SW_NORMAL);
}

void GMonitorPortBase::SetSender( Sender* pSender )
{
	SAFE_DELETE(m_pSender);
	m_pSender = pSender;
}

