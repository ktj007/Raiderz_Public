#ifndef _GDBCACHE_FLUSH_MGR_H
#define _GDBCACHE_FLUSH_MGR_H


class GPlayerDBCache;
class GCharacterDBCache;
class GDBTaskCacheFlush;
class GDBManager;
class GItem;
class GEntityPlayer;
class GItemDBCache;


class GDBCacheFlush
{
public :
	GDBCacheFlush(GPlayerDBCache* pPlayerCc);
	~GDBCacheFlush();

	enum FLUST_STATE
	{
		FLUSH_NONE = 0
	  , FLUSH_BUILD_DBCACHE_CHAR
	  , FLUSH_BUILD_DBCACHE_ITEM
	  , FLUSH_POST_TASK
	  , FLUSH_ROLLBACK
	};

	virtual bool Post();
	bool HasTask();
	
	FLUST_STATE GetLastState();

protected :
	void					SetState(const FLUST_STATE state);

	virtual bool			BuidDBCacheChar(GCharacterDBCache* cc);
	virtual bool			BuldDBCacheItem(GPlayerDBCache* pc);
	virtual bool			PostTask(GPlayerDBCache* pc, GDBTaskCacheFlush* pTask);
	virtual void			Rollback(GPlayerDBCache* pc);	
	GDBTaskCacheFlush*		NewTask(const MUID& uidPlayer);
	virtual GDBManager*		GetDBManager();

	GItem*					GetItem(GEntityPlayer* pPlayer, GItemDBCache* ic);

	bool					IsSameItem(GItem* pItem, GItemDBCache* ic);

protected :
	GPlayerDBCache*			m_pPlayerCc;
	GDBTaskCacheFlush*		m_pTask;
	CID						m_nCID;
	vector<IUID>			m_vIUID;
	vector<FLUST_STATE>		m_vState;
	bool					m_bPost;
};


#endif