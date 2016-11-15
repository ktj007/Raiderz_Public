#pragma once

#include "XDummyBot.h"

class XDummyBot_Trade : public XDummyBot
{
	enum PHASE
	{
		PHASE_INIT				=0,
		PHASE_INIT_DONE			=1,

		PHASE_STARTREQ			=2,
		PHASE_STARTREQ_DONE		=3,

		PHASE_STARTACCEPT		=4,
		PHASE_STARTACCEPT_DONE	=5,

		PHASE_PUTUPITEM			=6,
		PHASE_PUTUPITEM_DONE	=7,

		PHASE_PUTUPSILVER		=8,
		PHASE_PUTUPSILVER_DONE	=9,

		PHASE_CONFIRM			=10,
		PHASE_CONFIRM_DONE		=11,		
	};


public:
	XDummyBot_Trade(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo);
	~XDummyBot_Trade(void);

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	virtual const wchar_t* _GetName() { return GetName(); }
	static const wchar_t* GetName() { return L"Trade"; }	

	PHASE GetPhase() { return m_nPhase; }

protected:
	virtual	void OnRun(float fDelta);

private:
	void ChangePhase(PHASE nPhase);
	XDummyBot_Trade* GetTraderBot();	// 거래대상 봇

private:
	MRegulator					m_rgltTimeout;
	PHASE						m_nPhase;
	XMyInventory				m_Inventory;

	XDummyBot_Trade*			m_pTraderBot; 		// 거래대상 봇
	int							m_nTradeItemID;		// 거래하는 아이템	
	int							m_nOnceTradeMoney;	// 일회 거래되는 돈
};

//////////////////////////////////////////////////////////////////////////
/// 
/// 아이템 1개와 돈 10000 을 거래. 반복.
///
//////////////////////////////////////////////////////////////////////////