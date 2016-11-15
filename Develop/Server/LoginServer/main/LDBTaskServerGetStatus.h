#pragma once

#include "LDBAsyncTask.h"
#include "MMemPool.h"


class LDBTaskServerGetStatus : public LDBAsyncTask, public MMemPool<LDBTaskServerGetStatus>
{
public:
	LDBTaskServerGetStatus();
	~LDBTaskServerGetStatus();

	void					AssignServerID(const std::vector<int>& vecServerID);

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();

private :
	bool					GetServerStatusInfo(mdb::MDatabase& rfDB, int nSQLIndex);

private:
	struct _SERVER_STATUS_INFO
	{
		_SERVER_STATUS_INFO()
		{
			nServerID	= 0;
			bIsRunning	= false;
			bServable	= false;
		}

		int nServerID;
		bool bIsRunning;
		bool bServable;
	};

	struct _DATA
	{
		vector<_SERVER_STATUS_INFO> vecServerStatusInfo;
	};

	class Completer
	{
	public:
		Completer(_DATA& Data);
		void Do();

	private:
		_DATA m_Data;
	};

	std::vector<int>	m_vecServerID;
	_DATA				m_Data;
};
