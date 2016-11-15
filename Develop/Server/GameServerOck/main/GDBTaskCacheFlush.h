#ifndef _GDBTASK_CACHE_SYNC_H
#define _GDBTASK_CACHE_SYNC_H


#include "GDBAsyncTask.h"
#include "MMemPool.h"


class GPlayerDBCache;
class GObjectDBCache;

template <typename T>
class GCacheSyncQuery : public MMemPool<GCacheSyncQuery<T>>
{
public :
	GCacheSyncQuery() : m_ID(0), m_bSuccess(false) {}
	GCacheSyncQuery(const T ID, const wstring& strQuery) : m_ID(ID), m_strQuery(strQuery), m_bSuccess(false) {}
	~GCacheSyncQuery() {}

	T				GetID() { return m_ID; }
	const wstring&	GetQuery() { return m_strQuery; }

	void			SetSuccess() { m_bSuccess = true; }
	bool			IsSuccess() { return m_bSuccess; }

	void SetData(const T ID, const wstring& strQuery, const bool bSuccess = false)
	{
		m_ID		= ID;
		m_strQuery	= strQuery;
		m_bSuccess	= bSuccess;
	}	

protected :
	T		m_ID;
	wstring	m_strQuery;
	bool	m_bSuccess;
};

typedef GCacheSyncQuery<CID>	GCharSyncQuery;
typedef GCacheSyncQuery<IUID>	GItemSyncQuery;
typedef vector<GItemSyncQuery>	GITEM_SYNC_QUERY_VEC;

class GDBTaskCacheFlush : public GDBAsyncTask, public MTestMemPool<GDBTaskCacheFlush>
{
public :
	GDBTaskCacheFlush(const MUID& uidPlayer);
	~GDBTaskCacheFlush();

	void	OnExecute(mdb::MDatabase& rfDB) override;
	void	_OnEndCompleted() override;

	void	SyncCharCache(const CID nCID, CString& strSQL);
	void	SyncItemCache(const IUID nIUID, CString& strSQL);

	GCharSyncQuery&			GetCharQuery()		{ return m_CharSync; }
	GITEM_SYNC_QUERY_VEC&	GetItemQueryVec()	{ return m_vItemSync; }

protected :
	using GDBAsyncTask::PushSQL;

	void EndCacheSync(GObjectDBCache* pCacheObj, const bool bSuccess);
	virtual GPlayerDBCache* GetPlayerDBCache(const MUID& uidPlayer);

protected :
	GCharSyncQuery			m_CharSync;
	GITEM_SYNC_QUERY_VEC	m_vItemSync;
};



#endif