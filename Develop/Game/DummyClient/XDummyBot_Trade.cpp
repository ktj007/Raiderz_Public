#include "StdAfx.h"
#include "XDummyBot_Trade.h"
#include "XBirdDummyAgent.h"
#include "XDummyLazyCommandPoster.h"
#include "CCommandResultTable.h"
#include "XItemManager.h"

XDummyBot_Trade::XDummyBot_Trade(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_nPhase(PHASE_INIT)
, m_nOnceTradeMoney(10000)
, m_nTradeItemID(1) // 한손검
{
	const float TIME_TIMEOUT_SEC = 60.0;
	m_rgltTimeout.SetTickTime(TIME_TIMEOUT_SEC);
}

XDummyBot_Trade::~XDummyBot_Trade(void)
{
}

void XDummyBot_Trade::OnRun( float fDelta )
{
	if (m_rgltTimeout.IsReady(fDelta))
	{
		m_pAgent->SetStatusError();

		char szErrorLog[1024] = {0};
		sprintf_s(szErrorLog, 1024, "(%d)(%S)(%d) XDummyBot_Trade::OnRun(), Timeout!!\n", m_nPhase, m_pAgent->GetLoginID(), m_pAgent->GetID());
		mlog(szErrorLog);
	}

	XBirdClient* pClient = m_pAgent->GetClient();

	switch (m_nPhase)
	{
	case PHASE_INIT :
		{
			XDummyBot_Trade* pTraderBot = GetTraderBot();
			if (NULL == pTraderBot) return;
			m_pTraderBot = pTraderBot;

			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			XBIRDPOSTCMD0(pClient, MC_GM_GOD_REQ);
			XLAZYCMDPOST0(pClient, MC_GM_CLEAR_INVENTORY_REQ);
			XLAZYCMDPOST2(m_pAgent->GetClient(), MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(m_nTradeItemID), MCommandParameterInt(1));

			ChangePhase(PHASE_INIT_DONE);			
		}
		break;
	case PHASE_STARTREQ:
		{
			m_rgltTimeout.Start();

			wchar_t szMoney[16];
			wsprintf(szMoney, L"%d", m_nOnceTradeMoney*10);
			XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_SET_ME_REQ, MCommandParameterWideString(L"silver"), MCommandParameterWideString(szMoney));

			// 짝수 에이전트가 거래를 요청한다.
			if (m_pAgent->IsEvenNumberID() && PHASE_STARTREQ == m_pTraderBot->GetPhase())
			{
				XBIRDPOSTCMD1(pClient, MC_TRADE_START_REQ, MCommandParameterUID(m_pTraderBot->m_pAgent->GetUID()));
				ChangePhase(PHASE_STARTREQ_DONE);
			}
		}
		break;
	case PHASE_STARTACCEPT:
		{
			XBIRDPOSTCMD2(pClient, MC_TRADE_START_ACCEPT_REQ, MCommandParameterUID(m_pTraderBot->m_pAgent->GetUID()), MCommandParameterBool(true));
			ChangePhase(PHASE_STARTACCEPT_DONE);
		}
		break;
	case PHASE_PUTUPITEM:
		{
			XItem* pItem = m_Inventory.GetRandomItem();
			if (NULL == pItem)
			{
				ChangePhase(PHASE_PUTUPSILVER);
				return;
			}

			TD_TRADE_ITEM_PUTUP_REQ tdPutUpItem(pItem->m_nSlotID, 1);
			XBIRDPOSTCMD1(pClient, MC_TRADE_PUTUP_ITEM_REQ, MCommandParameterSingleBlob(tdPutUpItem));
			ChangePhase(PHASE_PUTUPITEM_DONE);
		}
		break;
	case PHASE_PUTUPSILVER:
		{
			XBIRDPOSTCMD1(pClient, MC_TRADE_PUTUP_MONEY_REQ, MCommandParameterInt(m_nOnceTradeMoney));
			ChangePhase(PHASE_PUTUPSILVER_DONE);
		}
		break;
	case PHASE_CONFIRM:
		{
			if (PHASE_CONFIRM == m_pTraderBot->GetPhase() || 
				PHASE_CONFIRM_DONE == m_pTraderBot->GetPhase())
			{
				XBIRDPOSTCMD0(pClient, MC_TRADE_CONFIRM_REQ);
				ChangePhase(PHASE_CONFIRM_DONE);
			}		
		}
		break;
	}
}

void XDummyBot_Trade::ChangePhase( PHASE nPhase )
{
	m_nPhase = nPhase;
}

minet::MCommandResult XDummyBot_Trade::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		{
			vector<TD_ITEM>	vecTDInventory;
			if (pCommand->GetBlob(vecTDInventory, 1)==false) return CR_ERROR;

			for each (TD_ITEM tdItem in vecTDInventory)
			{
				XItem* pNewItem = info.item->MakeNewItem(tdItem.m_nItemID);
				if (pNewItem == NULL) continue;

				pNewItem->m_pItemData	= info.item->GetItemData(tdItem.m_nItemID);
				pNewItem->m_nAmount		= tdItem.m_nQuantity;		
				pNewItem->m_nSlotID		= tdItem.m_nSlotID;

				m_Inventory.PushItem(tdItem.m_nSlotID, pNewItem);
			}

		}break;
	case MC_ITEM_ADD:
		{
			vector<TD_ITEM_INSERT> vecTDItemInsert;
			vector<TD_ITEM_INCREASE> vecTDItemIncrease;
			if (pCommand->GetBlob(vecTDItemInsert, 0)==false) return CR_ERROR;
			if (pCommand->GetBlob(vecTDItemIncrease, 1)==false) return CR_ERROR;

			for each(const TD_ITEM_INSERT& tdItemInsert in vecTDItemInsert)
			{
				m_Inventory.InsertItem(tdItemInsert);
			}
			for each(const TD_ITEM_INCREASE& tdItemIncrease in vecTDItemIncrease)
			{
				m_Inventory.IncreaseItemAmount(tdItemIncrease.m_nSlotID, tdItemIncrease.m_nAmount);
			}

			if (PHASE_INIT_DONE == m_nPhase)
			{
				ChangePhase(PHASE_STARTREQ);
			}
		}
		break;
	case MC_ITEM_REMOVE:
		{
			vector<TD_ITEM_DELETE>		vecTDItemDelete;
			vector<TD_ITEM_DECREASE>	vecTDDecreaseItem;
			if (pCommand->GetBlob(vecTDItemDelete, 0)==false) return CR_ERROR;
			if (pCommand->GetBlob(vecTDDecreaseItem, 1) == false)	return CR_ERROR;
			for each(const TD_ITEM_DELETE& tdItemDelete in vecTDItemDelete)
			{
				m_Inventory.PopItem(tdItemDelete.m_nSlotID);
			}
			for each (TD_ITEM_DECREASE tdItemDecrease in vecTDDecreaseItem)
			{
				m_Inventory.DecreaseItemAmount(tdItemDecrease.m_nSlotID, tdItemDecrease.m_nAmount);
			}
		}
		break;	
	case MC_TRADE_START_REQ_RELAY:
		{
			MUID uidReqPlayer;
			if (pCommand->GetParameter(&uidReqPlayer, 0, MPT_UID)==false)	return CR_ERROR;

			ChangePhase(PHASE_STARTACCEPT);			
		}
		break;
	case MC_TRADE_START:
		{			
			ChangePhase(PHASE_PUTUPITEM);			
		}
		break;
	case MC_TRADE_PUTUP_ITEM:
		{
			MUID uidReqPlayer;
			if (pCommand->GetParameter(&uidReqPlayer,	0,	MPT_UID)==false) return CR_ERROR;

			if (m_pAgent->GetUID() == uidReqPlayer)
			{
				ChangePhase(PHASE_PUTUPSILVER);
			}			
		}
		break;
	case MC_TRADE_PUTUP_MONEY:
		{
			MUID uidReqPlayer;
			if (pCommand->GetParameter(&uidReqPlayer,	0,	MPT_UID)==false) return CR_ERROR;

			if (m_pAgent->GetUID() == uidReqPlayer)
			{
				ChangePhase(PHASE_CONFIRM);
			}			
		}
		break;
	case MC_TRADE_COMPLETE:
		{
			ChangePhase(PHASE_STARTREQ);
		}
		break;
	}


	return CR_TRUE;
}

XDummyBot_Trade* XDummyBot_Trade::GetTraderBot()
{
	int nID = m_pAgent->GetID();
	int nTraderBotID = nID + 1;
	if (1 == nID % 2)
	{
		nTraderBotID = nID - 1;
	}

	XBirdDummyAgentMgr* pAgentMgr = m_pAgent->GetAgentMgr();
	if (NULL == pAgentMgr) return NULL;
	XBirdDummyAgent* pTraderAgent = pAgentMgr->GetAgentByID(nTraderBotID);
	if (NULL == pTraderAgent) return NULL;
	XDummyBot* pTraderBot = pTraderAgent->GetFrontBot();
	if (NULL == pTraderBot) return NULL;
	if (GetName() != pTraderBot->_GetName()) return NULL;

	return dynamic_cast<XDummyBot_Trade*>(pTraderBot);
}