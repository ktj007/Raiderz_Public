#include "StdAfx.h"
#include "GTrialField.h"
#include "CCommandTable.h"
#include "GCommandCenter.h"
#include "GEntityPlayer.h"
#include "GCommand.h"
#include "GSystem.h"
#include "GGlobal.h"
#include "GTrialFieldBoundary.h"
#include "GConst.h"

GTrialField::GTrialField(MUID& uid, MUID uidGroup)
: GDynamicField(uid, uidGroup)
, m_nExpiredTime(0)
, m_pBoundary(NULL)
{
}

GTrialField::~GTrialField()
{
	SAFE_DELETE(m_pBoundary);
}

bool GTrialField::Create( const GFieldInfo* pFieldInfo )
{
	DCHECK(pFieldInfo);
	bool bRet = 
		__super::Create(pFieldInfo);

	if (pFieldInfo->m_bBoundCheck)
	{
		m_pBoundary = new GTrialFieldBoundary(this);
	}

	return bRet;
}

void GTrialField::RouteExpiring(vector<MUID> vecPlayers)
{
	MCommand* pNewCommand = MakeNewCommand(MC_DYNAMIC_FIELD_EXPIRING,  
		1,
		NEW_UINT(m_nExpiredTime)
		);

	pNewCommand->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCommand);

	if (IsRunForTest())
	{
		mlog_filter("mizelan", "GDynamicField::RouteExpiring - %u/%u\n", m_nExpiredTime, gsys.pSystem->GetNowTime());
	}
}

void GTrialField::AddEntity(GEntity* pEntity)
{
	__super::AddEntity(pEntity);

	if (pEntity->IsPlayer())
	{
		if (IsExpiredCounting())
		{
			RouteExpiring(vector<MUID>(1, pEntity->GetUID()));
		}
	}
}

void GTrialField::RemoveEntity(GEntity* pEntity)
{
	__super::RemoveEntity(pEntity);

	if (pEntity->IsPlayer())
	{
		if (IsExpiredCounting())
		{
			RouteExpiring(vector<MUID>(1, pEntity->GetUID()));
		}
	}
}

void GTrialField::StartExpiredCount( uint32 nExpiredTime )
{
	m_nExpiredTime = nExpiredTime;


	if (IsRunForTest())
	{
		mlog_filter("mizelan", "GDynamicField::StartExpiredCount - %u/%u\n", m_nExpiredTime, gsys.pSystem->GetNowTime());
	}

	GVectorMUID vecPlayers;
	m_pEntityMgr->GetPlayerUIDs(vecPlayers.Vector());
	RouteExpiring(vecPlayers.Vector());
}

void GTrialField::CancelExpiredCount()
{
	m_nExpiredTime = 0;

	GVectorMUID vecPlayers;

	m_pEntityMgr->GetPlayerUIDs(vecPlayers.Vector());	
	RouteCancelExpire(vecPlayers.Vector());
}

void GTrialField::Update( float fDelta )
{
	__super::Update(fDelta);

	if (m_pBoundary)
	{
		m_pBoundary->Update(fDelta);
	}
}

bool GTrialField::IsExpired()
{
	if (m_pBoundary &&
		m_pBoundary->IsExpired())
		return true;

	return false;
}