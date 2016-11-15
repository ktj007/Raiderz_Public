#pragma once

#include "MActor.h"

namespace minet {

class MSendListener
{
public:
	virtual void OnSend(const MUID& muid,DWORD bytes_transferred) {}
};

class MSender : public MActor
{
protected:
	MSendListener*	m_pListener;
	void OnSend(MTcpAct* const pTcpAct,DWORD bytes_transferred);
public:
	MSender();
	virtual void ProcEvent( MAct* act, DWORD bytes_transferred );
	virtual void ProcError( MAct* act, DWORD bytes_transferred, DWORD error );
	void Init( MProactor* proactor );
	void SetListener(MSendListener* pListener) { m_pListener = pListener; }
};


} // namespace minet {