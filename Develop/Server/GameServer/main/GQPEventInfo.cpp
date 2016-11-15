#include "stdafx.h"
#include "GQPEventInfo.h"
#include "CSDef_QuestPVP.h"
#include "AStlUtil.h"

GQPESpawnInfo::GQPESpawnInfo( void )
: m_nSpawnID(0)
, m_fDelayTime(0.0f)
, m_fDespawnTime(0.0f)
{

}

GQPEItemInfo::GQPEItemInfo( void )
: m_nItemID(0)
, m_nAmount(0)
{

}

GQPEBeginInfo::~GQPEBeginInfo()
{
	SAFE_DELETE_VECTOR(m_vecSpawnInfo);
}

GQPEEndInfo::GQPEEndInfo()
: m_nWinnerTeam(QT_NONE)
, m_nMoney(0)
, m_nXP(0)
{

}

GQPEEndInfo::~GQPEEndInfo()
{
	SAFE_DELETE_VECTOR(m_vecSpawnInfo);
	SAFE_DELETE_VECTOR(m_vecItemInfo);
}

GQPEventInfo::GQPEventInfo( void )
: m_nID(0)
{

}

GQPEventInfo::~GQPEventInfo( void )
{
	SAFE_DELETE_VECTOR(m_vecEndInfo);
}

GQPEEndInfo* GQPEventInfo::GetEndInfo( QUESTPVP_TEAM nWinnerTeam )
{
	for each (GQPEEndInfo* pEndInfo in m_vecEndInfo)
	{
		if (nWinnerTeam == pEndInfo->m_nWinnerTeam)
		{
			return pEndInfo;
		}
	}

	return NULL;
}
