#pragma once

#include "XDummyBot.h"

class XDummyLazyCommandPoster;
class XBirdClient;

class XDummyBot_Train : public XDummyBot
{
public:
	XDummyBot_Train(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Train(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Train"; }

protected:
	virtual void OnRun(float fDelta);

private:
	vector<int>					m_vecRecvTalentList;
	vector<int>					m_vecTalentList;
	int							m_nListCursor;
	MRegulator					m_rgltTimeout;

	bool InitParam();
	void PostTrain(XBirdClient* pClient);
};
