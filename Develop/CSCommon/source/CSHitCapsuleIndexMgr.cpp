#include "StdAfx.h"
#include "CSHitCapsuleIndexMgr.h"

CSHitCapsuleIndexMgr::CSHitCapsuleIndexMgr()
: m_nTalent(NONE_HITCAPSULEGROUP_ID)
, m_nCurNPCMode(0)
, m_bUsingTalent(false)
{

}

int8 CSHitCapsuleIndexMgr::GetGroupIndex()
{
	// [ 1순위] --- 탤런트에 의한 경우
	if (m_bUsingTalent)
		return m_nTalent;

	// [ 2순위] --- 모션에 의한 경우
	return GetModeGroupID();
}

void CSHitCapsuleIndexMgr::InsertNPCModeGroupIndex(int nMode, int8 nGroupID)
{
	m_mapMode.insert(MAP_MODE::value_type(nMode, nGroupID));
}

void CSHitCapsuleIndexMgr::Change_ByTalent( int8 nGroupID )
{
	if (nGroupID == NONE_HITCAPSULEGROUP_ID)
		return;

	m_bUsingTalent = true;
	m_nTalent = nGroupID;
}

void CSHitCapsuleIndexMgr::Change_ByTalentComplete()
{
	m_bUsingTalent = false;
	m_nTalent = NONE_HITCAPSULEGROUP_ID;
}

void CSHitCapsuleIndexMgr::ChangeNPCMode( int nMode )
{
	m_nCurNPCMode = nMode;
}

void CSHitCapsuleIndexMgr::Change_ByTrigger( int8 nGroupID )
{
	MAP_MODE::iterator iter = m_mapMode.find(m_nCurNPCMode);
	if (iter == m_mapMode.end())		return;
	m_mapMode[m_nCurNPCMode] = nGroupID;
}

int8 CSHitCapsuleIndexMgr::GetModeGroupID()
{
	MAP_MODE::iterator iter = m_mapMode.find(m_nCurNPCMode);
	if (iter == m_mapMode.end())		return INVALID_HITCAPSULEGROUP_ID;
	return iter->second;
}

void CSHitCapsuleIndexMgr::Clear()
{
	m_bUsingTalent = false;
	m_nTalent = NONE_HITCAPSULEGROUP_ID;
	m_nCurNPCMode = 0;
	m_mapMode.clear();
}

