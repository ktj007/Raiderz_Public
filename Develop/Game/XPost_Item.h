#ifndef _XPOST_ITEM_H
#define _XPOST_ITEM_H

#include "XNetClient.h"
#include "XCommandTable.h"
#include "CTransData.h"
#include "MCommandParameter.h"

inline void XPostItem_EquipItem(SH_ITEM_SLOT_TYPE nSlotTypeFrom, int nSlotIDFrom, SH_ITEM_SLOT nSlotIDTo)
{
	XPOSTCMD3(MC_ITEM_EQUIP_REQ, MCommandParameterInt(nSlotTypeFrom), MCommandParameterInt(nSlotIDFrom), MCommandParameterChar(int8(nSlotIDTo)));
}

inline void XPostItem_Unequip(SH_ITEM_SLOT nItemSlot)
{
	XPOSTCMD1(MC_ITEM_UNEQUIP_REQ, MCommandParameterChar(int8(nItemSlot)));
}

inline void XPostItem_Use(int nInvenSlotID)
{
	XPOSTCMD1(MC_ITEM_USE_REQ, MCommandParameterInt(nInvenSlotID));
}

inline void XPostItem_Interact(int nInvenSlotID, const MUID& uidNPC)
{
	XPOSTCMD2(MC_ITEM_INTERACT_NPC_REQ, MCommandParameterInt(nInvenSlotID), MCommandParameterUID(uidNPC));
}

inline void XPostItem_DyePrepare(SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nDyeSlotID)
{
	XPOSTCMD3(MC_ITEM_DYE_PREPARE_REQ, MCommandParameterInt(nTargetSlotType), MCommandParameterInt(nTargetSlotID), MCommandParameterInt(nDyeSlotID));
}

inline void XPostItem_DyePrepareCancel()
{
	XPOSTCMD0(MC_ITEM_DYE_PREPARECANCEL_REQ);
}

inline void XPostItem_Dye(SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nDyeSlotID)
{
	XPOSTCMD3(MC_ITEM_DYE_REQ, MCommandParameterInt(nTargetSlotType), MCommandParameterInt(nTargetSlotID), MCommandParameterInt(nDyeSlotID));
}

inline void XPostItem_DropItem(SH_ITEM_SLOT_TYPE nSlotType, int nInvenSlotID, int nDropQuantity)
{
	XPOSTCMD3(MC_ITEM_DROP_REQ, MCommandParameterInt(nSlotType), MCommandParameterInt(nInvenSlotID), MCommandParameterInt(nDropQuantity));
}

inline void XPostItem_MoveItem(SH_ITEM_SLOT_TYPE nFromSlotType,  int nFromSlotID, int nFromItemID, SH_ITEM_SLOT_TYPE nToSlotType,  int nToSlotID, int nToItemID, int nAmount)
{
	XPOSTCMD7(MC_ITEM_MOVE_REQ, MCommandParameterInt(nFromSlotType), MCommandParameterInt(nFromSlotID), MCommandParameterInt(nFromItemID), MCommandParameterInt(nToSlotType), MCommandParameterInt(nToSlotID), MCommandParameterInt(nToItemID), MCommandParameterInt(nAmount));
}

inline void XPostItem_SortInvenSlot()
{
	XPOSTCMD0(MC_ITEM_SORT_INVEN_SLOT_REQ);
}

#endif

