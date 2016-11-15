#ifndef _GSYNCING_CACHE_LIST_H
#define _GSYNCING_CACHE_LIST_H


class GObjectSyncingCache;
class GCharacterSyncingCache;
class GItemSyncingCache;


class GSyncingCacheList
{
public :
	GSyncingCacheList(const MUID& uidPlayer) : m_uidPlayer(uidPlayer) {}
	~GSyncingCacheList();

	typedef vector<GObjectSyncingCache*> SYNCING_CACHE_VEC;

	void					Reserve(const size_t nSize) { m_vCache.reserve(nSize); }
	size_t					GetSize()					{ return m_vCache.size(); }

	GCharacterSyncingCache*	PushChar();
	GItemSyncingCache*		PushItem(const IUID nIUID);

	void					EndCacheSync(const bool bZero);
	void					Release();

	void					SetPlayer(const	MUID& uidPlayer);
	MUID					GetPlayer() { return m_uidPlayer; }

protected :
	MUID					m_uidPlayer;
	SYNCING_CACHE_VEC		m_vCache;
};


#endif