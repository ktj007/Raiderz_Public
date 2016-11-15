#include "StdAfx.h"
#include "XTargetActorInfo.h"

void XTargetActorInfo::Init()
{
	nMouseSearchUIID = 0;
	nSearchUIID		= 0;
	nTargetUIID		= 0;
	bLockOnTarget	= false;
	nLockOnTime		= 0;
	nPostReqTime	= 0;

	Clear();
}

void XTargetActorInfo::Clear()
{
	map<UIID, TargetActorInfo*>::iterator itor = m_ActorMap.begin();

	for (; itor != m_ActorMap.end(); ++itor)
	{
		TargetActorInfo* pActorInfo = (*itor).second;
		SAFE_DELETE(pActorInfo);
	}

	m_ActorMap.clear();
}

void XTargetActorInfo::Add( const UIID nUIID, TD_TARGET_INFO& tdInfo )
{
	if (Find(nUIID) == true)
	{
		TargetActorInfo* pInfo = Get(nUIID);
		pInfo->Copy(tdInfo);
		return;
	}

	TargetActorInfo* pInfo = new TargetActorInfo(tdInfo);
	m_ActorMap.insert(map<UIID, TargetActorInfo*>::value_type(nUIID, pInfo));
}

void XTargetActorInfo::Add( const UIID nUIID, TD_ENEMY_INFO& tdInfo )
{
	if (Find(nUIID) == true)
	{
		TargetActorInfo* pInfo = Get(nUIID);
		pInfo->Copy(tdInfo);
		return;
	}

	TargetActorInfo* pInfo = new TargetActorInfo(tdInfo);
	m_ActorMap.insert(map<UIID, TargetActorInfo*>::value_type(nUIID, pInfo));
}

bool XTargetActorInfo::Find( const UIID nUIID )
{
	map<UIID, TargetActorInfo*>::iterator itor = m_ActorMap.find(nUIID);
	if (itor != m_ActorMap.end())	return true;

	return false;
}

bool XTargetActorInfo::Find_Time( const UIID nUIID )
{
	// 타이머 체크해서 저장된것이 3초전이면 false를 날려 요청하도록

	map<UIID, TargetActorInfo*>::iterator itor = m_ActorMap.find(nUIID);
	if (itor != m_ActorMap.end())
	{
		TargetActorInfo* pInfo = (*itor).second;
		if ( (pInfo->m_time + TARGET_INFO_REFRESH_TIME) > timeGetTime())
		{
			nPostReqTime = 0;
			return true;
		}
	}
	return false;
}

TargetActorInfo* XTargetActorInfo::Get( const UIID nUIID )
{
	map<UIID, TargetActorInfo*>::iterator itor = m_ActorMap.find(nUIID);
	if (itor != m_ActorMap.end())
	{
		return (*itor).second;
	}
	
	return NULL;
}

void XTargetActorInfo::AddEnemyList()
{
	vector<TD_ENEMY_INFO>& enemylist = gvar.Game.EnemyInfo.GetEnemyList();
	for (vector<TD_ENEMY_INFO>::iterator itor = enemylist.begin();
		itor != enemylist.end(); ++itor)
	{
		Add(itor->nUIID, (*itor));
	}
}

bool XTargetActorInfo::IsPostTime()
{
	if((nPostReqTime + TARGET_INFO_REFRESH_TIME) < timeGetTime())
	{
		nPostReqTime = timeGetTime();
		return true;
	}

	return false;
}