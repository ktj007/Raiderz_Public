#include "stdafx.h"
#include "MCommandManager.h"
//#include "CMLexicalAnalyzer.h"
#include <algorithm>

namespace minet {

MCommandManager::MCommandManager(void) : m_nMaxQueueSize(DEFAULT_COMMAND_QUEUE_MAX_SIZE)
{

}

MCommandManager::~MCommandManager(void)
{
	Clear();
}

size_t MCommandManager::GetCommandQueueCount(void)
{
	cml2::MCriticalSection::Guard guard(&m_CommandQueueLock);
	return m_CommandQueue.size();
}

bool MCommandManager::Post(MCommand* pCmd)
{
	cml2::MCriticalSection::Guard guard(&m_CommandQueueLock);

	if (m_CommandQueue.size() >= m_nMaxQueueSize && pCmd->m_pCommandDesc)
	{
		if (!pCmd->m_pCommandDesc->IsFlag(MCDT_LOCAL) && 
			pCmd->m_pCommandDesc->GetFlow() == MCF_C2S)
		{
			return false;
		}
	}
		
	m_CommandQueue.push_back(pCmd);

	return true;
}

MCommand* MCommandManager::GetCommand(void)
{
	cml2::MCriticalSection::Guard guard(&m_CommandQueueLock);

	if(m_CommandQueue.empty())
	{
		return NULL;
	}

	MCommand* pCmd = *m_CommandQueue.begin();
	m_CommandQueue.erase(m_CommandQueue.begin());

	return pCmd;
}

void MCommandManager::Clear()
{
	cml2::MCriticalSection::Guard guard(&m_CommandQueueLock);

	for(MCommandList::iterator i=m_CommandQueue.begin(); i!=m_CommandQueue.end(); ++i)
	{
		SAFE_DELETE( (*i) );
	}
	m_CommandQueue.clear();
}

void MCommandManager::GetSyntax(char* szSyntax, int nSyntaxBufferSize, const MCommandDesc* pCD)
{
	sprintf_s(szSyntax, nSyntaxBufferSize, "%s ", pCD->GetName());
	for(size_t i=0; i<pCD->GetParameterDescCount(); i++)
	{
		MCommandParameterDesc* pPD = pCD->GetParameterDesc(i);
		sprintf_s(szSyntax, nSyntaxBufferSize, "%s %s", szSyntax, pPD->GetDescription());
	}
}

void MCommandManager::SetCommandHandler( int nCmdID, MCommandHandler* pHandler, MCommandHanderFunc* fnFunc )
{
	MCommandDesc* pCommandDesc = MGetCommandDescMap()->GetCommandDescByID(nCmdID);
	_ASSERT(NULL != pCommandDesc);
	if (NULL == pCommandDesc) return;

	pCommandDesc->m_pFunc = fnFunc;
	pCommandDesc->m_pHandler = pHandler;
}

} // namespace minet