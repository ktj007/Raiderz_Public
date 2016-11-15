#include "stdafx.h"
#include "MDelayTester.h"
#include "MCommandManager.h"

namespace minet {

MDelayTester::MDelayTester() : m_nDelayTime(0)
{

}

MDelayTester::~MDelayTester()
{
	cml2::MCriticalSection::Guard guard(&m_Lock);

	for(DelayCommandList::iterator itor=m_CommandList.begin(); itor!=m_CommandList.end(); ++itor)
	{
		MCommand* pCommand = (*itor).pCommand;
		SAFE_DELETE(pCommand);
	}
	m_CommandList.clear();
}

void MDelayTester::SetDelay( uint32 nDelayTime )
{
	m_nDelayTime = nDelayTime;
}

void MDelayTester::Post( MCommand* pCommand, uint32 nNowTime )
{
	_Command _cmd;
	_cmd.nTime = nNowTime;
	_cmd.pCommand = pCommand;

	{
		cml2::MCriticalSection::Guard guard(&m_Lock);

		m_CommandList.push_back(_cmd);
	}
}

void MDelayTester::OnRun( MCommandManager& cm, uint32 nNowTime )
{
	for(DelayCommandList::iterator itor=m_CommandList.begin(); itor!=m_CommandList.end(); )
	{
		_Command& _cmd = (*itor);
		if (nNowTime - _cmd.nTime >= m_nDelayTime)
		{
			cm.Post(_cmd.pCommand);
			itor = m_CommandList.erase(itor);
		}
		else return;
	}
}

} // namespace minet