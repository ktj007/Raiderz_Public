#include "StdAfx.h"
#include "LPlayerAcceptManager.h"
#include "LPlayerAcceptor_FromGameServer.h"
#include "SDef.h"
#include "LGlobal.h"
#include "LSystem.h"


LPlayerAcceptManager::LPlayerAcceptManager()
{
}

LPlayerAcceptManager::~LPlayerAcceptManager()
{
	SAFE_DELETE_MAP(m_AcceptorMap);
}

void LPlayerAcceptManager::Update()
{
	if (m_AcceptorMap.empty()) return;

	uint32 nNowTime = gsys.pSystem->GetNowTime();

	for (LPlayerAcceptorMap::iterator itor = m_AcceptorMap.begin(); itor != m_AcceptorMap.end(); )
	{
		LPlayerAcceptor_FromGameServer* pAcceptor = (*itor).second;

		if (nNowTime - pAcceptor->GetReserveTime() >= TIMEOUT_TICK_MOVE_SERVER)
		{
			pAcceptor->OnTimeout();
			SAFE_DELETE(pAcceptor);
			itor = m_AcceptorMap.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

bool LPlayerAcceptManager::Add(LPlayerAcceptor_FromGameServer* pAcceptor)
{
	RVALID_RET(pAcceptor != NULL, false);
	RVALID_RET(pAcceptor->GetConnectionKey().IsValid(), false);

	if (FindAcceptor(pAcceptor->GetConnectionKey()) != NULL)
		return false;

	m_AcceptorMap.insert(LPlayerAcceptorMap::value_type(pAcceptor->GetConnectionKey(), pAcceptor));
	return true;
}

void LPlayerAcceptManager::Del(MUID uidConnectionKey)
{
	LPlayerAcceptorMap::iterator itr = m_AcceptorMap.find(uidConnectionKey);
	if (itr == m_AcceptorMap.end())
		return;

	SAFE_DELETE(itr->second);
	m_AcceptorMap.erase(uidConnectionKey);
}

LPlayerAcceptor_FromGameServer* LPlayerAcceptManager::FindAcceptor(MUID uidConnectionKey)
{
	LPlayerAcceptorMap::iterator itr = m_AcceptorMap.find(uidConnectionKey);
	if (itr == m_AcceptorMap.end())
		return NULL;

	return itr->second;
}


size_t LPlayerAcceptManager::GetSize()
{
	return m_AcceptorMap.size();
}
