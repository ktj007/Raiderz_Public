#include "stdafx.h"
#include "GDBTaskExecCounter.h"

GDBTaskExecCounter::GDBTaskExecCounter()
{

}

GDBTaskExecCounter::~GDBTaskExecCounter()
{

}

void GDBTaskExecCounter::AddCount( const SDBTASK_ID nID, const int64 nWorkTm )
{
	CountMap::iterator it = m_mCounter.find(nID);
	if (m_mCounter.end() == it)
	{
		COUNT c;

		c.nCnt		= 1;
		c.nWorkTm	= nWorkTm;

		m_mCounter.insert(CountMap::value_type(nID, c));
		return;
	}

	it->second.nCnt++;
	it->second.nWorkTm += nWorkTm;
}

GDBTaskExecCounter::COUNT GDBTaskExecCounter::GetCount( const SDBTASK_ID nID )
{
	CountMap::iterator it = m_mCounter.find(nID);
	if (m_mCounter.end() == it)
		return COUNT();

	return it->second;
}

GDBTaskExecCounter::SortedList GDBTaskExecCounter::GetDecrSortedList()
{
	SortedList sl;

	if (m_mCounter.empty())
		return sl;

	for (CountMap::iterator it = m_mCounter.begin(); it != m_mCounter.end(); ++it)
	{
		SortedList::iterator iit = sl.begin();
		for (; iit != sl.end(); ++iit)
		{
			if (iit->second.nCnt < it->second.nCnt)
			{
				sl.insert(iit, *it);
				break;
			}
		}

		if (sl.end() == iit)
			sl.push_back(*it);
	}

	return sl;
}