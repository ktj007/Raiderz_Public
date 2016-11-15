#pragma once

#include "MTypes.h"
#include "MCommand.h"
#include <list>

namespace minet {


class MCommandHistory
{
private:
	int					m_nCapacity;
	std::list<int>		m_List;
public:
	MCommandHistory(int nCapacity=100) : m_nCapacity(nCapacity) {}
	~MCommandHistory() {}

	void Insert(MCommand* pCommand);
};














} // namespace minet