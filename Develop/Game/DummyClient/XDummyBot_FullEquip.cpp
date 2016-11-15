#include "StdAfx.h"
#include "XDummyBot_FullEquip.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XDummyLazyCommandPoster.h"
#include "XDummyCollision.h"

namespace {
	const int MAXIMUM_ITEM_CNT = 80 - 14;	// 60은 인벤의 최대 갯수, 14는 파츠의 갯수(파츠별로 아이템 받아서 찰 수 있음)
}

XDummyBot_FullEquip::XDummyBot_FullEquip(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_requestedEquipmentTick(0)
{
}

XDummyBot_FullEquip::~XDummyBot_FullEquip(void)
{
}

void XDummyBot_FullEquip::OnRun( float fDelta )
{
}

minet::MCommandResult XDummyBot_FullEquip::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	assert(NULL!=pAgent);
	assert(NULL!=pCommand);
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	assert(NULL!=pClient);

	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		OnCommand_MC_CHAR_MYINFO(*pAgent, *pClient, *pCommand);
		break;

	case MC_ITEM_ADD:
		OnCommand_MC_ITEM_ADD(*pAgent, *pClient, *pCommand);
		break;
	}

	// 장비 요청 후 30초 후에 커맨드를 받으면 (일이 끝났건 끝나지 않았건 상관 없이) 제거한다
	if (0 != m_requestedEquipmentTick)
	{
		if (m_requestedEquipmentTick + 30 * SECOND < timeGetTime())
		{
			pAgent->DelBot(this);
			m_requestedEquipmentTick = 0;
		}
	}

	return CR_FALSE;
}

namespace {
	void PrepareInventoryForGetEquipments(XBirdClient& client,MCommand& cmd)
	{
		typedef vector<TD_ITEM> INVENTORY;
		INVENTORY inventory;

		if (false == cmd.GetBlob(inventory, 1))
		{
			mlog("XDummyBot_FullEquip::OnCommand_MC_CHAR_MYINFO() - 인벤토리 추출 실패!");
			return;
		}

		const int needDelCnt = inventory.size() - MAXIMUM_ITEM_CNT;
		if (0 >= needDelCnt)
		{
			return;
		}

		for (int i = 0; i < needDelCnt; ++i)
		{
			TD_ITEM& item(inventory[i]);
			XPOSTCMD3(MC_ITEM_DROP_REQ, MCommandParameterInt(SLOTTYPE_INVENTORY), MCommandParameterInt(item.m_nSlotID), MCommandParameterInt(item.m_nQuantity));
		}
	}

	int GetBestItemIdByParts(const SH_ITEM_SLOT& slot)
	{
		//todo 리소스에서 얻어오는 식으로 변경 해야 함
		switch (slot)
		{
		case ITEMSLOT_HEAD:			return 62760;
		case ITEMSLOT_FACE:			return 0;
		case ITEMSLOT_HANDS:		return 62763;
		case ITEMSLOT_FEET:			return 62764;
		case ITEMSLOT_BODY:			return 62761;
		case ITEMSLOT_LEG:			return 62762;
		case ITEMSLOT_LFINGER:		return 68014;
		case ITEMSLOT_RFINGER:		return 68015;
		case ITEMSLOT_NECK:			return 68018;
		case ITEMSLOT_CHARM:		return 0;
		case ITEMSLOT_LWEAPON:		return 0;
		case ITEMSLOT_RWEAPON:		return 70081;
		case ITEMSLOT_LWEAPON2:		return 0;
		case ITEMSLOT_RWEAPON2:		return 0;
		}

		return -1;
	}

	bool IsEquipBestItem(const vector<TD_ITEM>& equipments,const SH_ITEM_SLOT& slot)
	{
		assert(0 <= GetBestItemIdByParts(slot));
		if (0 == GetBestItemIdByParts(slot))
		{
			// 파츠에 대한 아이템이 없다면 그냥 넘어 간다
			return true;
		}

		for each (const TD_ITEM& item in equipments)
		{
			if (slot != item.m_nSlotID)
			{
				continue;
			}

			return item.m_nItemID == GetBestItemIdByParts(slot);
		}

		return false;
	}

	void RequestNewEquipment(XBirdClient& client,const SH_ITEM_SLOT& slot)
	{
		assert(0 < GetBestItemIdByParts(slot));
		XBIRDPOSTCMD2(&client, MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(GetBestItemIdByParts(slot)), MCommandParameterInt(1));
	}

	void ProcessForEquipment(XBirdClient& client,MCommand& cmd)
	{
		vector<TD_ITEM> qeuipments;
		if (false == cmd.GetBlob(qeuipments, 2))
		{
			mlog("XDummyBot_FullEquip::OnCommand_MC_CHAR_MYINFO() - 장비 추출 실패!");
			return;
		}

		for (SH_ITEM_SLOT slot = ITEMSLOT_DEFAULT_BEGIN; slot <= ITEMSLOT_DEFAULT_MAX; slot = scast<SH_ITEM_SLOT>(slot + 1))
		{
			if (true == IsEquipBestItem(qeuipments, slot))
			{
				continue;
			}

			RequestNewEquipment(client, slot);
		}
	}
}

void XDummyBot_FullEquip::OnCommand_MC_CHAR_MYINFO(XBirdDummyAgent& agent,XBirdClient& client,MCommand& cmd)
{
	if (0 != m_requestedEquipmentTick)
	{
		// 이미 캐릭터 정보를 받아보았고, 필요한 사항을 요청 하였다
		return;
	}

	PrepareInventoryForGetEquipments(client, cmd);
	ProcessForEquipment(client, cmd);

	m_requestedEquipmentTick = timeGetTime();
}

void XDummyBot_FullEquip::OnCommand_MC_ITEM_ADD(XBirdDummyAgent& agent,XBirdClient& client,MCommand& cmd)
{
	vector<TD_ITEM_INSERT> insertedItems;
	if (false == cmd.GetBlob(insertedItems, 0))
	{
		mlog("%s > XDummyBot_FullEquip::OnCommand_MC_ITEM_ADD() - 인벤토리 얻기 실패!", agent.GetName());
		return;
	}

	XBIRDPOSTCMD2(&client, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
	for each(const TD_ITEM_INSERT& item in insertedItems)
	{
		for (SH_ITEM_SLOT slot = ITEMSLOT_DEFAULT_BEGIN; slot <= ITEMSLOT_DEFAULT_MAX; slot = scast<SH_ITEM_SLOT>(slot + 1))
		{
			if (item.m_nID != GetBestItemIdByParts(slot))
			{
				continue;
			}

			XBIRDPOSTCMD3(&client, MC_ITEM_EQUIP_REQ, MCommandParameterInt(SLOTTYPE_INVENTORY), MCommandParameterInt(item.m_nSlotID), MCommandParameterChar(slot));
			return;
		}
	}
}
