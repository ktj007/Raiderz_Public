#pragma once

#include "MThread2.h"
#include "RBackgroundWorkUnit.h"
#include "RTimer.h"
#include <map>
#include <list>

namespace rs3 {

class RSceneManager;
class RBackgroundWorkUnit;
class RBackgroundWorkNotifier;
class REngine;

// Background Work는 Priority를 가진다. 숫자가 클수록 낮은 Priority
enum RBACKGROUNDWORKER_PRIORITY
{
	RBGWK_HIGHEST		= -50000,
	RBGWK_NORMAL		= 0,

	RBGWK_HIGH_OFFSET	= 10000,	//빼주면 high 에 가까워진다.
	

	RBGWK_SLEEP			= 50000
};

#define DEFAULT_DISTANCE_FOR_BACKGROUND_PRIORITY	30000.0f

class RBackgroundJobSafeQueue
{
public:
	typedef std::multimap< int, RBackgroundWorkUnit::BG_TICKET* > JOB_QUEUE;
private:
	JOB_QUEUE				m_Queue;
	cml2::MCriticalSection	m_csLock;
public:
	RBackgroundJobSafeQueue() {}
	~RBackgroundJobSafeQueue() {}
	void Insert(RBackgroundWorkUnit::BG_TICKET* pTicket);
	void MoveTo(RBackgroundJobSafeQueue& tarQueue);
	void ReOrderAndMoveNotSleepTicketTo(RBackgroundJobSafeQueue& tarQueue);
	void ReOrder();
	void Clear();
	RBackgroundWorkUnit::BG_TICKET* PopTicket();

	size_t GetSize();
	bool IsEmpty();
};


struct RBackgroundWorkDebuggingInfo
{
	unsigned int nBackgroundWorkUnitTimes[50];
	unsigned int nCompleteWorkUnitTimes[50];
	unsigned int nBackgroundWorkUnitCount[50];
	unsigned int nCompleteWorkUnitCount[50];

	RBackgroundWorkDebuggingInfo()
	{
		for (int i = 0; i < 50; i++)
		{
			nBackgroundWorkUnitTimes[i] = 0;
			nCompleteWorkUnitTimes[i] = 0;
			nBackgroundWorkUnitCount[i] = 0;
			nCompleteWorkUnitCount[i] = 0;
		}
	}
};

class RS_API RBackgroundWorker
{
	friend RSceneManager;
	friend RBackgroundWorkUnit;
	friend REngine;

private:
	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 작업관련 큐
	//
	RBackgroundJobSafeQueue	m_readyQueue;				// main thread에서만 사용함
	RBackgroundJobSafeQueue	m_sleepQueue;				// main thread에서만 사용함
	RBackgroundJobSafeQueue	m_runningQueue;
	RBackgroundJobSafeQueue	m_completeQueue;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Thread 제어를 위한 이벤트
	//
	enum SEND_EVENT
	{
		SEND_ACTIVATE_THREAD		= 0,
		SEND_HALT_THREAD,
		SEND_EVENT_NUM
	};
	enum
	{
		RECEIVE_TRHEAD_ACTIVATE		= WAIT_OBJECT_0,
		RECEIVE_TRHEAD_HALT			= WAIT_OBJECT_0 + 1
	};

	bool m_bInited;

	HANDLE					m_hThreadControlEvents[SEND_EVENT_NUM];
	HANDLE					m_hThreadWillBeDieEvent;


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 작업자 쓰레드(Worker Thread)관련 핸들 및 Critical Section
	//
	cml2::MCriticalSection	m_csResourceLock;
	cml2::MCriticalSection	m_csTicket;

	HANDLE					m_hWorkerThread;
	bool					m_bSleptWorkerThread;
	bool					m_bRequestedStopWorking;

	// Worker Thread Proc
	static unsigned int WINAPI	WorkerThreadProc(void* _pWorker);


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Complete Queue 업데이트 관련
	//
	DWORD					m_dwAcceptableUpdateTime;	// Acceptable Elapsed Time
	bool					m_bSmoothUpdate;
	void					UpdateCompleteQueue();		///< 메인 쓰레드에서 호출하는 Update()함수에서 부름

	// notifier
//	std::list<RResource*>	m_notificationQueue;		// 작업관련 통지큐

	// for debugging
	RBackgroundWorkDebuggingInfo m_DebuggingInfo;

	void LogStatus();
public:
	RBackgroundWorker();
	~RBackgroundWorker(){}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	void					Init();			// 초기화
	void					Destroy();		// 클리어및 소멸

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 메인쓰레드와 작업쓰레드와의 동기화를 위한 Lock / UnLock
	//


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 메인쓰레드에서만 불리는 queue 관련 함수
	// 1. AddToReadyQueue			: ReadyQueue에 작업추가
	// 2. WakeUpWorker				: 작업쓰레드 깨우기
	// 3. FlushReadyQeueueAndRun	: ReadyQueue에 있는 작업은 RunningQueue에 옮기고 작업쓰레드 깨우기
	// 4. ReorderQueues				: 작업관련 큐의 우선순위를 재조정한다.
	// 5. ClearQueues				: 작업관려 큐를 비운다.
	void AddToReadyQueue( RBackgroundWorkUnit* pWorkable );
	void WakeUpWorker();
	void FlushReadyQueueAndRun();
	void ReOrderReadyQueue();
	void ReOrderRunningQueue();
	void ClearQueues();

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 메인쓰레드에서만 Update를 호출할 수 있음
	// 1. m_pBackgroundWorkUnit 체크 NULL이면 지우기
	//      - 메인쓰레드가 어떤곳에서 InvalidateWorkable 할수 있음
	// 2. OnUpdateCompleteWork 불러주기
	// 3. m_pBackgroundWorkUnit 체크 NULL이면 지우기
	//      - OnUpdateCompleteWork시에 InvalidateWorkable 하는경우가 대부분.
	//      - OnUpdateCompleteWork시에 새로운 AddJob을 요청할 수 있고 이때는 지워주지 않는다.
	void Update();

	// Acceptable Time. 만일 0이면 오직 하나의 작업만 수행한다.
	void	SetUpdateAcceptableTime( DWORD dwAcceptableTime, bool bSmooth )	{ m_dwAcceptableUpdateTime = dwAcceptableTime; m_bSmoothUpdate = bSmooth; }
	DWORD	GetUpdateAcceptableTime()										{ return m_dwAcceptableUpdateTime; }

	// queue size
	int GetRemainedWorkableTicketSize();


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 강제로 백그라운드 작업 멈추기. 이 메쏘드를 자주 쓰는것은 올바른 사용법이 아닙니다.
	// cf. 백그라운드 작업을 클리어 하는데 올바른 방법은 강제로 멈추기가 아니라
	// ClearAllQueues를 통해 스스로 작업을 멈추게 하는것 입니다.
	//
	void ForceBackgroundWorkStop();

	bool IsInited() { return m_bInited; }

	void LockTicket()	{ m_csTicket.Lock();	}
	void UnLockTicket()	{ m_csTicket.Unlock();	}
	void LockResource() { m_csResourceLock.Lock(); }
	void UnLockResource() { m_csResourceLock.Unlock(); }

// 	void RegisterNotify(RResource* pResource);
// 	void CancelNotify(RResource* pResource);
// 	void PostNotifyQueue();
};


}


// 해야할 것
// 큐 정리하기 - RunningQueue, COmpleteQueue -> SafeQueue 클래스
// BG_TICKET 정리하기
// 테스트 테스트

