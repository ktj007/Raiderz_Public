#include "StdAfx.h"
#include "XDummyBot_Shop.h"
#include "XBirdDummyAgent.h"
#include "XDummySpawnMgr.h"
#include "XItemManager.h"
#include "XDummyLazyCommandPoster.h"

XDummyBot_Shop::XDummyBot_Shop( XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo )
: XDummyBot(pAgent, pAgentInfo)
, m_nPhase(PHASE_INIT)
, m_nTargetUID(MUID::ZERO)
, m_nCurrentSilver(0)
, m_nShopNPCID(102205)			// 상인 한스
{
	m_pSpawnMgr = new XDummySpawnMgr(pAgentInfo, false);

	const float TIME_TIMEOUT_SEC = 60.0;
	m_rgltTimeout.SetTickTime(TIME_TIMEOUT_SEC);
}

XDummyBot_Shop::~XDummyBot_Shop(void)
{
	SAFE_DELETE(m_pSpawnMgr);
}


void XDummyBot_Shop::OnRun( float fDelta )
{
	m_pSpawnMgr->Update(fDelta, m_pAgent->GetClient(), m_pAgent->GetCenter());

	if (m_rgltTimeout.IsReady(fDelta))
	{
		m_pAgent->SetStatusError();

		char szErrorLog[1024] = {0};
		sprintf_s(szErrorLog, 1024, "(%d)(%S)(%d) XDummyBot_Shop::OnRun(), Timeout!!\n", m_nPhase, m_pAgent->GetLoginID(), m_pAgent->GetID());
		mlog(szErrorLog);
	}

	XBirdClient* pClient = m_pAgent->GetClient();
	switch (m_nPhase)
	{
	case PHASE_INIT:
		{
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));				
			XBIRDPOSTCMD0(pClient, MC_GM_GOD_REQ);			
			XBIRDPOSTCMD3(pClient, MC_GM_REQUEST_SPAWN, MCommandParameterInt(m_nShopNPCID), MCommandParameterInt(1), MCommandParameterVector(vec3::ZERO));			
			
			ChangePhase(PHASE_INIT_DONE);
		}break;	
	case PHASE_INITREPEAT:
		{
			XBIRDPOSTCMD0(pClient, MC_GM_CLEAR_INVENTORY_REQ);
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"silver"), MCommandParameterWideString(L"400000000"));
			
			ChangePhase(PHASE_INITREPEAT_DONE);
		}break;	
	case PHASE_INTERACTION:
		{
			m_rgltTimeout.Start();

			XNPCForDummy* pTarget = m_pSpawnMgr->GetNPC(m_nTargetUID);
			if (NULL == pTarget) return;

			XBIRDPOSTCMD3(pClient, MC_INTERACTION_INTERACTION_REQ, MCommandParameterUID(m_nTargetUID), MCommandParameterVector(pTarget->vPos), MCommandParameterVector(vec3::ZERO));
			ChangePhase(PHASE_INTERACTION_DONE);
		}break;
	case PHASE_IELEMENT:
		{
			XBIRDPOSTCMD2(pClient, MC_NPCINTERACTION_IELEMENT_REQ, MCommandParameterUID(m_nTargetUID), MCommandParameterInt(4));
			ChangePhase(PHASE_IELEMENT_DONE);
		}break;
	case PHASE_BUY:
		{
			int nItemID = XMath::RandomPickUp(m_vecBuyableItemID);
			XItemData* pItemData = info.item->GetItemData(nItemID);
			if (NULL == pItemData)
			{
				ChangePhase(PHASE_SELL);
			}
			else
			{
				XBIRDPOSTCMD2(pClient, MC_NPCSHOP_BUY_REQ, MCommandParameterInt(nItemID), MCommandParameterUShort(static_cast<USHORT>(XMath::RandomNumber(1, pItemData->m_nStackAmount))));
				ChangePhase(PHASE_BUY_DONE);
			}			
		}break;
	case PHASE_SELL:
		{
			XItem* pItem = m_Inventory.GetRandomSellableItem();
			if (NULL == pItem)
			{
				ChangePhase(PHASE_INTERACTION_END);
			}
			else
			{
				XBIRDPOSTCMD2(pClient, MC_NPCSHOP_SELL_REQ, MCommandParameterInt(pItem->m_nSlotID), MCommandParameterUShort(static_cast<USHORT>(XMath::RandomNumber(1, pItem->m_nAmount))));
				ChangePhase(PHASE_SELL_DONE);
			}
			
		}break;	
	case PHASE_INTERACTION_END:
		{
			XBIRDPOSTCMD0(pClient, MC_NPCINTERACTION_END_REQ);			
			ChangePhase(PHASE_INTERACTION_END_DONE);
		}break;
	}
}

minet::MCommandResult XDummyBot_Shop::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	__super::OnCommand(pAgent, pCommand);

	m_pSpawnMgr->OnCommand(pCommand);

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
	case MC_GM_SPAWN:
		{
			vector<MUID> vecNPCUID;
			if (pCommand->GetBlob(vecNPCUID,	0)==false) return CR_ERROR;
			if (vecNPCUID.empty()) return CR_ERROR;

			m_nTargetUID = vecNPCUID.front();
			ChangePhase(PHASE_INITREPEAT);
		}break;
	case MC_CHAR_UPDATE_MONEY:
		{
			if (pCommand->GetParameter(&m_nCurrentSilver,		0, MPT_INT)==false) return CR_ERROR;

			if (IsInteractable())
			{
				ChangePhase(PHASE_INTERACTION);
			}			
		}break;
	case MC_NPCINTERACTION_INTERACTION:
		{
			vector<TD_INTERACTION_ELEMENT> vecTDIElement;

			if (pCommand->GetBlob(vecTDIElement, 2)==false) return CR_ERROR;

			if (true == vecTDIElement.empty())
			{
				return CR_ERROR;
			}

			ChangePhase(PHASE_IELEMENT);
		}break;
	case MC_NPCSHOP_SHOW:
		{
			vector<int> vecBuyableItemID;
			if (pCommand->GetBlob(vecBuyableItemID,	0)==false) return CR_ERROR;

			m_vecBuyableItemID.clear();

			for each (int nItemID in vecBuyableItemID)
			{
				XItemData* pItemData = info.item->GetItemData(nItemID);
				if (NULL == pItemData) continue;
				if (true == pItemData->m_bUnique) continue;
				if (false == pItemData->m_bSellable) continue;

				m_vecBuyableItemID.push_back(nItemID);
			}

			ChangePhase(PHASE_BUY);
		}break;
	case MC_NPCSHOP_BUY:
		{
			ChangePhase(PHASE_SELL);
		}break;
	case MC_NPCSHOP_SELL:
		{
			ChangePhase(PHASE_INTERACTION_END);	
		}break;
	case MC_NPCINTERACTION_END:
		{
			ChangePhase(PHASE_INITREPEAT);
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
		}break;
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

			if (IsInteractable())
			{
				ChangePhase(PHASE_INTERACTION);
			}

		}break;
	}

	return CR_FALSE;
}

void XDummyBot_Shop::ChangePhase( PHASE nPhase )
{
	m_nPhase = nPhase;
}

bool XDummyBot_Shop::IsInteractable()
{
	return 400000000 == m_nCurrentSilver && m_Inventory.IsEmpty();
}
