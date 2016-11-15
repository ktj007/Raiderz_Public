#pragma once

#include "GDBAsyncTask.h"
#include "MMemPool.h"

class GEntityPlayer;

class GDBTaskMailGetContent: public GDBAsyncTask, public MMemPool<GDBTaskMailGetContent>
{
public:
	GDBTaskMailGetContent(const MUID& uidReqPlayer);
	virtual ~GDBTaskMailGetContent();

	enum
	{
		SQL_MAIL_GET_TEXT = 0,
		SQL_MAIL_GET_APPENDED_ITEM_LIST
	};

	void					Input(MUID nMUID, bool bHasText);
	void					OnExecute(mdb::MDatabase& rfDB) override;

	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

private:
	void					_Execute_GetText(mdb::MDatabase& rfDB);
	void					_Execute_GetAppendedItem(mdb::MDatabase& rfDB);

private :
	struct _DATA
	{
		MUID	m_uidPlayer;
		MUID	m_uidMail;
		bool	m_bHasText;
	};
	_DATA	m_Data;

	struct _RESULT_MAIL_ITEM
	{
		int				m_nItemID;
		int64			m_nIUID;
		int				m_nSlotIndex;
		int				m_nColor;
		short			m_nStackAmt;
		unsigned char	m_nDura;
		unsigned char	m_nEnchCount;

		int				m_nEnchBufID1;
		int				m_nEnchBufID2;
		int				m_nEnchBufID3;
		int				m_nEnchBufID4;
		int				m_nEnchBufID5;
		int				m_nEnchBufID6;
	};
	typedef vector<_RESULT_MAIL_ITEM> _MailItemVec;

	struct _RESULT
	{
		// Result
		wstring			m_strText;
		_MailItemVec	m_vecAppendedItem;
	};
	_RESULT	m_Result;

private :
	class Completer
	{
	public :
		Completer(_DATA& Data, _RESULT& Result);
		void Do();

		bool SetMailRead(GEntityPlayer* pPlayer);
		void AssignMailAppendedItems(GEntityPlayer* pPlayer);
		void RouteMailReadRes(GEntityPlayer* pPlayer);

	private :
		_DATA&		m_Data;
		_RESULT&	m_Result;
	};
};
