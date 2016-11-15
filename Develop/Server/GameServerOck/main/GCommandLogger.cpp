#include "stdafx.h"
#include "GCommandLogger.h"
#include "CCommandTable.h"
#include "GGlobal.h"
#include "GServer.h"
#include "GConst.h"


void GCommandLogger::Log(MCommand* pCmd)
{
#ifndef _DEBUG
	return;
#endif

	if (m_IgnoredCmdSet.find(pCmd->GetID()) != m_IgnoredCmdSet.end()) return;

	wchar_t msg[1024]=L"";	
	MUID uidReceiver, uidSender;

	//보내는지 받는지
	wchar_t trans[16] = L"[Recv]";
	uidReceiver = pCmd->GetReceiverUID();
	uidSender = pCmd->GetSenderUID();

	if (uidReceiver == uidSender) wcsncpy_s( trans, L"[Locl]", _TRUNCATE);
	else if( uidReceiver != gsys.pServer->GetUID() ) wcsncpy_s( trans, L"[Send]", _TRUNCATE );
	wcsncpy_s(msg, trans, sizeof(trans) - 1);

	//패킷내용.
	swprintf_s(msg, L"%s (%d)%s ", msg, pCmd->m_pCommandDesc->GetID(), MLocale::ConvAnsiToUCS2(pCmd->m_pCommandDesc->GetName()).c_str());
	for(int i=0; i<pCmd->GetParameterCount(); i++)
	{
		wstring strParam;
		pCmd->GetParameter(strParam, i, MPT_WSTR);		
		MUID uid = pCmd->GetSenderUID();
		swprintf_s(msg, L"%s %s(%s)", msg, MLocale::ConvAnsiToUCS2(pCmd->GetParameter(i)->GetClassName()).c_str(), strParam);
	}

	if (uidReceiver != uidSender)
	{
		if( uidSender == gsys.pServer->GetUID() )
		{
			swprintf_s(msg, L"%s , [To:(%u:%u)]", msg, uidReceiver.High, uidReceiver.Low);
		}
		else if (uidReceiver == gsys.pServer->GetUID() ) 
		{
			swprintf_s(msg, L"%s , [From:(%u:%u)]", msg, uidSender.High, uidSender.Low);
		}
	}

#ifdef _DEBUG
	if (GConst::LOG_PACKET)
	{
		wcscat_s(msg, L"\n");
		m_FileLogger.Output(msg);
	}
#endif

}

GCommandLogger::GCommandLogger(ICommandLogListener* pListener)
: m_pListener(pListener)
{
#ifdef _DEBUG
	if (GConst::LOG_PACKET)
	{
		m_FileLogger.Init(MLOG_LEVEL_DEBUG, MLOG_FILE, "command.txt");
	}
#endif
}

void GCommandLogger::IgnoreDebugCommand()
{
	IgnoreCommand(MC_DEBUG_STRING);
	IgnoreCommand(MC_REQUEST_DEBUG_STRING);
}

GCommandLogger::~GCommandLogger()
{
	if (m_pListener)
	{
		SAFE_DELETE(m_pListener);
	}
}