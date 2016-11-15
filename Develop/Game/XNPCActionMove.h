#pragma once

#include "XNPCActionState.h"

class XNPCAligner;

struct XNPCActionMoveParam
{
	vec3					vTarPos;
	float					fSpeed;
};

class XNPCActionMove : public XNPCActionState
{
protected:
	XNPCActionMoveParam		m_MoveParam;
	XNPCAligner*		m_pEnemyAligner;
public:
	XNPCActionMove(XModuleNonControl* pOwner);
	virtual ~XNPCActionMove();
	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();
	virtual void			Update(float fDelta);
	virtual XEventResult	Event(XEvent& msg);

	virtual int GetID()					{ return NPC_ACTION_STATE_MOVE;	}
	const wchar_t* GetDebugName()		{ return L"move";				}

};