#include "stdafx.h"
#include "GCommandLogger.h"
#include "GApplication.h"

void GCommandLogger::IgnoreCommand(int nID, bool bIgnore)
{
	if (bIgnore == true)
	{
		m_IgnoredCmdSet.insert(nID);
	}
	else
	{
		m_IgnoredCmdSet.erase(nID);
	}
}

void GCommandLogger::IgnoreCommandAll(bool bIgnore)
{
	if (bIgnore == true)
	{
		for (int i = 0; i < 10000; i++)
		{
			m_IgnoredCmdSet.insert(i);
		}
	}
	else
	{
		m_IgnoredCmdSet.clear();
	}
}

void GCommandLogger::Log(MCommand* pCmd)
{
#ifndef _DEBUG
	return;
#endif

	if (m_IgnoredCmdSet.find(pCmd->GetID()) != m_IgnoredCmdSet.end()) return;

	char msg[1024]="";
	char szParam[256];
	MUID uidReceiver, uidSender;

	//보내는지 받는지
	char trans[16] = "[Recv]";
	uidReceiver = pCmd->GetReceiverUID();
	uidSender = pCmd->GetSenderUID();
	if (uidReceiver == uidSender) strcpy_s( trans, "[Locl]" );
	else if( uidReceiver != gsys.pServer->GetUID() ) strcpy_s( trans, "[Send]" );
	strcpy_s(msg, trans);

	//패킷내용.
	sprintf_s(msg, "%s (%d)%s ", msg, pCmd->m_pCommandDesc->GetID(), pCmd->m_pCommandDesc->GetName());
	for(int i=0; i<pCmd->GetParameterCount(); i++){
		pCmd->GetParameter(i)->GetString(szParam,sizeof(szParam));
		MUID uid = pCmd->GetSenderUID();
		sprintf_s(msg, "%s %s(%s)", msg, pCmd->GetParameter(i)->GetClassName(), szParam);

	}

	if (GApplication::GetInstancePtr())
		GApplication::GetInstancePtr()->LogCommand(msg);

}