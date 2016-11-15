#include "stdafx.h"
#include "LNetServer.h"
#include "LCommandTable.h"
#include "LCommandCenter.h"
#include "LCmdHandlerGroup.h"

LNetServer::LNetServer(const MNetServerDesc& desc) :MNetServer(desc)
, m_nLastCheckTime(0)
, m_nRecvCPS(0)
, m_nSendCPS(0)
, m_nLocalCPS(0)
{
	m_pCmdHandlerGroup = new LCmdHandlerGroup(this);
}

LNetServer::~LNetServer()
{
	SAFE_DELETE(m_pCmdHandlerGroup);
}


void LNetServer::OnPrepareCommand( MCommand* pCommand )
{
	minet::MNetServer::OnPrepareCommand(pCommand);
	LogCommand(pCommand);
}

void LNetServer::UpdateCmdCount(MCommand* pCommand)
{
	if (pCommand->m_pCommandDesc->IsFlag(MCDT_LOCAL))
	{
		m_nLocalTotalCmdCount++;m_nLocalCmdCnt++;
	}
	else if (pCommand->m_Receiver==m_This)
	{
		m_nRecvTotalCmdCount++;
		m_nRecvCmdCnt++;
	}
	else
	{
		m_nSendTotalCmdCount++;
		m_nSendCmdCnt += pCommand->GetReceiverCount();
	}

	unsigned int nNowTime = timeGetTime();
	if (nNowTime - m_nLastCheckTime >= 1000)
	{
		m_nLocalCPS = m_nLocalCmdCnt;
		m_nRecvCPS = m_nRecvCmdCnt;
		m_nSendCPS = m_nSendCmdCnt;
		m_nLocalCmdCnt = m_nRecvCmdCnt = m_nSendCmdCnt = 0;

		m_nLastCheckTime = nNowTime;
	}
}

void LNetServer::SendCommand( minet::MCommand* pCommand )
{
	MNetServer::SendCommand(pCommand);

	LogCommand(pCommand);
}

void LNetServer::LogCommand( minet::MCommand* pCommand )
{
	//if (gsys.pCommandCenter) gsys.pCommandCenter->LogCommand(pCommand);
	//UpdateCmdCount(pCommand);
}