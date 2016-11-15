#ifndef _GDBCACHE_SYSTEM_H
#define _GDBCACHE_SYSTEM_H


class GPlayerDBCache;
class GCharacterDBCache;
class GItemDBCache;
class GEntityPlayer;


typedef pair<GCharacterDBCache*, bool>	GSyncCharRes;
typedef pair<GItemDBCache*, bool>		GSyncItemRes;


class GDBCacheSystem : public MTestMemPool<GDBCacheSystem>
{
public :
	GDBCacheSystem() {}
	virtual ~GDBCacheSystem();

	GPlayerDBCache*			CreatePlayerCache(const MUID& uidPlayer, const CID nCID);
	void					DeletePlayerCache(const MUID& uidPlayer);
		
	GItemDBCache*			CreateItem(const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID);
	void					DeleteItem(const MUID& uidPlayer, const IUID nIUID);

	void					Release();
	virtual void			ReleasePlayerDBCacheMgr();
	virtual bool			FlushAllCache();

	GPlayerDBCache*			FindPlayerCache(const MUID& uidPlayer);
	GCharacterDBCache*		FindCharCache(const MUID& uidPlayer);
	GItemDBCache*			FindItemCache(const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID);

	void					SetItemSlot(const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID);

	size_t					GetItemCount(const MUID& uidPlayer);

	int16					GetItemCacheModAmt(const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID);
	int						GetCharCacheModMoney(const MUID& uidPlayer);
	
protected :
	virtual GEntityPlayer*	GetEntityPlayer(const MUID& uidPlayer);
};


#endif