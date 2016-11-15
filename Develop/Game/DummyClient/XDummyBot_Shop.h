#pragma once

#include "XDummyBot.h"

class XDummySpawnMgr;
class XDummyLazyCommandPoster;

class XDummyBot_Shop : public XDummyBot
{
	enum PHASE
	{
		PHASE_INIT						=0,
		PHASE_INIT_DONE					=1,		
		PHASE_INITREPEAT				=2,
		PHASE_INITREPEAT_DONE			=3,		
		PHASE_INTERACTION				=4,
		PHASE_INTERACTION_DONE			=5,
		PHASE_IELEMENT					=6,
		PHASE_IELEMENT_DONE				=7,		
		PHASE_BUY						=8,
		PHASE_BUY_DONE					=9,
		PHASE_SELL						=10,
		PHASE_SELL_DONE					=11,
		PHASE_INTERACTION_END			=14,
		PHASE_INTERACTION_END_DONE		=15,
	};

	struct ENTITY_NPC
	{
		MUID uid;
		UIID nUIID;
		vec3 vPos;
	};
public:
	XDummyBot_Shop(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Shop(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand) override;	

	static const wchar_t* GetName() { return L"Shop"; }

private:
	virtual void OnRun(float fDelta) override;	
	void ChangePhase( PHASE nPhase );

	bool IsInteractable();

private:
	MRegulator					m_rgltTimeout;

	PHASE	m_nPhase;
	MUID	m_nTargetUID;
	XDummySpawnMgr* m_pSpawnMgr;

	vector<int>		m_vecBuyableItemID;
	XMyInventory	m_Inventory;			///< 아이템 인벤토리	

	int				m_nShopNPCID;
	int				m_nCurrentSilver;
};
