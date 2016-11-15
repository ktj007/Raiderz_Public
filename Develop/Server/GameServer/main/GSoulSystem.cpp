#include "StdAfx.h"
#include "GSoulSystem.h"
#include "GEntityPlayer.h"
#include "GLootInfo.h"
#include "GModuleAI.h"
#include "CSSoul.h"
#include "CCommandTable.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GServer.h"
#include "GCommand.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GPartySystem.h"
#include "GDropSouls.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"

GSoulSystem::GSoulSystem()
{
}

GSoulSystem::~GSoulSystem()
{
}

void GSoulSystem::DrainSoul(GEntityPlayer* pPlayer, GEntityNPC* pVictimNPC)
{
	if (NULL == pPlayer) return;
	if (NULL == pVictimNPC) return;	

	if (true == pVictimNPC->GetNPCLoot().GetDropList().GetDropSouls().IsEmpty()) return;

	if (!gsys.pPartySystem->IsPartyMember(pPlayer))
	{
		doDrainSoul(pPlayer, pVictimNPC);
	}
	else
	{
		MUID uidParty = pPlayer->GetPartyUID();
		GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
		_ASSERT(pParty != NULL);

		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
		{
			MUID uidMember = it->first;

			GEntityPlayer* pMember = pPlayer->FindPlayer(uidMember);
			if (pMember == NULL)	continue;

			doDrainSoul(pMember, pVictimNPC);			
		}
	}

	pVictimNPC->GetNPCLoot().GetDropList().GetDropSouls().Clear();
}

void GSoulSystem::doDrainSoul(GEntityPlayer* pPlayer, GEntityNPC* pVictimNPC)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();	
	GItem* pCharmItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_CHARM);
	if (NULL == pCharmItem) return;
	GItemData* pCharmItemData = pCharmItem->m_pItemData;
	if (NULL == pCharmItemData) return;
	if (ARMOR_AMULET != pCharmItemData->m_nArmorType) return;

	int nSoulQuantity = pVictimNPC->GetNPCLoot().GetDropList().GetDropSouls().GetQuantity(pPlayer->GetCID(), pCharmItemData->m_nSoulType);
	if (pCharmItemData->m_nSoulMaxQuantity - nSoulQuantity < pCharmItem->m_nSoulCount)
	{
		nSoulQuantity = pCharmItemData->m_nSoulMaxQuantity - pCharmItem->m_nSoulCount;
	}
	if (0 == nSoulQuantity) return pPlayer->RouteSystemMsg(CR_FAIL_SOUL_FULL_AMULET_SOULQUANTITY);

	if (false == gsys.pDBManager->ItemUpdateSoulCnt(pPlayer->GetCID(), pCharmItem->m_nSlotType, pCharmItem->m_nSlotID, pCharmItem->m_nIUID, pCharmItem->m_nSoulCount + nSoulQuantity)) 
		return;

	pCharmItem->m_nSoulCount += nSoulQuantity;

	MCommand* pNewCmdMe = MakeNewCommand(MC_SOUL_DRAIN_ME, 
		3,
		NEW_UID(pVictimNPC->GetUID()),
		NEW_CHAR(ITEMSLOT_CHARM),
		NEW_UCHAR(nSoulQuantity));

	pPlayer->RouteToMe(pNewCmdMe);

	MCommand* pNewCmdOther = MakeNewCommand(MC_SOUL_DRAIN_OTHER, 
		4,
		NEW_UID(pVictimNPC->GetUID()),
		NEW_UID(pPlayer->GetUID()),
		NEW_UCHAR(pCharmItemData->m_nSoulType),
		NEW_UCHAR(nSoulQuantity));

	pPlayer->RouteToThisCell(pNewCmdOther, pPlayer->GetUID());
}