#ifndef _GDBTASK_INSERTCONNLOG_H
#define _GDBTASK_INSERTCONNLOG_H


#include "GDBAsyncTask.h"


//class GDBTaskConnLogInsert : public GDBAsyncTask
//{
//public :
//	GDBTaskConnLogInsert(const MUID& uidReqPlayer);
//	~GDBTaskConnLogInsert();
//
//
//	enum 
//	{
//		CONNLOG_INSERT = 0,
//	};
//
//	void					Input(const int nAID);
//
//
//	void					OnExecute(mdb::MDatabase& rfDB);
//	mdb::MDB_THRTASK_RESULT	_OnCompleted();
//
//	virtual SDBTASK_ID		GetID() override;
//
//
//private :
//	struct _DATA
//	{
//		MUID	uidPlayer;
//		int		nAID;
//	};
//
//	class Completer
//	{
//	public :
//		Completer(_DATA& Data);
//		void Do();
//
//	private :
//		void ImmediatelyUpdateDisconnTime();
//
//	private :
//		_DATA& m_Data;
//	};
//
//
//protected :
//	_DATA m_Data;
//};


#endif