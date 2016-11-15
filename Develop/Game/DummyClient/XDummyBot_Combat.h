#pragma once

class XDummySpawnMgr;

#include "XDummyBot.h"
#include "XDummyBot_Roam.h"

class XDummyBot_Combat : public XDummyBot_Roam
{
private:	
	void PostAttack();
	uint32 m_nLastPostAttackTime;

	float	m_fAttackTime;

protected:
	XDummySpawnMgr* m_pSpawnMgr;	
	UIID m_nTargetUIID;

protected:
	virtual void OnRun(float fDelta) override;
	virtual void SelectTarget();
	virtual bool IsValidTarget();
	virtual vec3 GetTargetPos();
	virtual void UpdateTarget(UIID nUIID);

public:
	XDummyBot_Combat(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);


	virtual ~XDummyBot_Combat();
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	static const wchar_t* GetName() { return L"Combat"; }
};