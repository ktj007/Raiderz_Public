#ifndef _GDBCACHE_ITEM_H
#define _GDBCACHE_ITEM_H


#include "GObjectDBCache.h"
#include "cache_value_type.h"
#include "MMemPool.h"

const static size_t	DBCACHE_ITEM_MAX_MOD_COUNT	= 5;
const static int	DBCACHE_MAX_MOD_STACKAMT	= DBCACHE_SKEEP;
const static int	DBCACHE_MAX_MOD_DURA		= DBCACHE_SKEEP;

class GItemDBCache : public GObjectDBCache
{
public :
	GItemDBCache(const MUID& uidPlayer, const IUID nIUID, const size_t nMaxModCount = DBCACHE_ITEM_MAX_MOD_COUNT
		, const size_t nMaxModStackAmt = DBCACHE_MAX_MOD_STACKAMT, const size_t nMaxModDura = DBCACHE_MAX_MOD_DURA);
	virtual ~GItemDBCache() {}

	IUID				GetIUID();

	bool				CheckValidModifyCnt(const size_t nStackAmtModCnt, const size_t nDuraModCnt);
	bool				CheckVaildModifyStackAmt(const bool bAutoSync);
	bool				CheckVaildModifyDura(const bool bAutoSync);
	
	bool				IsModified();
	void				SetZero() override;

	cache_StackAmt&		GetStackAmt();
	cache_Dura&			GetDura();

	bool				IncStackAmt(const int16 nIncAmt, const bool bAutoSync = true);
	bool				DecStackAmt(const int16 nDecAmt, const bool bAutoSync = true);

	bool				IncDura(const uint8 nIncDura, const bool bAutoSync = true);
	bool				DecDura(const uint8 nDecDura, const bool bAutoSync = true);

	void				SetMaxModifyCnt(const size_t nMaxModCount);

	void				SetSlot(const uint8 nSlotType, const int16 nSlotID);
	uint8				GetSlotType();
	int16				GetSlotID();

	int16				GetModAmt();


protected :
	virtual bool		IsOverflowModifiedRange() override;
	virtual bool		IsOverflowModifiedCnt() override;

	size_t				CalcModifiedCnt();
	void				StartCaching();

protected :
	const IUID			m_nIUID;
	uint8				m_nSlotType;
	int16				m_nSlotID;
	cache_StackAmt		m_cacheStackAmt;
	cache_Dura			m_cacheDura;
	size_t				m_nMaxModCount;
};


class GItemDBCache_Leaf : public GItemDBCache, public MMemPool<GItemDBCache_Leaf>
{
public :
	GItemDBCache_Leaf(const MUID& uidPlayer, const IUID nIUID, const size_t nMaxModCount = DBCACHE_ITEM_MAX_MOD_COUNT) 
		: GItemDBCache(uidPlayer, nIUID, nMaxModCount) {}
	~GItemDBCache_Leaf() {}
};


#endif