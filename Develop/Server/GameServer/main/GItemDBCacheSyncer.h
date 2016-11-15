#pragma once

#include "GSyncingCacheList.h"

class GDBSYNC_ITEM
{
public :
	GDBSYNC_ITEM() : m_nSlotType(0), m_nSlotID(0), m_nIUID(0) {}
	GDBSYNC_ITEM(const uint8 nSlotType, const int16 nSlotID, const IUID nIUID) 
		: m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nIUID(nIUID) {}

	uint8	m_nSlotType;
	int16	m_nSlotID;
	IUID	m_nIUID;
};

typedef vector<GDBSYNC_ITEM> GDBSYNC_ITEM_VEC;

class GItemDBCacheSyncer
{
public:
	GItemDBCacheSyncer(const MUID& uidPlayer);
	~GItemDBCacheSyncer(void);

	bool BeginCacheSync(const uint8 nSlotType, const int16 nSlotID, const IUID nIUID);	
	bool BeginCacheSync(const GDBSYNC_ITEM_VEC& vSyncItem);	
	void EndCacheSync(const bool bZero );

private:
	bool SyncItem(const uint8 nSlotType, const int16 nSlotID, const IUID nIUID);

private:
	GSyncingCacheList	m_SyncingList;
};
