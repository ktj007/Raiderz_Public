#include "stdafx.h"
#include "GCollisionTree.h"
#include "GCollisionTreeMgr.h"

GCollisionTreeMgr::~GCollisionTreeMgr( void )
{
	Clear();
}

void GCollisionTreeMgr::Insert( int nZoneID, GCollisionTree* pCollisionTree )
{
	insert(value_type(nZoneID, pCollisionTree));
}

GCollisionTree* GCollisionTreeMgr::Find( int nZoneID )
{
	iterator itor = find(nZoneID);
	if (itor == end()) return NULL;

	return (*itor).second;
}

void GCollisionTreeMgr::Clear()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		GCollisionTree* pCollisionTree = (*itor).second;
		delete pCollisionTree;
	}
	clear();
}
