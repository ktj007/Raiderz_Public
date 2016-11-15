#pragma once

#include "MMemPool.h"
#include "GAIEventInvoker.h"
#include <queue>

class GSectorAIEventInvoker: public GAIEventInvoker, public MMemPool<GSectorAIEventInvoker>
{
public:
	GSectorAIEventInvoker(MUID uidField);
	virtual ~GSectorAIEventInvoker();

	void EnterPlayer(MPoint ptCell);
	void LeavePlayer(MPoint ptCell, int nPlayerQty);
	void MoveNPC(MUID uidNPC, bool bPlayerEmptyInNeighborCells);
	// 섹터이동처럼 Leave-Enter 쌍으로 작동하는 이벤트 사이에, 양쪽 섹터에서 플레이어가 하나도 없는 상황이 만들어진다.
	// 전투 중 위 상황에서 이벤트를 검사할 경우 LOD가 튀는 현상을 막기위한 함수.
	void LazedNotifyBegin(void);	// 즉시 통보하지 않고 쌓아둔다.
	void LazedNotifyEnd(void);		// 쌓인 이벤트를 일괄 통보한다.

	void Invoke(GAIEvent aiEvent);

	void InvokeEnterPlayer(MUID uidField, int nSectorX, int nSectorY);
	void InvokeLeavePlayer(MUID uidField, int nSectorX, int nSectorY);
	void InvokeMoveNPCBePlayer(MUID uidNPC);
	void InvokeMoveNPCEmptyPlayer(MUID uidNPC);

private:
	MUID m_uidField;
	bool m_isPlayerEmpty;

	bool m_isLazedNotify;
	queue<GAIEvent> m_queueEvent;
};
