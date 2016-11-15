#pragma once

#include "XDummyHandler.h"

class XDummyHandler_TrialJumper: public XDummyHandler
{
public:
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	virtual const wchar_t* GetName()	{ return L"TrialJumper";	}

private:
	void MakeFieldList(void);
	int GetRandomFieldGroupID(void);

private:
	vector<int>	m_vecFieldGroupID;
};
