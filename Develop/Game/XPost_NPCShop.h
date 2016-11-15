#ifndef _XPOST_NPCSHOP_H
#define _XPOST_NPCSHOP_H

#include "XNetClient.h"
#include "XCommandTable.h"

inline void XPostNPCShop_Buy(int nItemID, int nBuyAmount)
{
	XPOSTCMD2(MC_NPCSHOP_BUY_REQ, MCommandParameterInt(nItemID), MCommandParameterUShort(static_cast<USHORT>(nBuyAmount)));
}

inline void XPostNPCShop_Sell(int nInvenSlotID,  int nSellAmount)
{
	XPOSTCMD2(MC_NPCSHOP_SELL_REQ, MCommandParameterInt(nInvenSlotID), MCommandParameterUShort(static_cast<USHORT>(nSellAmount)));
}

inline void XPostNPCShop_SellVeryCommon()
{
	XPOSTCMD0(MC_NPCSHOP_SELL_VERYCOMMON_REQ);
}

inline void XPostNPCShop_RepairOne(SH_ITEM_SLOT_TYPE slotType, int nSlotID)
{
	XPOSTCMD2(MC_NPCSHOP_REPAIR_ONE_REQ, MCommandParameterChar((int8)slotType), MCommandParameterInt(nSlotID));
}

inline void XPostNPCShop_RepairAll()
{
	XPOSTCMD0(MC_NPCSHOP_REPAIR_ALL_REQ);
}

#endif