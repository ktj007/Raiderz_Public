#include "stdafx.h"
#include "MCommandBinder.h"

namespace minet {

void MCommandDistributer::BindCommandHandler(int nCmdID, MCommandHandlerCaller& info)
{
	_ASSERT(m_cmdHandlers.find(nCmdID)==m_cmdHandlers.end());
	m_cmdHandlers[nCmdID] = info;
}

MHandlerBase* MCommandDistributer::GetCommandHandlerInstance(int nCmdID)
{
	if(m_cmdHandlers.find(nCmdID)==m_cmdHandlers.end())
		return NULL;

	MCommandHandlerCaller& info = m_cmdHandlers[nCmdID];
	return info.GetHandlerInstance();
}

MCommandResult MCommandDistributer::Distribute( MCommand* pCommand )
{
	const int nCmdID = pCommand->GetID();

	// 새 테스트중인 핸들러
	if(m_cmdHandlers.find(nCmdID)!=m_cmdHandlers.end())
	{
		MCommandHandlerCaller& info = m_cmdHandlers[nCmdID];
		return info.OnCommand(pCommand);
	}

	return CR_FALSE;
}

}