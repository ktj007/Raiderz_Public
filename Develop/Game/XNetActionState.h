#pragma once

#include "XState.h"
#include "XNetActionID.h"

class XModuleNetControl;
class XNetPlayer;

enum XNetActionFlag
{
	NACT_CANNOT_CHANGE_ANIMATION = 0x01,		/// 애니메이션을 바꿀 수 있는지 여부
};

class XNetActionState : public XState<XModuleNetControl>
{
protected:
	uint32					m_nFlags;
	void SetFlag(uint32 nFlags) { m_nFlags = nFlags; }
public:
	XNetActionState(XModuleNetControl* pOwner);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta) {}
	virtual void			PostUpdate(float fDelta) {}
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual int				GetID() { return NET_ACTION_STATE_NONE; }
	virtual const wchar_t*		GetDebugName() { return L"none"; }
	XNetPlayer*				GetOwnerPlayer();
	uint32 GetFlag()		{ return m_nFlags; }
	bool CheckFlag(uint32 nFlag)	{ return ((m_nFlags & nFlag) > 0); }
};


class XNetActionFSM : public XStateMachine<XModuleNetControl>
{
public:
	XNetActionFSM(XModuleNetControl* pOwner);
	virtual ~XNetActionFSM();

	bool CheckFlag(uint32 nFlag);
	XNetActionState* GetCurrentState() { return static_cast<XNetActionState*>(XStateMachine<XModuleNetControl>::GetCurrentState()); }
};
