#ifndef _GSTATE_H
#define _GSTATE_H

#include <map>
using namespace std;
#include "GMessage.h"
#include "MDebug.h"
#include "GFrameworkLib.h"
#include "GConst.h"
#include "MLocale.h"

template <class _T>
class GStateMachine;

template <class _T>
class GState;

/// FSM의 부모 State 
template <class _T>
class GState
{
	friend class GStateMachine<_T>;
protected:
	_T*							m_pOwner;
	GStateMachine<_T>*			m_pFSM;
	int							m_nMessageIDToChagedState;

protected:
	void					AddTransitionRule(int nMsgType, int nStateID)			{ _ASSERT(m_pFSM); m_pFSM->AddTransitionRule(GetID(), nMsgType, nStateID); }
	void					SetMessageIDToChangedState(int nMessageIDToChagedState)	{ m_nMessageIDToChagedState = nMessageIDToChagedState;}
	virtual void			OnRegisterTrainstionRule()=0;

public:
	GState(_T* pOwner, GStateMachine<_T>* pFSM) : m_pOwner(pOwner), m_pFSM(pFSM), m_nMessageIDToChagedState(-1)	{}
	virtual					~GState()	{}
	virtual void			Enter()=0;
	virtual void			Update(float fDelta)=0;
	virtual void			Exit(int nNextStateID)=0;
	virtual int				GetID()=0;
	virtual GMessageResult	Message(GMessage& msg) { return MR_FALSE; }
	virtual const wchar_t*		DebugString() { return L""; }
};

/// FSM TrasnsitionReactor
//
//		상태 전이 이벤트 함수 호출 순서:
//			FSM:OnPreChangeState() -> PrevState:Exit() -> NewState:Enter() -> FSM:OnChangedState() -> NewState:Message(상태전이에 사용됐던 메세지)
//
//		주의: 상태전이로 인한 메세지를 초기에 받을때 Enter()이후에 받게 된다.
//		해결책: 상태전이에 상관없이 받고 싶은 메세지는 GMoudleAI에 직접 넣도록 한다.
//

template <class _T>
class GStateTrasnsitionReactor
{
	friend class GStateMachine<_T>;
	typedef map<int, int>	MAP_TRASITION;
private:
	MAP_TRASITION			m_TransitionMap;
	GStateMachine<_T>*		m_pFSM;
private:
	bool					Transition(int nMsgType, int nStateID)					{ return m_pFSM->ChangeState(nStateID, nMsgType); }
	void					Add(int nMsgType, int nStateID)				{ m_TransitionMap.insert(MAP_TRASITION::value_type(nMsgType, nStateID)); }
	bool					Handle_MessageToTransition(int nMsgType)
	{
		MAP_TRASITION::iterator iter = m_TransitionMap.find(nMsgType);
		if (iter == m_TransitionMap.end())	return false;
		if (!Transition(nMsgType, iter->second))		return false;

		return true;
	}
public:
	GStateTrasnsitionReactor(GStateMachine<_T>* pFSM): m_pFSM(pFSM)		{}
	~GStateTrasnsitionReactor()											{}
};

/// 상태 머신
template <class _T>
class GStateMachine
{
public:
	typedef map<int, GStateTrasnsitionReactor<_T>* >	STATE_TRANSITION_REACTOR_MAP;
protected:
	_T*				m_pOwner;
private:
	GState<_T>*		m_pCurrentState;
	map<int, GState<_T>*>					m_StateMap;
	STATE_TRANSITION_REACTOR_MAP		m_TransitionReactorMap;
	bool			m_bLog;
	wstring			m_strDebugName;
protected:
	virtual void OnPreChangeState(GState<_T>* pNextState)		{}
	virtual void OnChangedState(GState<_T>* pNextState)			{}

	void SetCurrentState(GState<_T>* pState)	{m_pCurrentState = pState;}
	void SetCurrentState(int nStateID)
	{
		GState<_T>* pState = FindState(nStateID);
		if (pState) m_pCurrentState = pState;
		else m_pCurrentState = NULL;
	}
public:
	GStateMachine(_T* pOwner) : m_pOwner(pOwner), m_pCurrentState(NULL), m_bLog(false) {}
	virtual ~GStateMachine() { DeleteAll(); }

	int  GetSize()								{ return (int)m_StateMap.size(); }

	void AddTransitionRule(int nThisStateID, int nMsgType, int nStateID)			
	{ 
		STATE_TRANSITION_REACTOR_MAP::iterator iter = m_TransitionReactorMap.find(nThisStateID);
		if (iter == m_TransitionReactorMap.end())
		{
			_ASSERT(0);
			return;
		}

		GStateTrasnsitionReactor<_T>* pTransitionReactor = iter->second;
		pTransitionReactor->Add(nMsgType, nStateID); 
	}


	void Update(float fDelta)
	{
		if (m_pCurrentState)	m_pCurrentState->Update(fDelta);
	}

	GMessageResult Message(GMessage& msg)
	{
		if (IsRunForTest())
		{
			if (m_bLog)
			{	
				mlog("[FSM-%s] Message: %s(%d)\n", MLocale::ConvUTF16ToAnsi(m_strDebugName.c_str()).c_str(), GMessageIDStr[msg.m_nID], msg.m_nID);
			}
		}


		if (m_pCurrentState) 
		{
			STATE_TRANSITION_REACTOR_MAP::iterator iter = m_TransitionReactorMap.find(m_pCurrentState->GetID());
			if (iter == m_TransitionReactorMap.end())
			{
				_ASSERT(0);
			}

			GStateTrasnsitionReactor<_T>* pTransitionReactor = iter->second;
			pTransitionReactor->Handle_MessageToTransition(msg.m_nID);

			return m_pCurrentState->Message(msg);
		}
		return MR_FALSE;
	}

	bool ChangeState(GState<_T>* pNewState, int nMsgIDToChanged=-1)
	{
		if (!pNewState)	 return false;
		if (pNewState == m_pCurrentState) return false;

		if (IsRunForTest())
		{
			if (m_bLog)
			{
				mlog("[FSM-%s] ChangeState: %s -> %s\n", 
					MLocale::ConvUTF16ToAnsi(m_strDebugName.c_str()).c_str(),
					(m_pCurrentState) ? MLocale::ConvUTF16ToAnsi(m_pCurrentState->DebugString()).c_str() : "",
					(pNewState) ? MLocale::ConvUTF16ToAnsi(pNewState->DebugString()).c_str() : "");
			}
		}

		OnPreChangeState(pNewState);

		int nCurrID = (m_pCurrentState) ? m_pCurrentState->GetID() : -1;
		if (m_pCurrentState) m_pCurrentState->Exit(nCurrID);
		m_pCurrentState = pNewState;
		m_pCurrentState->SetMessageIDToChangedState(nMsgIDToChanged);
		if (m_pCurrentState) m_pCurrentState->Enter();

		OnChangedState(pNewState);
		

		return true;
	}

	bool ChangeState(int nStateID, int nMsgIDToChanged=-1)
	{
		GState<_T>* pState = FindState(nStateID);
		return ChangeState(pState, nMsgIDToChanged);
	}

	void AddState(GState<_T>* pState)
	{
		_ASSERT(pState);
		if (!pState)	return;
		if (m_StateMap.find(pState->GetID()) != m_StateMap.end())	return;

		m_StateMap.insert(map<int, GState<_T>*>::value_type(pState->GetID(), pState));
		m_TransitionReactorMap.insert(STATE_TRANSITION_REACTOR_MAP::value_type(pState->GetID(), new GStateTrasnsitionReactor<_T>(this)));

		pState->OnRegisterTrainstionRule();

		pState->m_pFSM = this;
	}

	void DeleteState(int nStateID)
	{
		map<int, GState<_T>*>::iterator itor = m_StateMap.find(nStateID);
		if (itor != m_StateMap.end())
		{
			delete (*itor).second;
		}
	}

	void DeleteAll()
	{
		for (STATE_TRANSITION_REACTOR_MAP::iterator itor = m_TransitionReactorMap.begin(); itor != m_TransitionReactorMap.end(); ++itor)
		{
			delete (*itor).second;
		}

		for (map<int, GState<_T>*>::iterator itor = m_StateMap.begin(); itor != m_StateMap.end(); ++itor)
		{
			delete (*itor).second;
		}

		m_StateMap.clear();
		m_TransitionReactorMap.clear();
	}

	GState<_T>*		FindState(int nStateID)
	{
		map<int, GState<_T>*>::iterator itor = m_StateMap.find(nStateID);
		if (itor != m_StateMap.end())
		{
			GState<_T>* pState = (*itor).second;
			return pState;
		}

		return NULL;
	}

	GState<_T>* GetCurrentState() { return m_pCurrentState; }
	int GetCurrentStateID() { if (m_pCurrentState) return m_pCurrentState->GetID(); return -1; }
	void SetLog(bool bLog) { m_bLog = bLog; }
	void SetDebugName(const wchar_t* szName) { m_strDebugName = szName; }
};

#endif