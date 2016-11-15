#pragma once

namespace rs3 {

class RBackgroundWorker;
class RBackgroundJobSafeQueue;

//////////////////////////////////////////////////////////////////////////
//
// 백그라운드 작업의 단위
// 티켓을 가지고 올바른 작업인지 워커쓰레드가 판단한다.
//
class RS_API RBackgroundWorkUnit
{
	friend RBackgroundWorker;
	friend RBackgroundJobSafeQueue;
public:
	enum WorkState
	{
		WS_INVALID,				// 작업 실패
		WS_SLEEPING,			// 보류중
		WS_RUNNING,				// 도는 중
		WS_FINISHED				// 작업 완료
	};
	WorkState				GetState() { return m_nState; }

	void					UpdateCompleteWork();			///< 메인쓰레드에서 호출
	void					BackgroundWork();				///< 백그라운드 워커쓰레드에서 작업
	void					WorkInTheForeground();			///< 포그라운드에서 모든 로딩을 수행

	inline bool				IsWorkable();					///< 사용 가능한 WorkUnit인지 여부
	inline bool				IsBackgroundWork();				///< 백그라운드로 수행되는지 여부
	inline bool				IsFinished();					///< 작업이 종료되었는지 여부
	inline bool				IsSleeping();

	virtual const char*		GetName()			{ return "base"; }	///< WorkUnit 이름 반환
	virtual int				GetDebuggingID() { return 0; }	///< 디버깅용. - 테스트가 끝난 후 제거됨

	void					InvalidateWork();
	void					InvalidateWorkWithNoLock();

protected:
	RBackgroundWorkUnit();
	virtual ~RBackgroundWorkUnit();
	void					SetState(WorkState nState) { m_nState = nState; }

	RBackgroundWorker*		m_pBackgroundWorker;
	WorkState				m_nState;

			void			SetPriority(int priority)	{	m_pTicket->m_nPriority = priority;}
	virtual int				CalcPriority()			= 0;	///< 백그라운드 작업의 우선순위
	virtual void			OnBackgroundWork()		= 0;	///< 백그라운드 워커쓰레드에서 작업
	virtual void			OnUpdateCompleteWork()	= 0;	///< 메인쓰레드에서 호출

	void					FinishWork();					///< 작업이 올바르게 완료되었을때, OnUpdateCompleteWork에서만 호출해줘야 한다.
	
	inline void				SetRecursiveChainedFlag();		///< BackgroundWork, UpdateComplete를 여러번 수행하려 할 때 사용함.
private:
	class BG_TICKET
	{
	private:
		bool						m_bExpired;
		RBackgroundWorkUnit*		m_pBackgroundWorkUnit;
		BG_TICKET() : m_bExpired(false), m_nPriority(0), m_pBackgroundWorkUnit(NULL) {}
	public:
		BG_TICKET(RBackgroundWorkUnit* pWorkUnit) : m_bExpired(false), m_nPriority(0), m_pBackgroundWorkUnit(pWorkUnit) {}
		~BG_TICKET(){}
		void InvalidateWork() 
		{ 
			m_bExpired = true;
			m_pBackgroundWorkUnit = NULL; 
		}

		int							m_nPriority;
		RBackgroundWorkUnit*		GetWorkUnit() { return m_pBackgroundWorkUnit; }
		bool IsExpired() { return m_bExpired; }
	};
	BG_TICKET*				m_pTicket;
	bool					m_bRecursiveChained;
	bool					m_bBackgroundWorkFinished;
};
//----------------------------------------------------------------------------------------------------
inline bool RBackgroundWorkUnit::IsWorkable()
{
	return m_pTicket != NULL;
}
inline bool RBackgroundWorkUnit::IsBackgroundWork() 
{ 
	return m_pBackgroundWorker != NULL; 
}

inline void RBackgroundWorkUnit::SetRecursiveChainedFlag()
{
	m_bRecursiveChained = true;
}

inline bool RBackgroundWorkUnit::IsFinished()
{
	return (m_nState == WS_FINISHED);
}


//----------------------------------------------------------------------------------------------------
}
