#ifndef _XSTATE_H
#define _XSTATE_H

#include <map>
using namespace std;
#include "XEvent.h"

template <class _T>
class XState
{
protected:
	_T*						m_pOwner;
public:
							XState(_T* pOwner) : m_pOwner(pOwner)	{}
	virtual					~XState()	{}
	virtual void			Enter(void* pParam=NULL)=0;
	virtual void			Update(float fDelta)=0;
	virtual void			PostUpdate(float fDelta)=0;
	virtual void			Exit()=0;
	virtual bool			CheckEnterable(void* pParam)	{ return true; }			// 해당 상태로 진입 가능한지 체크
	virtual bool			CheckExitable(int nNextStateID) { return true; }
	virtual XEventResult	Event(XEvent& msg)=0;
	virtual int				GetID()=0;
};

template <class _T>
class XStateMachine
{
private:
	_T*						m_pOwner;
	XState<_T>*				m_pCurrentState;
	XState<_T>*				m_pGlobalState;
	map<int, XState<_T>*>	m_StateMap;
	int						m_nPrevStateID;
	XState<_T>*	FindState(int nStateID)
	{
		map<int, XState<_T>*>::iterator itor = m_StateMap.find(nStateID);
		if (itor != m_StateMap.end())
		{
			XState<_T>* pState = (*itor).second;
			return pState;
		}
		return NULL;
	}
public:
	XStateMachine(_T* pOwner) : m_pOwner(pOwner), m_pCurrentState(NULL), m_pGlobalState(NULL), m_nPrevStateID(-1) {}
	virtual ~XStateMachine() { DeleteAll(); }

	void SetCurrentState(XState<_T>* pState)	{m_pCurrentState = pState;}
	void SetGlobalState(XState<_T>*	pState)		{m_pGlobalState = pState;}
	void SetCurrentState(int nStateID)
	{
		XState<_T>* pState = FindState(nStateID);
		if (pState) m_pCurrentState = pState;
		else m_pCurrentState = NULL;
	}
	void SetGlobalState(int nStateID)
	{
		XState<_T>* pState = FindState(nStateID);
		if (pState) m_pGlobalState = pState;
		else m_pGlobalState = NULL;
	}

	void Update(float fDelta)
	{
		if (m_pGlobalState)		m_pGlobalState->Update(fDelta);
		if (m_pCurrentState)	m_pCurrentState->Update(fDelta);
	}

	void PostUpdate(float fDelta)
	{
		if (m_pGlobalState)		m_pGlobalState->PostUpdate(fDelta);
		if (m_pCurrentState)	m_pCurrentState->PostUpdate(fDelta);
	}

	XEventResult Event(XEvent& msg)
	{
		if (m_pGlobalState)
		{
			XEventResult ret = m_pGlobalState->Event(msg);
			if (ret != MR_FALSE) return ret;
		}
		if (m_pCurrentState) return m_pCurrentState->Event(msg);
		return MR_FALSE;
	}

	bool ChangeState(XState<_T>* pNewState, void* pParam=NULL)
	{
		if (pNewState == m_pCurrentState) return false;
		if (m_pCurrentState && (!m_pCurrentState->CheckExitable(pNewState->GetID()))) return false;
		if (!pNewState->CheckEnterable(pParam)) return false;

		if (m_pCurrentState) 
		{
			m_pCurrentState->Exit();
			m_nPrevStateID = m_pCurrentState->GetID();
		}
		m_pCurrentState = pNewState;
		if (m_pCurrentState) m_pCurrentState->Enter(pParam);

		return true;
	}

	bool ChangeState(int nStateID, void* pParam=NULL)
	{
		XState<_T>* pState = FindState(nStateID);
		return ChangeState(pState, pParam);
	}

	void AddState(XState<_T>* pState)
	{
		m_StateMap.insert(map<int, XState<_T>*>::value_type(pState->GetID(), pState));
	}

	void DeleteState(int nStateID)
	{
		map<int, XState<_T>*>::iterator itor = m_StateMap.find(nStateID);
		if (itor != m_StateMap.end())
		{
			delete (*itor).second;
		}
	}

	void DeleteAll()
	{
		for (map<int, XState<_T>*>::iterator itor = m_StateMap.begin(); itor != m_StateMap.end(); ++itor)
		{
			delete (*itor).second;
		}
		m_StateMap.clear();
	}

	XState<_T>* GetCurrentState() { return m_pCurrentState; }
	int GetCurrentStateID() { if (m_pCurrentState) return m_pCurrentState->GetID(); return -1; }
	int GetPrevStateID()	{ return m_nPrevStateID; }
};






#endif