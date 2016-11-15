#ifndef _GDBTASK_MAIL_GET_LIST_H
#define _GDBTASK_MAIL_GET_LIST_H

class GEntityPlayer;

#include "GDBAsyncTask.h"
#include "MMemPool.h"

class GDBTaskMailGetList : public GDBAsyncTask, public MMemPool<GDBTaskMailGetList>
{
public :
	GDBTaskMailGetList(const MUID& uidReqPlayer);
	~GDBTaskMailGetList();

	enum
	{
		SQL_MAIL_GET_LIST = 0
	};

	bool					Input(int64 nReqMailUIDTop, int64 nReqMailUIDBottom, int nReqMailCount, bool bOpenMaibox=true);

	virtual void			OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

protected :
	struct _DATA_MAIL
	{
		int64	nMailUID;
		int		nType;
		bool	bReadFlag;
		wstring	strSender;
		wstring	strTitle;
		bool	bHasText;
		int		nAppendedMoney;
		int		nDefaultItemID;
		int		nRemainDelTime;
	};
	typedef vector<_DATA_MAIL> _MailVec;

	struct _DATA
	{
		MUID		uidPlayer;
		int64		nReqMailUIDTop;
		int64		nReqMailUIDBottom;
		int			nReqMailCount;
		bool		bOpenMaibox;

		_MailVec	vecDBMailList;

		_DATA() : uidPlayer(MUID::ZERO), nReqMailUIDTop(0), nReqMailUIDBottom(0), nReqMailCount(0), bOpenMaibox(false) {}
	};

	_DATA m_Data;

private:
	class Completer
	{
	public :
		Completer(_DATA& Data);
		void Do();
		void AssignMailList(GEntityPlayer* pPlayer);
		void RouteMailList(GEntityPlayer* pPlayer);
		void CheckRemainMailAtDB(GEntityPlayer* pPlayer);

	private :
		_DATA& m_Data;
	};

};


#endif