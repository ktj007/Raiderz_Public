#include "stdafx.h"
#include "MMockNetClient.h"

namespace minet {

MMockNetClient::MMockNetClient( const MNetClientDesc& desc )
: MNetClient(desc)
, m_pAgent(NULL)
, m_bIsConnected(false)
{

}


bool MMockNetClient::Post( MCommand* pCommand )
{
	if (m_pAgent) m_pAgent->OnSend(pCommand);

	SAFE_DELETE(pCommand);
	return true;
}

void MMockNetClient::LinkToAgent( MTestNetAgent* pAgent )
{
	m_pAgent = pAgent;
}

void MMockNetClient::UnlinkToAgent()
{
	m_pAgent = NULL;
}

minet::MCommandResult MMockNetClient::OnCommand( MCommand* pCommand )
{
	return MNetClient::OnCommand(pCommand);
}


MTestNetAgent::Logger::~Logger()
{
	Clear();
}

void MTestNetAgent::Logger::LogCommand( bool bSend, minet::MCommand* pCommand )
{
	minet::MCommand* pLogCommand = pCommand->Clone();

	_LogItem item;
	item.bSend = bSend;
	item.pCommand = pLogCommand;
	m_Items.push_back(item);
}

void MTestNetAgent::Logger::OutputLog(const char* szTitle)
{
	const int LINE_LEN = 60;

	// 제목 출력
	char title[256] = "-- NetClient Commands Log";
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
		minet::MCommand* pCmd = m_Items[i].pCommand;

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

void MTestNetAgent::Logger::Clear()
{
	for (size_t i = 0; i < m_Items.size(); i++)
	{
		delete m_Items[i].pCommand;
	}
	m_Items.clear();
}

MTestNetAgent::MTestNetAgent( MNetClient* pNetClient ) : m_pMockNetClient(NULL)
{
	if (pNetClient)
		m_pMockNetClient = dynamic_cast<MMockNetClient*>(pNetClient);

	if (m_pMockNetClient == NULL) return;
	m_pMockNetClient->LinkToAgent(this);
}

MTestNetAgent::~MTestNetAgent()
{
	ResetSendCommands();

	if (m_pMockNetClient)
		m_pMockNetClient->UnlinkToAgent();
}

void MTestNetAgent::OnSend( MCommand* pCommand )
{
	if (IsIgnoreCmdID(pCommand->GetID())) return;

	MCommand* pClone = pCommand->Clone();
	m_vecSentCommands.push_back(pClone);
	m_Logger.LogCommand(true, pCommand);
}

minet::MCommand& MTestNetAgent::GetSendCommand( size_t nIndex )
{
	if (nIndex < 0 || nIndex >= m_vecSentCommands.size()) return m_NullCommand;
	return *m_vecSentCommands[nIndex];
}

size_t MTestNetAgent::GetSendCommandCount()
{
	return m_vecSentCommands.size();
}

bool MTestNetAgent::IsExistSendCommand( int nCmdID )
{
	for each (minet::MCommand* pCmd in m_vecSentCommands)
	{
		if (pCmd->GetID() == nCmdID)
			return true;
	}
	return false;
}

void MTestNetAgent::ResetSendCommands()
{
	for (size_t i = 0; i < m_vecSentCommands.size(); i++)
	{
		delete m_vecSentCommands[i];
	}
	m_vecSentCommands.clear();
	m_Logger.Clear();
}

void MTestNetAgent::OutputLog(const char* szTitle)
{
	m_Logger.OutputLog(szTitle);
}

MCommandResult MTestNetAgent::OnRecv( int nCmdID, int nParamCount, minet::MCommandParameter* pCmdParam, ... )
{
	if (m_pMockNetClient == NULL) return CR_FALSE;

	MUID uidTarget= m_pMockNetClient->GetServerUID();
	auto_ptr<MCommand> pNewCommand(m_pMockNetClient->NewCommand(nCmdID, uidTarget));

	va_list marker;
	minet::MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */
	for (int i = 0; i < nParamCount; i++)
	{
		pNewCommand->AddParameter(pParam);
		pParam = va_arg( marker, minet::MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	if(pNewCommand->CheckRule() == false) 
	{
		_ASSERT(0);
		return CR_ERROR;
	}

	m_Logger.LogCommand(false, pNewCommand.get());
	return m_pMockNetClient->OnCommand(pNewCommand.get());
}

MCommandResult MTestNetAgent::OnRecv( MCommand* pCommand )
{
	auto_ptr<MCommand> pRecvCommand(pCommand);

	MUID uidTarget = m_pMockNetClient->GetServerUID();
	pCommand->SetReceiverUID(m_pMockNetClient->GetUID());
	pCommand->SetSenderUID(uidTarget);

	if(pCommand->CheckRule() == false) 
	{
		_ASSERT(0);
		return CR_ERROR;
	}

	m_Logger.LogCommand(false, pCommand);
	return m_pMockNetClient->OnCommand(pCommand);
}

void MTestNetAgent::IgnoreCmd( int nCmdID )
{
	m_setIgnoreCmdID.insert(nCmdID);
}

void MTestNetAgent::ResetIgnoreCmd()
{
	return m_setIgnoreCmdID.clear();
}

int MTestNetAgent::GetSendCommandID( size_t nIndex )
{
	if (nIndex < 0 || nIndex >= m_vecSentCommands.size()) return 0;
	return m_vecSentCommands[nIndex]->GetID();

}

MUID MTestNetAgent::GetServerUID()
{
	return m_pMockNetClient->GetServerUID();
}

MUID MTestNetAgent::GetUID()
{
	return m_pMockNetClient->GetUID();
}
} // namespace minet