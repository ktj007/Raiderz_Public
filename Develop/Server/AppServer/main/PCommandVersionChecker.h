#pragma once

#include "PLogicChecker.h"

class PCommandVersionChecker: public PLogicChecker
{
public:
	PCommandVersionChecker(MUID uidClient, int nCommandVersion);
	
	bool Check(void);
	void OnFail(void);

private:
	MUID m_uidClient;
	int m_nCommandVersion;
};
