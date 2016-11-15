#pragma once

#include "MiNetPrerequisites.h"
#include "MAct.h"

namespace minet {

class MActor
{
protected:
	MProactor*	m_pProactor;
public:
	MActor() {	m_pProactor = NULL;	}

	// worker Thread에서 호출 ----
	virtual void ProcEvent(MAct* act, DWORD bytes_transferred) = 0;
	virtual void ProcError(MAct* act, DWORD bytes_transferred, DWORD erorr) = 0;
	// worker Thread에서 호출 ----^
};


} // namespace minet