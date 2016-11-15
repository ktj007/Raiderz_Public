#include "stdafx.h"
#include "GItemDBCacheMgr.h"
#include "GItemDBCache.h"


bool GItemDBCacheMgr::Insert( GItemDBCache* pItem )
{
	if (NULL == pItem)
		return false;

	pair<iterator, bool> res = insert(value_type(pItem->GetIUID(), pItem));
	return res.second;		
}

GItemDBCache* GItemDBCacheMgr::Find( const IUID nIUID )
{
	iterator it = find(nIUID);
	if (end() == it)
		return NULL;

	return it->second;
}

void GItemDBCacheMgr::Erase( const IUID nIUID )
{
	erase(nIUID);
}