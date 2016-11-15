#ifndef _XPOST_ENCHANT_H
#define _XPOST_ENCHANT_H

#include "XNetClient.h"
#include "XCommandTable.h"
#include "CTransData.h"
#include "MCommandParameter.h"

inline void XPostItem_EnchantItem(SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nEnchantStoneSlotID, int nEnchantAgentSlotID)
{
	XPOSTCMD4(MC_ENCHANT_REQ, MCommandParameterInt(nTargetSlotType), MCommandParameterInt(nTargetSlotID), MCommandParameterInt(nEnchantStoneSlotID), MCommandParameterInt(nEnchantAgentSlotID));
}

inline void XPostItem_EnchantCheck(SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nEnchantStoneSlotID, int nEnchantAgentSlotID)
{
	XPOSTCMD4(MC_ENCHANT_CHECK_REQ, MCommandParameterInt(nTargetSlotType), MCommandParameterInt(nTargetSlotID), MCommandParameterInt(nEnchantStoneSlotID), MCommandParameterInt(nEnchantAgentSlotID));
}

inline void XPostItem_EnchantPrepare()
{
	XPOSTCMD0(MC_ENCHANT_PREPARE);
}

inline void XPostItem_EnchantCancel()
{
	XPOSTCMD0(MC_ENCHANT_CANCEL);
}

#endif

