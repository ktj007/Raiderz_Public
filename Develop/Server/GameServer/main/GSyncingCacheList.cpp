#include "stdafx.h"
#include "GSyncingCacheList.h"
#include "GObjectSyncingCache.h"
#include "GCharacterSyncingCache.h"
#include "GItemSyncingCache.h"
#include "AStlUtil.h"
#include "GGlobal.h"
#include "GDBCacheSystem.h"


GSyncingCacheList::~GSyncingCacheList()
{
	Release();
}


void GSyncingCacheList::EndCacheSync( const bool bZero )
{
	GPlayerDBCache* pc = gsys.pDBCacheSystem->FindPlayerCache(m_uidPlayer);
	if (NULL == pc)
		return;

	for each (GObjectSyncingCache* oc in m_vCache)
	{
		oc->EndCacheSync(pc, bZero);
		SAFE_DELETE(oc);
	}

	m_vCache.clear();
}

void GSyncingCacheList::Release()
{
	SAFE_DELETE_VECTOR(m_vCache);
}

void GSyncingCacheList::SetPlayer( const MUID& uidPlayer )
{
	m_uidPlayer = uidPlayer;
}

GCharacterSyncingCache* GSyncingCacheList::PushChar()
{
	GCharacterSyncingCache* cc = new GCharacterSyncingCache();
	if (NULL == cc)
		return NULL;

	m_vCache.push_back(cc);

	return cc;
}

GItemSyncingCache* GSyncingCacheList::PushItem( const IUID nIUID )
{
	GItemSyncingCache* ic = new GItemSyncingCache(nIUID);
	if (NULL == ic)
		return NULL;

	m_vCache.push_back(ic);

	return ic;
}