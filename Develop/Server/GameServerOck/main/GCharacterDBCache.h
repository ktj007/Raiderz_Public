#ifndef _GDBCACHE_CHARACTER_H
#define _GDBCACHE_CHARACTER_H


#include "GObjectDBCache.h"
#include "cache_value_type.h"
#include "MMemPool.h"


const static size_t DBCACHE_CHAR_MAX_MOD_COUNT	= 5;
const static int	DBCACHE_MAX_MOD_MONEY		= DBCACHE_SKEEP;
const static int	DBCACHE_MAX_MOD_XP			= DBCACHE_SKEEP;

class GCharacterDBCache : public GObjectDBCache
{
public :
	GCharacterDBCache(const MUID& uidPlayer, const CID nCID, const size_t nMaxModCount = DBCACHE_CHAR_MAX_MOD_COUNT);
	virtual ~GCharacterDBCache() {}

	CID				GetCID()	{ return m_nCID; }

	cache_Money&	GetMoney();
	cache_XP&		GetXP();
	
	bool			IncMoney(const int nIncMoney, const bool bAutoSync = true);
	bool			DecMoney(const int nDecMoney, const bool bAutoSync = true);

	bool			IncXP(const int nIncXP, const bool bAutoSync = true);
	bool			DecXP(const int nDecXP, const bool bAutoSync = true);

	bool			IsModified() override;
	void			SetZero() override;

	void			SetMaxModCount(const size_t nMaxModCount);

	int				GetModMoney();

protected :
	virtual bool	IsOverflowModifiedRange() override;
	virtual bool	IsOverflowModifiedCnt() override;

	size_t			CalcModifiedCnt();
	void			StartCaching();

protected :	
	const CID		m_nCID;
	cache_Money		m_cacheMoney;
	cache_XP		m_cacheXP;	
	size_t			m_nMaxModCount;
};

class GCharacterDBCache_Leaf : public GCharacterDBCache, public MMemPool<GCharacterDBCache_Leaf>
{
public :
	GCharacterDBCache_Leaf(const MUID& uidPlayer, const CID nCID, const size_t nMaxModCount = DBCACHE_CHAR_MAX_MOD_COUNT)
		: GCharacterDBCache(uidPlayer, nCID, nMaxModCount) {}
	~GCharacterDBCache_Leaf() {}
};


#endif