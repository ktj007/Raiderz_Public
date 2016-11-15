#include "stdafx.h"
#include "GPlayerDBCacheMgr.h"
#include "GPlayerDBCache.h"

bool GPlayerDBCacheMgr::Insert( GPlayerDBCache* pPlayer )
{
	pair<iterator, bool> res = insert(value_type(pPlayer->GetUID().Value, pPlayer));
	return res.second;
}

GPlayerDBCache* GPlayerDBCacheMgr::Find( const MUID& uidPlayer )
{
	iterator it = find(uidPlayer.Value);
	if (end() == it)
		return NULL;

	return it->second;
}

void GPlayerDBCacheMgr::Erase( const MUID& uidPlayer )
{
	erase(uidPlayer.Value);
}