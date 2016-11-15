#include "stdafx.h"
#include "MCommandCommunicator.h"
#include "MErrorTable.h"
#include <stdarg.h>
#include "MCommandStream.h"
#include "MCommandDesc.h"
#include "MPacket.h"
#include "MCommandBinder.h"
#include "MCommandProfiler.h"

namespace minet {

void MCommandCommunicator::ReceiveCommand(MCommand* pCommand)
{
	pCommand->m_Receiver = m_This;
	m_CommandManager.Post(pCommand);
}

MCommandResult MCommandCommunicator::OnCommand(MCommand* pCommand)
{
	const int nID = pCommand->GetID();

	assert(pCommand->m_pCommandDesc);

	return pCommand->Run();
}

MCommandCommunicator::MCommandCommunicator(void) : m_pCommandProfiler(NULL), m_nCrowdState(MCS_COMFORTABLE)
{
	m_uidGenerator.SetSeed(MUID(0xFFFFFFF0, 0));
	m_This.SetZero();
}

MCommandCommunicator::~MCommandCommunicator(void)
{
	Clear();
	if (m_pCommandProfiler)
	{
		delete m_pCommandProfiler;
		m_pCommandProfiler = NULL;
	}
}

bool MCommandCommunicator::Post(MCommand* pCommand)
{
	if(pCommand->CheckRule() == false) 
	{
		_ASSERT(0);
		return false;
	}

	// Send는 큐에 넣지 않고 바로 전송
	if ((pCommand->m_pCommandDesc->IsFlag(MCDT_MACHINE2MACHINE)==true) &&
		(pCommand->m_Receiver != m_This) )
	{
		if (m_pCommandProfiler)
		{
			m_pCommandProfiler->OnSend(pCommand);
		}

		SendCommand(pCommand);
		SAFE_DELETE(pCommand);
		return true;
	}

	if (m_CommandManager.Post(pCommand) == false)
	{
		SAFE_DELETE(pCommand);
	}

	return true;
}


MCommand* MCommandCommunicator::NewCommand(int nCmdID, MUID TargetUID)
{
	return new MCommand(nCmdID, m_This, TargetUID);
}

MCommand* MCommandCommunicator::NewLocalCommand( int nCmdID )
{
	return new MCommand(nCmdID, m_This, m_This);
}

MCommand* MCommandCommunicator::GetCommand()
{
	return m_CommandManager.GetCommand();
}


void MCommandCommunicator::Run(void)
{
	OnPrepareRun();
	OnCheckCrowded();

	while(1)
	{
		MCommand* pCommand = GetCommand();
		if(pCommand==NULL) break;
		
		OnCheckCrowded();


		if (IsCrowded())
		{
			// Crowded 상태가 되면, 필터처리 커멘드는 버린다.
			if (pCommand->m_pCommandDesc->IsFlag(MCDT_FILTER_CROWDED))
			{
				SAFE_DELETE(pCommand);
				continue;
			}		
		}
		else if (IsOverfull())
		{
			// Overfull 상태가 되면, 필터무시 커멘드가 아니면, 버린다.
			if (!pCommand->m_pCommandDesc->IsFlag(MCDT_FILTER_IGNORE))
			{
				SAFE_DELETE(pCommand);
				continue;
			}
		}
	

		OnPrepareCommand(pCommand);

		if ((pCommand->m_pCommandDesc->IsFlag(MCDT_PEER2PEER)==true))
		{
			if (pCommand->m_Sender != m_This)
			{
				if (m_pCommandProfiler)
				{
					m_pCommandProfiler->OnRecv(pCommand);
					m_pCommandProfiler->OnCommandBegin(pCommand);
				}

				if (OnCommand(pCommand) == CR_MOVED) continue;

				if (m_pCommandProfiler)
				{
					m_pCommandProfiler->OnCommandEnd(pCommand);
				}
			}
			else
			{
				if (m_pCommandProfiler)
					m_pCommandProfiler->OnSend(pCommand);

				SendCommand(pCommand);

				if (m_pCommandProfiler)
					m_pCommandProfiler->OnCommandBegin(pCommand);

				if (OnCommand(pCommand)==CR_MOVED) continue;

				if (m_pCommandProfiler)
					m_pCommandProfiler->OnCommandEnd(pCommand);
			}
		}
		else if ( (pCommand->m_pCommandDesc->IsFlag(MCDT_LOCAL)==true) || (pCommand->m_Receiver==m_This) )
		{
			if (m_pCommandProfiler)
			{
				m_pCommandProfiler->OnRecv(pCommand);
				m_pCommandProfiler->OnCommandBegin(pCommand);
			}

			if (OnCommand(pCommand)==CR_MOVED) continue;	// Local Command면 로컬에서 처리

			if (m_pCommandProfiler)
				m_pCommandProfiler->OnCommandEnd(pCommand);
		}
		else
		{
			if (m_pCommandProfiler)
				m_pCommandProfiler->OnSend(pCommand);

			SendCommand(pCommand);	// 그외에는 설정된 Receiver로 전송
		}

		SAFE_DELETE(pCommand);
	}

	OnRun();
}

void MCommandCommunicator::SetCommandHandler(int nCmdID, MCommandHandler* pHandler, MCommandHanderFunc* fnFunc)
{
	m_CommandManager.SetCommandHandler(nCmdID, pHandler, fnFunc);
}

void MCommandCommunicator::Clear()
{
	m_nCrowdState = MCS_COMFORTABLE;	

	while(MCommand* pCmd = GetCommand()) 
	{
		SAFE_DELETE(pCmd);
	}
}

MUID MCommandCommunicator::NewUID()
{
	cml2::MCriticalSection::Guard guard(m_csNewUID);
	return m_uidGenerator.Generate();
}

void MCommandCommunicator::InitProfiler()
{
	if (m_pCommandProfiler)
		m_pCommandProfiler->Init(&m_CommandManager);
}

void MCommandCommunicator::OnPrepareRun( void )
{
	m_nCrowdState = MCS_COMFORTABLE;	
}

void MCommandCommunicator::OnPrepareCommand( MCommand* pCommand )
{
	m_CommandHistory.Insert(pCommand);
}

void MCommandCommunicator::ProfileAnalysis(const char* szFolder, const char* szFileName, bool bSaveCSV)
{
	if (m_pCommandProfiler)
		m_pCommandProfiler->Analysis(szFolder, szFileName, bSaveCSV);
}

void MCommandCommunicator::SetCrowded()
{
	m_nCrowdState = MCS_CROWDED;
}

void MCommandCommunicator::SetOverfull()
{
	m_nCrowdState = MCS_OVERFULL;
}

bool MCommandCommunicator::IsOverfull()
{
	return m_nCrowdState == MCS_OVERFULL;
}

bool MCommandCommunicator::IsCrowded()
{
	return m_nCrowdState == MCS_CROWDED;
}

bool MCommandCommunicator::IsComfortable()
{
	return m_nCrowdState == MCS_COMFORTABLE;
}

} // namespace minet
