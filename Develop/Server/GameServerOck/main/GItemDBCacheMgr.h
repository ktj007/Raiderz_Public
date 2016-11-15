#ifndef _GDBCACHE_ITEM_MGR_H
#define _GDBCACHE_ITEM_MGR_H


#include "STypes.h"


#include <map>


class GItemDBCache;


typedef std::map<IUID, GItemDBCache*> GITEM_DBCACHE_MAP;


class GItemDBCacheMgr : protected std::map<IUID, GItemDBCache*>
{
public :
	bool			Insert(GItemDBCache* pItem);
	GItemDBCache*	Find(const IUID nIUID);
	void			Erase(const IUID nIUID);

	using std::map<IUID, GItemDBCache*>::begin;
	using std::map<IUID, GItemDBCache*>::end;
	using std::map<IUID, GItemDBCache*>::clear;
	using std::map<IUID, GItemDBCache*>::size;
	using std::map<IUID, GItemDBCache*>::empty;

	
	// set<IUID>;
};


#endif