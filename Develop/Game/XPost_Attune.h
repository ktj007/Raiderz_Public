#ifndef _XPOST_ENCHANT_H
#define _XPOST_ENCHANT_H

#include "XNetClient.h"
#include "XCommandTable.h"
#include "CTransData.h"
#include "MCommandParameter.h"

inline void XPostItem_AttuneItem(SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID)
{
	//XPOSTCMD4(MC_ENCHANT_REQ, MCommandParameterInt(nTargetSlotType), MCommandParameterInt(nTargetSlotID));
}
inline void XPostItem_AttunePrepare()
{
	XPOSTCMD0(MC_SH_SA_PREPARE);
}

inline void XPostItem_AttuneCancel()
{
	XPOSTCMD0(MC_SH_SA_CANCEL);
}
#endif

