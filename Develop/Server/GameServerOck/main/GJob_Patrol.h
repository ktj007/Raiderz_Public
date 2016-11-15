#ifndef _GJOB_PATROL_H
#define _GJOB_PATROL_H

#include "MMemPool.h"
#include "GJob.h"
#include "CSTriggerInfo.h"

class GJob_Patrol : public GJob, public MMemPool<GJob_Patrol>
{
private:
	vector<MARKER_INFO> 	m_MarkerList;
	size_t			m_nCurrMarkerIndex;
	bool			m_bReverseOrder;
	bool 			m_bComplete;
	bool			m_bLastDoingIdle;
	PATROL_TYPE		m_PatrolType;
	bool			m_bUseIdleColt;
private:
	// 마커 인덱스가 마지막까지 왔는지 여부
	bool IsEndedMarker();

	// 마커 리스트 추가
	void InsertFromMarkerIDList(MARKER_LIST& listMarkers);
	
	// 이동할 마커 인덱스 증가, 반환값은 이동을 계속 진행할지 여부
	bool IncreaseMarkerIndex();
protected:
	virtual bool OnStart();
	virtual GJobResult OnRun(float fDelta);
	virtual void OnTaskComplete(GTaskID nTaskID);
	virtual void OnCancel();
			bool OnIdle();
public:
	DECLARE_JOB_ID(GJOB_PATROL);
	virtual string GetParamString() const override;

	GJob_Patrol(GModuleAI* pmAI, MARKER_LIST& MarkerList, PATROL_TYPE PatrolType);
	GJob_Patrol(GModuleAI* pmAI, const vector<MARKER_INFO>& MarkerList, PATROL_TYPE PatrolType);
	virtual ~GJob_Patrol();

	bool GetCurrentPatrolMarker(MARKER_INFO& Marker);
	bool AdvanceMoveMarker();
	void RunAdvance();
	void CallScript();
};


#endif // _GJOB_MOVE_TO_POS_H