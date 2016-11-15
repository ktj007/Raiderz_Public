#pragma once

#include "PLogicChecker.h"

class PWorldIDChecker: public PLogicChecker
{
public:
	PWorldIDChecker(MUID uidClient, int nWorldID);
	
	bool Check(void);
	void OnFail(void);

private:
	MUID m_uidClient;
	int m_nWorldID;
};
