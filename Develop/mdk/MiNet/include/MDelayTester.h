#pragma once

#include "MCommand.h"
#include "MThread2.h"

namespace minet {

class MDelayTester
{
private:
	struct _Command
	{
		uint32			nTime;
		MCommand*		pCommand;
	};
	typedef list<_Command>		DelayCommandList;

	uint32						m_nDelayTime;
	cml2::MCriticalSection		m_Lock;
	DelayCommandList			m_CommandList;
public:
	MDelayTester();
	~MDelayTester();
	void SetDelay(uint32 nDelayTime);
	void Post(MCommand* pCommand, uint32 nNowTime);
	void OnRun(MCommandManager& cm, uint32 nNowTime);
};


} // namespace minet