#include "stdafx.h"
#include "SCommandCenter.h"
#include "CCommandTable.h"
#include "SCommandLogger.h"

SCommandCenter::SCommandCenter( MNetServer* netserver )
: m_pNetServer(netserver)
{
	m_pCommandLogger = new SCommandLogger();

	IgnoreDebugCommandLog();
}

SCommandCenter::~SCommandCenter()
{
	SAFE_DELETE(m_pCommandLogger);
}

MCommand* SCommandCenter::NewCommand( int nCmdID, MUID TargetUID )
{
	return m_pNetServer->NewCommand(nCmdID, TargetUID);
}

MCommand* SCommandCenter::NewCommand( int nCmdID )
{
	return m_pNetServer->NewCommand(nCmdID, 0);
}

void SCommandCenter::PostCommand( MCommand* pCommand )
{
	if (NULL == pCommand) return;

	if (pCommand->IsNoReceiver())
	{
		SAFE_DELETE(pCommand);
		return;
	}

	m_pNetServer->Post(pCommand);
}

void SCommandCenter::SendCommand( MUID uidTarget, MCommand* pCommand )
{
	pCommand->SetReceiverUID(uidTarget);
	PostCommand(pCommand);
}

void SCommandCenter::IgnoreDebugCommandLog()
{
	m_pCommandLogger->IgnoreCommand(MC_DEBUG_STRING);
	m_pCommandLogger->IgnoreCommand(MC_REQUEST_DEBUG_STRING);
}

void SCommandCenter::LogCommand( MCommand* pCmd )
{
#ifdef _PUBLISH
	return;
#endif

	m_pCommandLogger->Log(pCmd);
}

MCommand* SCommandCenter::MakeNewCommand( int nCmdID, MUID TargetUID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	MCommand* pNewCommand = this->NewCommand(nCmdID, TargetUID);

	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */

	for (int i = 0; i < nParamCount; i++)
	{
		pNewCommand->AddParameter(pParam);
		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	return pNewCommand;
}

MCommand* SCommandCenter::MakeNewCommand( int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	MCommand* pNewCommand = this->NewCommand(nCmdID, 0);

	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */
	for (int i = 0; i < nParamCount; i++)
	{
		pNewCommand->AddParameter(pParam);
		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	return pNewCommand;
}
