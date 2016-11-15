#include "StdAfx.h"
#include "GSectorAIEventInvokerMgr.h"
#include "GSectorAIEventInvoker.h"

//////////////////////////////////////////////////////////////////////////
//
//	GFieldCellAIEventInvokerMgr
//
//////////////////////////////////////////////////////////////////////////

GSectorAIEventInvokerMgr::GSectorAIEventInvokerMgr(void)
{
}

GSectorAIEventInvokerMgr::~GSectorAIEventInvokerMgr(void)
{
}

void GSectorAIEventInvokerMgr::Init( MUID uidField, int nSizeX, int nSizeY )
{
	for(int x = 0; x < nSizeX; x++)	
	{		
		vector<GSectorAIEventInvoker*> vecInvokers;

		for(int y = 0; y < nSizeY; y++)
		{
			vecInvokers.push_back(new GSectorAIEventInvoker(uidField));
		}

		m_vecFieldCellAIEventInvokers.push_back(vecInvokers);
	}
}

void GSectorAIEventInvokerMgr::Clear()
{
	for each(const vector<GSectorAIEventInvoker*>& vecInvokers in m_vecFieldCellAIEventInvokers)
	{
		for each(GSectorAIEventInvoker* pInvoker in vecInvokers)
		{
			delete pInvoker;
		}
	}

	m_vecFieldCellAIEventInvokers.clear();
}

void GSectorAIEventInvokerMgr::AddEntity( const MPoint& ptCell, ENTITY_TYPEID nEntityTypeID, const MUID& uid, GFieldGrid::EntitySelector* selectorEntity )
{
	// AI LOD
	if (nEntityTypeID == ETID_PLAYER)
	{
		m_vecFieldCellAIEventInvokers[ptCell.x][ptCell.y]->EnterPlayer(ptCell);
	}
	else if (nEntityTypeID == ETID_NPC)
	{
		m_vecFieldCellAIEventInvokers[ptCell.x][ptCell.y]->MoveNPC(uid, selectorEntity->IsEmptyPlayers(ptCell));
	}
}

void GSectorAIEventInvokerMgr::RemoveEntity( const MPoint& ptCell, ENTITY_TYPEID nEntityTypeID, GFieldGrid::Sector* pCell)
{
	if (nEntityTypeID == ETID_PLAYER)
	{
		m_vecFieldCellAIEventInvokers[ptCell.x][ptCell.y]->LeavePlayer(ptCell, pCell->GetEntityQty(ETID_PLAYER));
	}
}

void GSectorAIEventInvokerMgr::EventGuardBegin( const MPoint& ptCell )
{
	m_vecFieldCellAIEventInvokers[ptCell.x][ptCell.y]->LazedNotifyBegin();
}

void GSectorAIEventInvokerMgr::EventGuardEnd( const MPoint& ptCell )
{
	m_vecFieldCellAIEventInvokers[ptCell.x][ptCell.y]->LazedNotifyEnd();
}

//////////////////////////////////////////////////////////////////////////
//
//	AIEventInvokerGuarder
//
//////////////////////////////////////////////////////////////////////////

AIEventInvokerGuarder::AIEventInvokerGuarder( GSectorAIEventInvokerMgr* pOwner, MPoint ptCell )
: m_pOwner(pOwner)
, m_ptCell(ptCell)
{
	m_pOwner->EventGuardBegin(m_ptCell);
}

AIEventInvokerGuarder::~AIEventInvokerGuarder()
{
	m_pOwner->EventGuardEnd(m_ptCell);
}