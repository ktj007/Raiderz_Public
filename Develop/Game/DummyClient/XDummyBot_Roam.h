#pragma once

#include "XDummyBot.h"

class XDummyBot_Roam : public XDummyBot
{
protected:
	float	m_fStopTime;
	float	m_fSpeed;

	vec3	m_vGoalPos;
	vec3	m_vPosition;
	float	m_fElapsedPostTime;

	

	void Roam();
	void SetNextRandomGoalPos();
	void UpdateMovingPost( float fDeltaTimee );

	virtual void OnRun(float fDelta) override;
public:
	XDummyBot_Roam(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	virtual ~XDummyBot_Roam() {}

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Roam"; }
};