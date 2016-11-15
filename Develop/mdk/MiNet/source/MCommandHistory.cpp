#include "stdafx.h"
#include "MCommandHistory.h"

namespace minet {

void MCommandHistory::Insert( MCommand* pCommand )
{
	int nID = pCommand->GetID();
	m_List.push_back(nID);


	if ((int)m_List.size() > m_nCapacity)
	{
		m_List.erase(m_List.begin());
	}
}


} // namespace minet