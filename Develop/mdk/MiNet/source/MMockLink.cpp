#include "stdafx.h"
#include "MMockLink.h"
#include "MNetServer.h"
#include "MMockNetServer.h"

namespace minet {

void MMockLink::OnSendCommand( MCommand& command )
{
	MCommand* pCloneCommand = command.Clone();

	bool bIgnore = false;

	if (m_bIgnoreAll)
	{
		bIgnore = true;

		if (IsAllowedID(pCloneCommand->GetID()))
		{
			bIgnore = false;
		}
	}
	else if (IsIgnoredID(pCloneCommand->GetID()))
	{
		bIgnore = true;
	}

	if (bIgnore)
	{
		delete pCloneCommand;
		return;
	}

	m_vecRecevedCommands.push_back(pCloneCommand);
	m_Logger.LogCommand(true, &command);
}

MCommandResult MMockLink::OnRecvCommand( MCommand& command )
{
	m_Logger.LogCommand(false, &command);

	MMockNetServer* pMockNetServer = dynamic_cast<MMockNetServer*>(m_pNetServer);
	if (pMockNetServer)
	{
		return pMockNetServer->OnCommand(&command);
	}
	return CR_ERROR;
}

MMockLink::MMockLink( MUID uidNew, MNetServer* pNetServer, MServerTcpSocket& tcp_socket, MCommandManager* pCM ) 
: MLink(pNetServer->GetUID(), uidNew, tcp_socket, pCM)
, m_bIgnoreAll(false)
, m_pNetServer(pNetServer)
, m_bCallDisconnect(false)
, m_bCallHardDisconnect(false)
{

}

MMockLink::~MMockLink()
{
	for (size_t i = 0; i < m_vecRecevedCommands.size(); i++)
	{
		delete m_vecRecevedCommands[i];
	}
	m_vecRecevedCommands.clear();
	m_bIgnoreAll = false;
}

MCommandResult MMockLink::OnRecv( int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	MCommand command;
	command.SetID(nCmdID);
	command.SetReceiverUID(m_pNetServer->GetUID());
	command.SetSenderUID(GetUID());

	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */
	for (int i = 0; i < nParamCount; i++)
	{
		command.AddParameter(pParam);
		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */


	if(command.CheckRule() == false) 
	{
		_ASSERT(0);
		return CR_ERROR;
	}

	return OnRecvCommand(command);
}

MCommand& MMockLink::GetCommand( size_t nIndex )
{
	if (nIndex < 0 || nIndex >= m_vecRecevedCommands.size()) return m_NullCommand;
	return *m_vecRecevedCommands[nIndex];
}

int MMockLink::GetCommandID( size_t nIndex )
{
	if (nIndex < 0 || nIndex >= m_vecRecevedCommands.size()) return 0;
	return m_vecRecevedCommands[nIndex]->GetID();
}

size_t MMockLink::GetCommandCount()
{
	return m_vecRecevedCommands.size();
}

bool MMockLink::IsExistCommand( int nCmdID )
{
	for each (MCommand* pCmd in m_vecRecevedCommands)
	{
		if (pCmd->GetID() == nCmdID)
			return true;
	}
	return false;
}

void MMockLink::ResetCommands()
{
	m_Logger.Clear();

	for (size_t i = 0; i < m_vecRecevedCommands.size(); i++)
	{
		delete m_vecRecevedCommands[i];
	}
	m_vecRecevedCommands.clear();
}

void MMockLink::PopFrontCommand()
{
	if (m_vecRecevedCommands.empty())
		return;

	delete m_vecRecevedCommands.front();
	m_vecRecevedCommands.erase(m_vecRecevedCommands.begin());
}

void MMockLink::OutputLog(const char* szTitle)
{
	m_Logger.OutputLog(szTitle);
}

void MMockLink::AddIgnoreID( int nID )
{
	m_vecIgnoredID.insert(nID);
}

void MMockLink::EraseIgnoreID( int nID )
{
	m_vecIgnoredID.erase(nID);
}

bool MMockLink::IsIgnoredID( int nID )
{
	return (m_vecIgnoredID.find(nID) != m_vecIgnoredID.end());
}

void MMockLink::IgnoreAll()
{
	m_bIgnoreAll = true;
}

void MMockLink::ReleaseIgnoreAll()
{
	m_bIgnoreAll = false;
}

void MMockLink::AddAllowedID( int nID )
{
	m_vecAllowedID.insert(nID);
}

bool MMockLink::IsAllowedID( int nID )
{
	return (m_vecAllowedID.find(nID) != m_vecAllowedID.end());
}

//////////////////////////////////////////////////////////////
MMockLink::Logger::~Logger()
{
	Clear();
}

void MMockLink::Logger::Clear()
{
	for (size_t i = 0; i < m_Items.size(); i++)
	{
		delete m_Items[i].pCommand;
	}

	m_Items.clear();
}

void MMockLink::Logger::LogCommand( bool bSend, MCommand* pCommand )
{
	MCommand* pLogCommand = pCommand->Clone();

	_LogItem item;
	item.bSend = bSend;
	item.pCommand = pLogCommand;
	m_Items.push_back(item);
}

void MMockLink::Logger::OutputLog(const char* szTitle)
{
	const int LINE_LEN = 60;

	// 제목 출력
	char title[256] = "-- MockLink Commands Log";
	if (szTitle != NULL) 
	{
		sprintf_s(title, "%s(%s)", title, szTitle);
	}
	strcat_s(title, " ");

	mlog ("%s", title);
	for (size_t i = 0; i < LINE_LEN-strlen(title); i++)
	{
		mlog("-");
	}
	mlog("v\n");

	// 커맨드 내용 출력
	for (size_t i = 0; i < m_Items.size(); i++)
	{
		MCommand* pCmd = m_Items[i].pCommand;

		char msg[1024]="";
		char szParam[256];
		char szType[64] = "";
		if (m_Items[i].bSend) strcpy_s(szType, "SEND");
		else strcpy_s(szType, "RECV");

		//패킷내용.
		sprintf_s(msg, "[%d:%s] (%d) %s", i, szType, pCmd->m_pCommandDesc->GetID(), pCmd->m_pCommandDesc->GetName());

		if (pCmd->GetParameterCount() > 0)
		{
			sprintf_s(msg, "%s :", msg);
		}

		for(int param=0; param < pCmd->GetParameterCount(); param++)
		{
			pCmd->GetParameter(param)->GetString(szParam,sizeof(szParam));
			MUID uid = pCmd->GetSenderUID();

			sprintf_s(msg, "%s %s(%s)", msg, pCmd->GetParameter(param)->GetClassName(), szParam);
		}
		strcat_s(msg, "\n");
		mlog(msg);
	}

	// 끝라인 출력
	for (int i = 0; i < LINE_LEN; i++)
	{
		mlog("-");
	}
	mlog("^\n");
}


} // namespace minet