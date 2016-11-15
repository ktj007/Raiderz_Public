#include "StdAfx.h"
#include "XDummyBot_Equip.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XDummyLazyCommandPoster.h"
#include "XDummyCollision.h"

XDummyBot_Equip::XDummyBot_Equip(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo), m_nPhase(PHASE_INIT)
{
	USES_CONVERSION_EX;	
}

XDummyBot_Equip::~XDummyBot_Equip(void)
{
	
}

void XDummyBot_Equip::OnRun( float fDelta )
{
	switch (m_nPhase)
	{
	case PHASE_INIT:
		{
			XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_CLEAR_INVENTORY_REQ);
			MakeTestItem();
			ChangePhase(PHASE_INIT_DONE);
		}
		break;
	}
}

minet::MCommandResult XDummyBot_Equip::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_CHANGE_FIELD:
		{
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"level"), MCommandParameterWideString(L"50"));
			XBIRDPOSTCMD0(pClient, MC_GM_GOD_REQ);
		}
		break;
	case MC_ITEM_ADD:
		{
			vector<TD_ITEM_INSERT> vecTDItemInsert;
			if (pCommand->GetBlob(vecTDItemInsert, 0)==false) return CR_ERROR;

			for each(const TD_ITEM_INSERT& tdItemInsert in vecTDItemInsert)
			{
				XItem* pItem = m_Inventory.InsertItem(tdItemInsert);
			}

			if (m_nTestItemCount <= m_Inventory.GetSize())
			{
				XItem* pItem = m_Inventory.GetRandomItem();
				if (NULL != pItem)
				{
					XLAZYCMDPOST3(pClient, MC_ITEM_EQUIP_REQ, MCommandParameterInt(SLOTTYPE_INVENTORY), MCommandParameterInt(pItem->m_nSlotID), MCommandParameterChar(pItem->m_pItemData->m_nItemSlot));
				}
				ChangePhase(PHASE_ADDITEM);
			}
		}
		break;
	case MC_ITEM_EQUIP:
		{
			int nSlotTypeFrom;
			int nSlotIDFrom;
			int8 nSlotIDTo;

			if (pCommand->GetParameter(&nSlotTypeFrom,	0, MPT_INT)==false) return CR_ERROR;
			if (pCommand->GetParameter(&nSlotIDFrom,	1, MPT_INT)==false) return CR_ERROR;
			if (pCommand->GetParameter(&nSlotIDTo,		2, MPT_CHAR)==false) return CR_ERROR;

			XItem* pItem = NULL;
			if (SLOTTYPE_INVENTORY == nSlotTypeFrom)
			{
				// 인벤토리에서 꺼낸다.
				pItem = m_Inventory.GetItem(nSlotIDFrom);
				if (pItem == NULL) return CR_TRUE;

				// 착용할 아이템 인벤토리에서 제거
				m_Inventory.PopItem(nSlotIDFrom);
			}
			else if (SLOTTYPE_EQUIP == nSlotTypeFrom)
			{
				// 장비창에서 해제
				pItem = m_EquipmentSlot.UnEquipItem(static_cast<SH_ITEM_SLOT>(nSlotIDFrom));
				if (pItem == NULL) return CR_TRUE;
			}
			else
			{
				return CR_TRUE;
			}

			// 아이템 착용
			m_EquipmentSlot.EquipItem(pItem, static_cast<SH_ITEM_SLOT>(nSlotIDTo));

			XItem* pEquipItem = m_EquipmentSlot.GetRandomItem();
			if (NULL != pEquipItem)
			{
				XLAZYCMDPOST1(pClient, MC_ITEM_UNEQUIP_REQ, MCommandParameterChar(pEquipItem->m_nSlotID));
			}

			ChangePhase(PHASE_EQUIP);
		}
		break;
	case MC_ITEM_UNEQUIP:
		{
			int nEquipSlotID = 0;
			int nInvenSlotID;

			if (pCommand->GetParameter(&nEquipSlotID,	0, MPT_CHAR)==false) return CR_ERROR;
			if (pCommand->GetParameter(&nInvenSlotID,	1, MPT_INT)==false) return CR_ERROR;

			XItem* pItem = m_EquipmentSlot.UnEquipItem(SH_ITEM_SLOT(nEquipSlotID));
			if (pItem)
			{
				pItem->m_nSlotID = nInvenSlotID;
				m_Inventory.PushItem(nInvenSlotID, pItem);
			}

			XItem* pInvenItem = m_Inventory.GetRandomItem();
			if (NULL != pInvenItem)
			{
				XLAZYCMDPOST3(pClient, MC_ITEM_EQUIP_REQ, MCommandParameterInt(SLOTTYPE_INVENTORY), MCommandParameterInt(pInvenItem->m_nSlotID), MCommandParameterChar(pInvenItem->m_pItemData->m_nItemSlot));
			}

			ChangePhase(PHASE_UNEQUIP);
		}
		break;
	case MC_ITEM_EQUIP_SWAPTOINVEN:
		{
			int nEquipSlotIDFrom = 0;
			int nEquipSlotIDTo = 0;
			int nUnequipSlotIDFrom = 0;
			int nUnequipSlotIDTo = 0;
			
			if (pCommand->GetParameter(&nEquipSlotIDFrom,	0, MPT_INT)==false) return CR_ERROR;
			if (pCommand->GetParameter(&nEquipSlotIDTo,		1, MPT_CHAR)==false) return CR_ERROR;
			if (pCommand->GetParameter(&nUnequipSlotIDFrom,	2, MPT_CHAR)==false) return CR_ERROR;
			if (pCommand->GetParameter(&nUnequipSlotIDTo,	3, MPT_INT)==false) return CR_ERROR;

			// 해제할 아이템 장비에서 꺼낸다.
			XItem* pUnequipItem = m_EquipmentSlot.UnEquipItem(SH_ITEM_SLOT(nUnequipSlotIDFrom));

			// 착용할 아이템 인벤토리에서 꺼낸다.
			XItem* pEquipItem = m_Inventory.GetItem(nEquipSlotIDFrom);
			if (pEquipItem == NULL) return CR_TRUE;

			// 착용할 아이템 인벤토리에서 제거
			m_Inventory.PopItem(nEquipSlotIDFrom);

			// 해제할 아이템 인벤토리에 넣는다.
			if (pUnequipItem != NULL)
			{
				pUnequipItem->m_nSlotID = nUnequipSlotIDTo;
				m_Inventory.PushItem(nUnequipSlotIDTo, pUnequipItem);
			}

			// 착용할 아이템 착용
			m_EquipmentSlot.EquipItem(pEquipItem, static_cast<SH_ITEM_SLOT>(nEquipSlotIDTo));


			// 착용 요청
			XItem* pInvenItem = m_Inventory.GetRandomItem();
			if (NULL != pInvenItem)
			{
				XLAZYCMDPOST3(pClient, MC_ITEM_EQUIP_REQ, MCommandParameterInt(SLOTTYPE_INVENTORY), MCommandParameterInt(pInvenItem->m_nSlotID), MCommandParameterChar(pInvenItem->m_pItemData->m_nItemSlot));
			}

			ChangePhase(PHASE_UNEQUIP);
		}
		break;
	}

	return CR_TRUE;
}

void XDummyBot_Equip::ChangePhase( PHASE nPhase )
{
	m_nPhase = nPhase;
}

void XDummyBot_Equip::MakeTestItem()
{		
	vector<int> vecItemID;
	vecItemID.push_back(1);		// 1HS
	vecItemID.push_back(2);		// 1HB
	vecItemID.push_back(3);		// 1HD
	vecItemID.push_back(4);		// 2HS
	vecItemID.push_back(5);		// Staff
	vecItemID.push_back(7);		// Shield

	//vecItemID.push_back(62000);	// Head
	vecItemID.push_back(9002);	// Body
	vecItemID.push_back(9003);	// Leg
	//vecItemID.push_back(62003);	// Hand
	vecItemID.push_back(9085);	// Feet

	//vecItemID.push_back(68000);	// Neck
	//vecItemID.push_back(68001);	// Finger
	//vecItemID.push_back(68008);	// Charm

	m_nTestItemCount = vecItemID.size() * 2;

	for each (int nItemID in vecItemID)
	{
		XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(nItemID), MCommandParameterInt(1));
		XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(nItemID), MCommandParameterInt(1));	
	}
}