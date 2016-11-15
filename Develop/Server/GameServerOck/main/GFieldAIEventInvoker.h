#pragma once

#include "GAIEventInvoker.h"

class GField;

class GFieldAIEventInvoker: public GAIEventInvoker, public MTestMemPool<GFieldAIEventInvoker>
{
public:
	GFieldAIEventInvoker(GField* pField);
	virtual ~GFieldAIEventInvoker();

	void EnterPlayer(void);
	void LeavePlayer(void);

	static void InvokeEnterPlayer(MUID uidField);
	static void InvokeLeavePlayer(MUID uidField);

private:
	GField* m_pOwnerField;
	bool m_isPlayerEmpty;
};
