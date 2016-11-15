#include "stdafx.h"
#include "GItemDBCache.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GPlayerDBCache.h"
#include "GPlayerDBCacheMgr.h"


GItemDBCache::GItemDBCache( const MUID& uidPlayer, const IUID nIUID, const size_t nMaxModCount, const size_t nMaxModStackAmt, const size_t nMaxModDura )
: GObjectDBCache(uidPlayer), m_nIUID(nIUID), m_cacheStackAmt(nMaxModStackAmt), m_cacheDura(nMaxModDura)
, m_nMaxModCount(nMaxModCount), m_nSlotType(0), m_nSlotID(0)
{

}

void GItemDBCache::SetZero()
{
	m_cacheStackAmt.SetZero();	
	m_cacheDura.SetZero();
}

bool GItemDBCache::IsModified()
{
	if (0 < CalcModifiedCnt())
		return true;

	return false;
}

bool GItemDBCache::IsOverflowModifiedRange()
{
	if ((DBCACHE_SKEEP == m_cacheStackAmt.GetMaxModifyRange()) && (DBCACHE_SKEEP == m_cacheDura.GetMaxModifyRange()))
		return false;

	if (m_cacheStackAmt.IsOverflowMaxModifyRange() || m_cacheDura.IsOverflowMaxModifyRange())
		return true;

	return false;
}

bool GItemDBCache::IsOverflowModifiedCnt()
{
	if (DBCACHE_SKEEP == m_nMaxModCount)
		return false;

	if (m_nMaxModCount < CalcModifiedCnt())
		return true;

	return false;
}

IUID GItemDBCache::GetIUID()
{
	return m_nIUID;
}

bool GItemDBCache::CheckValidModifyCnt( const size_t nStackAmtModCnt, const size_t nDuraModCnt )
{
	// 아이템은 두 속성을 같이 사용할 수 없다.
	// 
	// 소모성 아이템이 내구도를 가질 수 없다.
	// 내구도를 가지고 있는 아이템이 소모성일 수 없다.
	// 
	if ((0 < nStackAmtModCnt) && (0 < nDuraModCnt))
		return false;

	return true;
}

bool GItemDBCache::CheckVaildModifyStackAmt(const bool bAutoSync)
{
	
	if (!CheckValidModifyCnt(m_cacheStackAmt.GetModifiedCnt() + 1, m_cacheDura.GetModifiedCnt()))
		return false;
	
	if (bAutoSync && !BeginCacheSync())
		return false;

	return true;
}

bool GItemDBCache::CheckVaildModifyDura( const bool bAutoSync )
{
	// 아이템은 두 속성을 같이 사용할 수 없다.
	if (!CheckValidModifyCnt(m_cacheStackAmt.GetModifiedCnt(), m_cacheDura.GetModifiedCnt() + 1))
		return false;

	if (bAutoSync && !BeginCacheSync())
		return false;

	return true;
}

bool GItemDBCache::IncStackAmt( const int16 nIncAmt, const bool bAutoSync )
{
	if (!CheckVaildModifyStackAmt(bAutoSync))
		return false;

	StartCaching();

	m_cacheStackAmt.Increase(nIncAmt);
	
	return true;
}

bool GItemDBCache::DecStackAmt( const int16 nDecAmt, const bool bAutoSync )
{
	if (!CheckVaildModifyStackAmt(bAutoSync))
		return false;

	StartCaching();

	m_cacheStackAmt.Decrease(nDecAmt);

	return true;
}

bool GItemDBCache::IncDura( const uint8 nIncDura, const bool bAutoSync )
{
	if (!CheckVaildModifyDura(bAutoSync))
		return false;

	StartCaching();

	m_cacheDura.Increase(nIncDura);

	return true;
}

bool GItemDBCache::DecDura( const uint8 nDecDura, const bool bAutoSync )
{
	if (!CheckVaildModifyDura(bAutoSync))
		return false;

	StartCaching();

	m_cacheDura.Decrease(nDecDura);

	return true;
}

size_t GItemDBCache::CalcModifiedCnt()
{
	return m_cacheStackAmt.GetModifiedCnt() + m_cacheDura.GetModifiedCnt();
}

void GItemDBCache::StartCaching()
{
	if (0 < CalcModifiedCnt())
		return;

	SetSyncTm(GetLocalTm());
}

cache_StackAmt& GItemDBCache::GetStackAmt()
{
	return m_cacheStackAmt;
}

cache_Dura& GItemDBCache::GetDura()
{
	return m_cacheDura;
}

void GItemDBCache::SetMaxModifyCnt( const size_t nMaxModCount )
{
	m_nMaxModCount = nMaxModCount;
}

void GItemDBCache::SetSlot( const uint8 nSlotType, const int16 nSlotID )
{
	m_nSlotType = nSlotType;
	m_nSlotID = nSlotID;
}

uint8 GItemDBCache::GetSlotType()
{
	return m_nSlotType;
}

int16 GItemDBCache::GetSlotID()
{
	return m_nSlotID;
}

int16 GItemDBCache::GetModAmt()
{
	return GetStackAmt().GetIncrease() - GetStackAmt().GetDecrease();
}
