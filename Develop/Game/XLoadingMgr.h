#pragma once

#include "XLoadingChecker.h"

class XLoadingStarter;
class XLoadingCompleter;

/// 로딩관련 이벤트 ID
enum XLoadingEventID
{
	XLOADING_EVT_ON_FIELD_START_GAME = 1		//  서버로부터 MC_FIELD_START_GAME 커맨드 받음
};

/// 로딩 관리자
class XLoadingMgr
{
public:
	enum LoadingState
	{
		STATE_READY = 0,
		STATE_STARTED,
		STATE_WORLD_LOADED,
		STATE_COMPLETED
	};
protected:
	LoadingState					m_nState;
	XLoadingStarter*				m_pStarter;
	XLoadingCompleter*				m_pCompleter;
	vector<XLoadingChecker*>		m_vecCheckers;

	void WorldLoadingFinish();
	void Complete();					///< 로딩이 다 완료되면 호출
public:
	XLoadingMgr(XLoadingStarter* pStarter = NULL, XLoadingCompleter* pCompleter = NULL);
	virtual ~XLoadingMgr();

	void AddChecker(XLoadingChecker* pChecker);			///< 로딩이 다되었는지를 체크하는 클래스 추가
	void ClearCheckers();
	void Start();										///< 시작할 때 호출
	bool CheckCompleted();								///< 로딩이 완료되었는지 여부 확인
	void Event(XEvent& evt);

	bool IsLoading();									///< 첫번째 로딩인지 여부
	LoadingState GetState() { return m_nState; }
};