#include "stdafx.h"
#include "GNetServer.h"
#include "GCommandTable.h"
#include "GCommandCenter.h"
#include "GCmdHandler_Log.h"

GNetServer::GNetServer(const MNetServerDesc& desc) :MNetServer(desc)
, m_nLastCheckTime(0)
, m_nRecvCPS(0)
, m_nSendCPS(0)
, m_nLocalCPS(0)
{
	m_pLogCmdHandler = new GCmdHandler_Log(this);
}

GNetServer::~GNetServer()
{
	SAFE_DELETE(m_pLogCmdHandler);
}


void GNetServer::OnPrepareCommand( MCommand* pCommand )
{
	minet::MNetServer::OnPrepareCommand(pCommand);
	LogCommand(pCommand);
}

void GNetServer::UpdateCmdCount(MCommand* pCommand)
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

	unsigned int nNowTime = gsys.pSystem->GetNowTime();
	if (nNowTime - m_nLastCheckTime >= 1000)
	{
		m_nLocalCPS = m_nLocalCmdCnt;
		m_nRecvCPS = m_nRecvCmdCnt;
		m_nSendCPS = m_nSendCmdCnt;
		m_nLocalCmdCnt = m_nRecvCmdCnt = m_nSendCmdCnt = 0;

		m_nLastCheckTime = nNowTime;
	}
}

void GNetServer::SendCommand( minet::MCommand* pCommand )
{
	MNetServer::SendCommand(pCommand);

	LogCommand(pCommand);
}

void GNetServer::LogCommand( minet::MCommand* pCommand )
{
	if (gsys.pCommandCenter) gsys.pCommandCenter->LogCommand(pCommand);
	UpdateCmdCount(pCommand);
}