#ifndef _GDBASYNCTASK_EXEC_COUNTER_H
#define _GDBASYNCTASK_EXEC_COUNTER_H


#include "SDBTaskID.h"


#include <map>
#include <list>


class GDBTaskExecCounter
{
public :
	struct COUNT
	{
		COUNT() : nCnt(0), nWorkTm(0) {}

		int		nCnt;
		int64	nWorkTm;
	};

	typedef map<SDBTASK_ID, COUNT>		CountMap;
	typedef list<CountMap::value_type>	SortedList;

	GDBTaskExecCounter();
	~GDBTaskExecCounter();

	void		AddCount(const SDBTASK_ID nID, const int64 nWorkTm);
	COUNT		GetCount(const SDBTASK_ID nID);
	SortedList	GetDecrSortedList();


protected :
	CountMap m_mCounter;
};

typedef GDBTaskExecCounter::SortedList	GDBTaskSortedList;
typedef GDBTaskExecCounter::COUNT		GDBTaskCOUNT;

#endif