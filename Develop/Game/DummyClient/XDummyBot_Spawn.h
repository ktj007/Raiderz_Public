#pragma once


#include "XDummyBot.h"

class XDummyBot_Spawn : public XDummyBot
{
private:
	vec3	m_vCenter;
	float	m_fRadius;
	int		m_nFieldID;

	uint32	m_nLastPostTime;

	float	m_fTick;
	bool	m_bNPCOnly;
	float	m_fDespawnRate;

	list<int>		m_listNPC;
	vector<int>		m_vecNPCIDs;
protected:
	virtual void OnRun(float fDelta) override;

	void Post();

	void InitNPCInfoForTest();
	int GetRandomNPCID();
public:
	XDummyBot_Spawn(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);

	virtual ~XDummyBot_Spawn();
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Spawn"; }
};