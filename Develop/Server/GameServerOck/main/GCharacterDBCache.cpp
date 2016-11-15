#include "stdafx.h"
#include "GCharacterDBCache.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GPlayerDBCache.h"

GCharacterDBCache::GCharacterDBCache( const MUID& uidPlayer, const CID nCID, const size_t nMaxModCount ) 
: GObjectDBCache(uidPlayer), m_nCID(nCID), m_cacheMoney(DBCACHE_MAX_MOD_MONEY), m_cacheXP(DBCACHE_MAX_MOD_XP), m_nMaxModCount(nMaxModCount)
{

}

bool GCharacterDBCache::IsOverflowModifiedRange()
{
	if ((DBCACHE_SKEEP == m_cacheMoney.GetMaxModifyRange()) && (DBCACHE_SKEEP == m_cacheXP.GetMaxModifyRange()))
		return false;

	if (m_cacheMoney.IsOverflowMaxModifyRange() || m_cacheXP.IsOverflowMaxModifyRange())
		return true;

	return false;
}

bool GCharacterDBCache::IsOverflowModifiedCnt()
{
	if (DBCACHE_SKEEP == m_nMaxModCount)
		return false;

	if (m_nMaxModCount < CalcModifiedCnt())
		return true;

	return false;
}

bool GCharacterDBCache::IncMoney( const int nIncMoney, const bool bAutoSync )
{
	if (bAutoSync && !BeginCacheSync())
		return false;

	StartCaching();

	m_cacheMoney.Increase(nIncMoney);	
	
	return true;
}

bool GCharacterDBCache::DecMoney( const int nDecMoney, const bool bAutoSync )
{
	if (bAutoSync && !BeginCacheSync())
		return false;

	StartCaching();

	m_cacheMoney.Decrease(nDecMoney);

	return true;
}

void GCharacterDBCache::SetZero()
{
	m_cacheXP.SetZero();
	m_cacheMoney.SetZero();	
}

bool GCharacterDBCache::IsModified()
{
	if (0 < CalcModifiedCnt())
		return true;

	return false;
}

size_t GCharacterDBCache::CalcModifiedCnt()
{
	return m_cacheMoney.GetModifiedCnt() + m_cacheXP.GetModifiedCnt();
}

bool GCharacterDBCache::IncXP( const int nIncXP, const bool bAutoSync )
{
	if (bAutoSync && !BeginCacheSync())
		return false;

	StartCaching();

	m_cacheXP.Increase(nIncXP);

	return true;
}

bool GCharacterDBCache::DecXP( const int nDecXP, const bool bAutoSync )
{
	if (bAutoSync && !BeginCacheSync())
		return false;

	StartCaching();

	m_cacheXP.Decrease(nDecXP);

	return true;
}

cache_Money& GCharacterDBCache::GetMoney()
{
	return m_cacheMoney;
}

cache_XP& GCharacterDBCache::GetXP()
{
	return m_cacheXP;
}

void GCharacterDBCache::SetMaxModCount( const size_t nMaxModCount )
{
	m_nMaxModCount = nMaxModCount;
}

void GCharacterDBCache::StartCaching()
{
	if (0 < CalcModifiedCnt())
		return;

	SetSyncTm(gsys.pSystem->GetNowTime());
}

int GCharacterDBCache::GetModMoney()
{
	return m_cacheMoney.GetIncrease() - m_cacheMoney.GetDecrease();
}
