#pragma once

#include "GFieldGrid.h"

class GSectorAIEventInvoker;

//////////////////////////////////////////////////////////////////////////
//
//	GFieldCellAIEventInvokerMgr
//
//////////////////////////////////////////////////////////////////////////

class GSectorAIEventInvokerMgr : public MTestMemPool<GSectorAIEventInvokerMgr>
{
public:
	GSectorAIEventInvokerMgr(void);
	~GSectorAIEventInvokerMgr(void);

	void Init(MUID uidField, int nSizeX, int nSizeY);
	void Clear();

	// 엔티티 추가
	void AddEntity( const MPoint& ptCell, ENTITY_TYPEID nEntityTypeID, const MUID& uid, GFieldGrid::EntitySelector* selectorEntity );
	// 엔티티 삭제
	void RemoveEntity( const MPoint& ptCell, ENTITY_TYPEID nEntityTypeID, GFieldGrid::Sector* pCell);
	// 이벤트 가드 시작
	void EventGuardBegin(const MPoint& ptCell);
	// 이벤트 가드 종료
	void EventGuardEnd(const MPoint& ptCell);
private:
	vector <vector <GSectorAIEventInvoker*> >	m_vecFieldCellAIEventInvokers;
};

//////////////////////////////////////////////////////////////////////////
//
//	AIEventInvokerGuarder
//
//////////////////////////////////////////////////////////////////////////

class AIEventInvokerGuarder
{
public:
	AIEventInvokerGuarder(GSectorAIEventInvokerMgr* pOwner, MPoint ptCell);
	~AIEventInvokerGuarder();

private:
	GSectorAIEventInvokerMgr*	m_pOwner;
	MPoint	m_ptCell;
};

#define AIEVENT_INVOKER_GUARD(owner, cell)  AIEventInvokerGuarder(owner, cell)